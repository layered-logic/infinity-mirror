// Ported from Firmware/v1/webapp/src/ws-client.ts. Logic is identical;
// the only swap is that the RN build has no Vite env so callers must pass
// the URL explicitly (no defaultDeviceUrl helper).

import {
  AddWifiNetworkPayload,
  AddWifiNetworkResult,
  ConnectWifiNetworkResult,
  DeviceState,
  InboundFrame,
  OtaProgressBroadcast,
  RemoveWifiNetworkResult,
  ResponseEnvelope,
  SetWifiCredsPayload,
  StateBroadcast,
  WifiNetwork,
  isOtaProgress,
  isStateBroadcast,
  newReqId,
  nowEpochSeconds,
} from './protocol';

export type ConnState = 'idle' | 'connecting' | 'open' | 'closed';

export interface MirrorClientOptions {
  url: string;
  onState?: (state: DeviceState) => void;
  onConn?: (s: ConnState) => void;
  onError?: (msg: string) => void;
  // Streaming OTA progress emitted by the device during esp_https_ota's
  // perform loop (LL-057 Session B). One-way; clients render percent+phase.
  onOtaProgress?: (p: OtaProgressBroadcast) => void;
  autoReconnect?: boolean;
}

interface PendingRequest {
  resolve: (resp: ResponseEnvelope) => void;
  reject: (err: Error) => void;
  timeout: ReturnType<typeof setTimeout>;
}

const REQ_TIMEOUT_MS = 5000;
const RECONNECT_BASE_MS = 1000;
const RECONNECT_MAX_MS = 8000;

export class MirrorClient {
  private ws: WebSocket | null = null;
  private pending = new Map<string, PendingRequest>();
  private connState: ConnState = 'idle';
  private wantOpen = false;
  private reconnectAttempt = 0;
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null;

  constructor(private opts: MirrorClientOptions) {}

  get state(): ConnState {
    return this.connState;
  }

  connect(): void {
    this.wantOpen = true;
    this.openSocket();
  }

