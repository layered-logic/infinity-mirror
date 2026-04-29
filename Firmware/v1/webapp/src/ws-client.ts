import {
  DeviceState,
  InboundFrame,
  ResponseEnvelope,
  StateBroadcast,
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
}

interface PendingRequest {
  resolve: (resp: ResponseEnvelope) => void;
  reject: (err: Error) => void;
  timeout: ReturnType<typeof setTimeout>;
}

const REQ_TIMEOUT_MS = 5000;

export class MirrorClient {
  private ws: WebSocket | null = null;
  private pending = new Map<string, PendingRequest>();
  private connState: ConnState = 'idle';

  constructor(private opts: MirrorClientOptions) {}

  get state(): ConnState {
    return this.connState;
  }

  connect(): void {
    if (this.ws && (this.ws.readyState === WebSocket.OPEN || this.ws.readyState === WebSocket.CONNECTING)) {
      return;
    }
    this.setConn('connecting');
    const ws = new WebSocket(this.opts.url);
    this.ws = ws;

    ws.addEventListener('open', () => this.setConn('open'));
    ws.addEventListener('close', () => {
      this.setConn('closed');
      this.failAllPending(new Error('socket closed'));
    });
    ws.addEventListener('error', () => {
      this.opts.onError?.('socket error');
    });
    ws.addEventListener('message', (ev) => this.handleMessage(ev.data));
  }

  disconnect(): void {
    this.ws?.close();
    this.ws = null;
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
