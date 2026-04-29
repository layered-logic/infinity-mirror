/*
 * Captive-portal DNS hijack — see captive_dns.h for the contract.
 *
 * Wire format reminder (RFC 1035 §4.1):
 *   Header (12 bytes): ID, FLAGS, QDCOUNT, ANCOUNT, NSCOUNT, ARCOUNT
 *   Question:          QNAME (length-prefixed labels, terminated by 0)
 *                      QTYPE (2), QCLASS (2)
 *   Answer (we add):   NAME (use compression pointer 0xC00C → question name)
 *                      TYPE (2), CLASS (2), TTL (4), RDLENGTH (2), RDATA
 *
 * We respond to *every* query with our own A record regardless of
 * QTYPE — clients asking for AAAA / SRV / TXT etc. will get an
 * "answer" that doesn't match what they asked for, which makes them
 * fail fast and (usually) retry as A. That's fine for captive-portal
 * use; the goal is to make at least one resolution succeed and route
 * the OS to our HTTP server.
 */

#include "captive_dns.h"

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <lwip/sockets.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "state_bus.h"
#include "state_bus_events.h"

static const char *TAG = "captive_dns";

#define LL_DNS_PORT       53
#define LL_DNS_RX_MAX     512   /* RFC 1035 max UDP DNS message size */
#define LL_DNS_TASK_PRIO  4
#define LL_DNS_TASK_STACK 3072

/* Hijack target — every A query answers with this address. The dev
 * SoftAP gateway is 192.168.4.1 (DHCP server in lwip's default config
 * for esp_netif_create_default_wifi_ap). Hardcoded for now; if a
 * future SoftAP scheme uses a different subnet, look up the gateway
 * from the AP netif at start time instead. */
static const uint8_t HIJACK_IP[4] = { 192, 168, 4, 1 };

static TaskHandle_t g_task;
static int          g_sock = -1;
static volatile bool g_running;

/* Skip past the QNAME (sequence of length-prefixed labels terminated by
 * a zero byte) plus QTYPE + QCLASS, return the offset of the first byte
 * past the question. Returns -1 on malformed input. */
static int skip_question(const uint8_t *buf, int len)
{
    int p = 12;  /* DNS header */
    while (p < len) {
        uint8_t lbl = buf[p];
        if (lbl == 0) {
            p += 1;          /* null terminator */
            p += 4;          /* QTYPE + QCLASS */
            return (p <= len) ? p : -1;
        }
        if ((lbl & 0xC0) != 0) {
            /* Compression pointer in a question section is unusual but
             * legal — handle conservatively by treating it as a 2-byte
             * terminator. */
            p += 2;
            p += 4;
            return (p <= len) ? p : -1;
        }
        p += 1 + lbl;
    }
    return -1;
}

static int build_response(const uint8_t *q, int qlen, uint8_t *out, int outcap)
{
    int q_end = skip_question(q, qlen);
    if (q_end < 0 || q_end > outcap) return -1;

    /* Echo the question header + question itself, then mutate flags +
     * counts in place. */
    memcpy(out, q, q_end);

    /* Flags: QR=1 (response), AA=1 (authoritative), RA=1 (recursion
     * available — clients prefer this for non-fallback flows). RCODE=0. */
    out[2] = 0x84;
    out[3] = 0x80;

    /* QDCOUNT stays 1; ANCOUNT=1; NSCOUNT=0; ARCOUNT=0. */
    out[6] = 0x00; out[7] = 0x01;
    out[8] = 0x00; out[9] = 0x00;
    out[10] = 0x00; out[11] = 0x00;

    /* Answer record. Total = 2 (name pointer) + 2 (TYPE) + 2 (CLASS) +
     * 4 (TTL) + 2 (RDLENGTH) + 4 (RDATA) = 16 bytes. */
    int p = q_end;
    if (p + 16 > outcap) return -1;

    /* NAME = pointer to question name at offset 12 (start of QNAME). */
    out[p++] = 0xC0; out[p++] = 0x0C;
    /* TYPE = A (1). */
    out[p++] = 0x00; out[p++] = 0x01;
    /* CLASS = IN (1). */
    out[p++] = 0x00; out[p++] = 0x01;
    /* TTL = 60s. Long enough that the OS doesn't hammer us; short
     * enough that clients refresh after a config change. */
    out[p++] = 0x00; out[p++] = 0x00;
    out[p++] = 0x00; out[p++] = 0x3C;
    /* RDLENGTH = 4. */
    out[p++] = 0x00; out[p++] = 0x04;
    /* RDATA. */
    out[p++] = HIJACK_IP[0];
    out[p++] = HIJACK_IP[1];
    out[p++] = HIJACK_IP[2];
    out[p++] = HIJACK_IP[3];

    return p;
}