  disconnect(): void {
    this.wantOpen = false;
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer);
      this.reconnectTimer = null;
    }
    this.ws?.close();
    this.ws = null;
  }

  private openSocket(): void {
    if (this.ws && (this.ws.readyState === WebSocket.OPEN || this.ws.readyState === WebSocket.CONNECTING)) {
      return;
    }
    this.setConn('connecting');
    const ws = new WebSocket(this.opts.url);
    this.ws = ws;

    ws.addEventListener('open', () => {
      this.reconnectAttempt = 0;
      this.setConn('open');
    });
    ws.addEventListener('close', () => {
      this.setConn('closed');
      this.failAllPending(new Error('socket closed'));
      this.scheduleReconnect();
    });
    ws.addEventListener('error', () => {
      this.opts.onError?.('socket error');
    });
    ws.addEventListener('message', (ev: MessageEvent) => this.handleMessage(ev.data));
  }

  private scheduleReconnect(): void {
    if (!this.wantOpen) return;
    if (this.opts.autoReconnect === false) return;
    if (this.reconnectTimer) return;
    const delay = Math.min(
      RECONNECT_BASE_MS * Math.pow(2, this.reconnectAttempt),
      RECONNECT_MAX_MS,
    );
    this.reconnectAttempt += 1;
    this.reconnectTimer = setTimeout(() => {
      this.reconnectTimer = null;
      if (this.wantOpen) this.openSocket();
    }, delay);
  }

  send<TPayload, TResult = unknown>(op: string, payload?: TPayload): Promise<ResponseEnvelope & { result: TResult }> {
    return new Promise((resolve, reject) => {
      if (!this.ws || this.ws.readyState !== WebSocket.OPEN) {
        reject(new Error('not connected'));
        return;
      }
      const req_id = newReqId();
      const envelope = {
        op,
        req_id,
        ts: nowEpochSeconds(),
        ...(payload === undefined ? {} : { payload }),
      };
      const timeout = setTimeout(() => {
        this.pending.delete(req_id);
        reject(new Error(`timeout: ${op}`));
      }, REQ_TIMEOUT_MS);
      this.pending.set(req_id, {
        resolve: resolve as (r: ResponseEnvelope) => void,
        reject,
        timeout,
      });
      this.ws.send(JSON.stringify(envelope));
    });
  }

  getState(): Promise<DeviceState> {
    return this.send<undefined, DeviceState>('get_state').then((r) => {
      if (!r.ok) throw new Error(`get_state: ${r.error?.code ?? 'unknown'}`);
      return r.result;
    });
  }

  setState(patch: Partial<DeviceState>): Promise<void> {
    return this.send('set_state', patch).then(() => undefined);
  }

  ping(): Promise<{ fw_version: string; uptime_s: number }> {
    return this.send<undefined, { fw_version: string; uptime_s: number }>('ping').then((r) => r.result);
  }

  setWifiCreds(payload: SetWifiCredsPayload): Promise<void> {
    return this.send<SetWifiCredsPayload, { applied: boolean }>('set_wifi_creds', payload).then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'set_wifi_creds failed');
    });
  }

  // Multi-network store API — LL-046 step 4 / multi-network-design.md §7.
  // Provisioning uses set_wifi_creds for the legacy first-cred path
  // (still works for the SoftAP setup flow); add/remove are for managing
  // additional networks once the device is already online.

  listWifiNetworks(): Promise<WifiNetwork[]> {
    return this.send<undefined, { networks: WifiNetwork[] }>('list_wifi_networks').then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'list_wifi_networks failed');
      return r.result.networks;
    });
  }

  // Insert-or-update by ssid. Does NOT switch the active connection —
  // adding a network just adds it to the saved list; the SM picks based
  // on visibility next time it scans.
  addWifiNetwork(payload: AddWifiNetworkPayload): Promise<AddWifiNetworkResult> {
    return this.send<AddWifiNetworkPayload, AddWifiNetworkResult>('add_wifi_network', payload).then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'add_wifi_network failed');
      return r.result;
    });
  }

  // Forget a saved network. If `ssid` is the currently-connected one, the
  // mirror disconnects immediately (per design-doc §4.3) — caller is
  // responsible for surfacing the confirm dialog before invoking this.
  removeWifiNetwork(ssid: string): Promise<RemoveWifiNetworkResult> {
    return this.send<{ ssid: string }, RemoveWifiNetworkResult>('remove_wifi_network', { ssid }).then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'remove_wifi_network failed');
      return r.result;
    });
  }

  // Switch the active connection to a user-chosen saved network. Bumps
  // the entry's priority and forces a fresh scan-and-pick on the device.
  // Resolves synchronously with `switching: true` — the actual connect
  // happens async; observe the result via the next state broadcast.
  // The socket may briefly close mid-switch (the device's STA tear-down
  // kicks pending HTTP server connections); the auto-reconnect loop
  // brings it back once the new network has an IP.
  connectWifiNetwork(ssid: string): Promise<ConnectWifiNetworkResult> {
    return this.send<{ ssid: string }, ConnectWifiNetworkResult>('connect_wifi_network', { ssid }).then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'connect_wifi_network failed');
      return r.result;
    });
  }

  factoryReset(): Promise<void> {
    return this.send<undefined, { reset: boolean }>('factory_reset').then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'factory_reset failed');
    });
  }

  // Start an OTA pull. Device fetches the binary at `url`, validates it,
  // swaps the active partition, and reboots. On success the socket dies
  // before any response arrives — auto-reconnect picks up the new
  // firmware. On failure the device stays put and returns ota_failed.
  startOta(url: string): Promise<void> {
    return this.send<{ url: string }, unknown>('start_ota', { url }).then(
      (r) => {
        if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'start_ota failed');
      },
      // socket-died-before-response is the success path here, so swallow
      // a generic timeout/closed and let the caller treat that as "OK".
      (e: Error) => {
        const m = e.message;
        if (m.includes('socket closed') || m.includes('timeout')) return;
        throw e;
      },
    );
  }

  private handleMessage(data: unknown): void {
    if (typeof data !== 'string') return;
    let frame: InboundFrame;
    try {
      frame = JSON.parse(data);
    } catch {
      this.opts.onError?.('non-JSON frame');
      return;
    }
    if (isStateBroadcast(frame)) {
      this.opts.onState?.((frame as StateBroadcast).state);
      return;
    }
    if (isOtaProgress(frame)) {
      this.opts.onOtaProgress?.(frame);
      return;
    }
    const resp = frame as ResponseEnvelope;
    const p = this.pending.get(resp.req_id);
    if (!p) return;
    this.pending.delete(resp.req_id);
    clearTimeout(p.timeout);
    p.resolve(resp);
  }

  private failAllPending(err: Error): void {
    for (const [, p] of this.pending) {
      clearTimeout(p.timeout);
      p.reject(err);
    }
    this.pending.clear();
  }

  private setConn(s: ConnState): void {
    this.connState = s;
    this.opts.onConn?.(s);
  }
}
