#pragma once

/*
 * V1 Pro shipping PCB — ESP32-C3-MINI-1-N4.
 *
 * Hardware home: D:\PCBs\Infinity_Mirror_C3 (own git repo).
 * Built as JLCPCB order Y11, 15 boards, production 2026-08-17.
 *
 * Pins below come from the shipped netlist (tools/gen_sch.py COMPS) and were
 * CONFIRMED ON HARDWARE during board #1 first-article bring-up, 2026-08-25:
 * the chip enumerated over native USB and the LED chain was driven end to end
 * (32 px lit, colour order verified). Measured, not planned.
 *
 * Not a devkit — read the notes before assuming devkit behaviour.
 */

/* ---- required by board.h ---- */

/* GPIO10 -> U4 (SN74LV1T34, 3V3->5V) -> R11 330R -> J2 pin 2 "DAT".
 * NOT GPIO2. Stock WLED defaults the C3 LED pin to GPIO2, which on this board
 * is a boot strap tied to +3V3 through R2 and connects to nothing else. R18
 * (10k) pulls GPIO10 down so the strip stays quiet through boot. */
#define LL_PIN_LED_DATA        10

/* SW2, silkscreened "BOOT", bottom face. Active low, R9 10k pullup to +3V3.
 * SHARED WITH THE C3 BOOT STRAP — held low at reset it forces ROM download
 * mode, so any boot-time read of this pin must tolerate that overlap. Doubles
 * as the user/provisioning button once running. */
#define LL_PIN_BUTTON_PRIMARY  9

#define LL_HAS_WIFI            1
#define LL_HAS_BLE             1
#define LL_HAS_802154          0      /* C3 has no 15.4 radio */

/* Production panel is 6x6 / 32 LEDs. Verified against a real 32px strip. */
#define LL_LED_COUNT_DEFAULT   32

/* ---- optional / conventional ---- */

/* SW1, silkscreened "RESET", bottom face — wired to the module's hardware EN
 * pin (R1 10k pullup + C15 1uF RC), NOT to a GPIO. Firmware cannot read it and
 * must not try; the button module skips registration on -1. */
#define LL_PIN_BUTTON_RESET    (-1)

/* No LED-rail power gate on rev 1: J2's +5V comes straight off the buck with
 * nothing in between. Candidate for rev 2 — holding the LED rail off until
 * firmware is up is fix #3 in the C3 repo's V2_NOTES.md item 2. */
#define LL_PIN_LED_POWER_EN    (-1)

#define LL_PARTITION_SCHEME    "ab_with_factory"   /* 4MB, A/B + factory */

/* Verified on hardware 2026-08-25: driving pure red rendered red. */
#define LL_LED_COLOR_ORDER     GRB

/* ---- board-specific extras (not part of the board.h contract) ---- */

/* I2C expansion header J3, silk "GND 3V3 SDA SCL". 4.7k pullups R3/R4 fitted;
 * measured 4.6k on board #1. Header rail is +3V3. */
#define LL_PIN_I2C_SDA         4
#define LL_PIN_I2C_SCL         5

/* VBUS_RAW sense divider R12/R13 = 100k/10k -> 11:1. 15V in reads 1.364V at
 * the pin. TELEMETRY ONLY — DESIGN_SPEC rev 6 explicitly demoted this from
 * load-bearing when the mode-specific current governor was deleted. Do not
 * gate current limits on it. */
#define LL_PIN_VBUS_SENSE      3
#define LL_VBUS_SENSE_RATIO    11

/* Power-good telemetry, both brought out to probe pads on the bottom face. */
#define LL_PIN_PG_EFUSE        6   /* TP1 "IO6". U7 TPS259474A PG, open drain,
                                    * R27 100k pullup. Released HIGH once inrush
                                    * completes and PGTH is satisfied. */
#define LL_PIN_PG_PD           7   /* TP2 "IO7". U8 CH221K PG, open drain,
                                    * R30 100k pullup. Asserts LOW on successful
                                    * PD negotiation. Only signal on the board
                                    * that can flag a PD renegotiation. */

/* ---- reserved: do not allocate ---- */
/* GPIO2, GPIO8 — C3 boot straps, 10k pullups R2/R8. Must be high at reset. */
/* GPIO18, GPIO19 — native USB Serial/JTAG through U6 (USBLC6-2SC6) to J1.
 * The only flashing and console path on the board. */
/* GPIO0, GPIO1 — module pins 12/13, unconnected on rev 1. The two genuinely
 * free pins if rev 2 needs one (e.g. an addressable status LED). */

/* ---- bring-up caveat, rev 1 ----
 * This board does NOT reliably cold-start on an ordinary 5V USB port. Measured
 * on board #1: VBUS_RAW sits at 5.0V but +5V and +3V3 both collapse to 2.3V,
 * below the C3's 3.0V minimum, so the chip never starts and never enumerates.
 * The collapse is on-board, between the eFuse input and the 5V rail — not the
 * host port folding back. Wired flashing needs a USB-C PD port. Under
 * investigation: V2_NOTES.md item 2 in the C3 hardware repo. OTA is the
 * practical workaround once the device is provisioned. */
