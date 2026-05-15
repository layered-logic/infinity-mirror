import {
  DeviceState,
  InboundFrame,
  OtaProgressBroadcast,
  ResponseEnvelope,
  SetWifiCredsPayload,
  StateBroadcast,
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
  // Auto-reconnect with exponential backoff (1s → 2s → 4s → 8s capped).
  // Default true. Set false for tests or one-shot scripts.
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
    ws.addEventListener('message', (ev) => this.handleMessage(ev.data));
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

  // Send a request envelope and resolve when the matching response arrives.
  // Note: state.set responses are best-effort/stale per Session 2b — UI should
  // react to the state broadcast that follows, not this resolved value.
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

  // Submit Wi-Fi credentials. Device responds ok=true synchronously, then
  // tears down the SoftAP and joins the new network async — this client's
  // socket will drop within ~100ms and the reconnect-with-backoff has to
  // wait for the device to come up on the new netif (or for the user to
  // switch their laptop/phone to the same network the device just joined).
  setWifiCreds(payload: SetWifiCredsPayload): Promise<void> {
    return this.send<SetWifiCredsPayload, { applied: boolean }>('set_wifi_creds', payload).then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'set_wifi_creds failed');
    });
  }

  // Reset all settings + wipe Wi-Fi creds. Device falls back into the
  // no-creds boot path (SoftAP), so any STA-side client loses its socket.
  factoryReset(): Promise<void> {
    return this.send<undefined, { reset: boolean }>('factory_reset').then((r) => {
      if (!r.ok) throw new Error(r.error?.message ?? r.error?.code ?? 'factory_reset failed');
    });
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

export function defaultDeviceUrl(): string {
  // Dev: hardcoded SoftAP. Prod (served from device): same-origin.
  // import.meta.env.DEV is set by Vite's `vite` (dev) command; false in `vite build`.
  if (import.meta.env.DEV) {
    return 'ws://192.168.4.1/ws';
  }
  return `ws://${location.host}/ws`;
}