static void dns_task(void *arg)
{
    (void)arg;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        ESP_LOGE(TAG, "socket: errno=%d", errno);
        g_running = false;
        vTaskDelete(NULL);
        return;
    }

    struct sockaddr_in bind_addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(LL_DNS_PORT),
        .sin_addr.s_addr = htonl(INADDR_ANY),
    };
    if (bind(sock, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) < 0) {
        ESP_LOGE(TAG, "bind :%d failed: errno=%d", LL_DNS_PORT, errno);
        close(sock);
        g_running = false;
        vTaskDelete(NULL);
        return;
    }

    g_sock = sock;
    ESP_LOGI(TAG, "listening on udp :%d, hijacking all A queries to %u.%u.%u.%u",
             LL_DNS_PORT,
             HIJACK_IP[0], HIJACK_IP[1], HIJACK_IP[2], HIJACK_IP[3]);

    uint8_t rx[LL_DNS_RX_MAX];
    uint8_t tx[LL_DNS_RX_MAX + 16];

    while (g_running) {
        struct sockaddr_in src;
        socklen_t srclen = sizeof(src);

        int n = recvfrom(sock, rx, sizeof(rx), 0,
                         (struct sockaddr *)&src, &srclen);
        if (n < 12) {
            if (n < 0 && !g_running) break;  /* socket closed for shutdown */
            continue;                        /* runt or transient */
        }

        int outlen = build_response(rx, n, tx, sizeof(tx));
        if (outlen < 0) {
            ESP_LOGW(TAG, "malformed query (%d bytes) from %s - dropping",
                     n, inet_ntoa(src.sin_addr));
            continue;
        }

        int sent = sendto(sock, tx, outlen, 0,
                          (struct sockaddr *)&src, srclen);
        if (sent < 0) {
            ESP_LOGW(TAG, "sendto: errno=%d", errno);
        }
    }

    close(sock);
    g_sock = -1;
    ESP_LOGI(TAG, "stopped");
    vTaskDelete(NULL);
}

static bool is_ap_mode(void)
{
    wifi_mode_t mode;
    if (esp_wifi_get_mode(&mode) != ESP_OK) return false;
    return mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA;
}

static void start_responder(void)
{
    if (g_running) return;
    if (!is_ap_mode()) {
        ESP_LOGI(TAG, "not in AP mode - DNS hijack stays off");
        return;
    }
    g_running = true;
    BaseType_t ok = xTaskCreate(dns_task, "ll_dns", LL_DNS_TASK_STACK,
                                NULL, LL_DNS_TASK_PRIO, &g_task);
    if (ok != pdPASS) {
        ESP_LOGE(TAG, "xTaskCreate failed");
        g_running = false;
    }
}

static void stop_responder(void)
{
    if (!g_running) return;
    g_running = false;
    /* Closing the socket from this task wakes recvfrom on the dns_task
     * with errno set; the loop checks g_running and exits cleanly. */
    if (g_sock >= 0) {
        shutdown(g_sock, SHUT_RDWR);
    }
    /* Don't vTaskDelete from here — let the task self-terminate after
     * closing its socket so we don't leak the FD. */
}

static void on_wifi_connected(void *arg, esp_event_base_t base,
                              int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    start_responder();
}

static void on_wifi_disconnected(void *arg, esp_event_base_t base,
                                 int32_t id, void *data)
{
    (void)arg; (void)base; (void)id; (void)data;
    stop_responder();
}

esp_err_t ll_captive_dns_init(void)
{
    /* Nothing to do at init — the responder task is started lazily
     * when AP comes up. Here for symmetry with the other modules and
     * as a hook for future stats/diag plumbing. */
    ESP_LOGI(TAG, "init: responder deferred until AP up");
    return ESP_OK;
}

esp_err_t ll_captive_dns_subscribe(void)
{
    esp_err_t err = esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_CONNECTED,
        on_wifi_connected, NULL, NULL);
    if (err != ESP_OK) return err;

    return esp_event_handler_instance_register_with(
        ll_state_bus_get_loop(),
        LL_STATE_EVENT_BASE, LL_EV_WIFI_DISCONNECTED,
        on_wifi_disconnected, NULL, NULL);
}
