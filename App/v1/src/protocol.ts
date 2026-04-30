// Wire types — mirrors docs/control-protocol-spec.md.
// Imported verbatim from Firmware/v1/webapp/src/protocol.ts (deliberately
// kept framework-free so it ports between webapp and RN).

export type AuthMode = 'open' | 'paired';

export type PatternId =
  | 'solid'
  | 'rainbow'
  | 'scanner'
  | 'spinner'
  | 'random'
  | 'breathing'
  | 'twinkle';

export interface DeviceState {
  on: boolean;
  pattern_id: PatternId | string;
  base_color: string;
  brightness: number;
  led_count: number;
  auth_mode: AuthMode;
  telemetry_enabled: boolean;
  // Added in Session 5 sub-4. Older firmware will omit; clients must
  // tolerate `undefined` per control-protocol-spec §9 (forward compat).
  provisioning_active?: boolean;
  wifi_ssid?: string | null;
}

export interface SetWifiCredsPayload {
  ssid: string;
  password: string;
}

export interface RequestEnvelope {
  op: string;
  req_id: string;
  ts: number;
  payload?: unknown;
  hmac?: string;
}

export interface ResponseEnvelope {
  op: string;
  req_id: string;
  ok: boolean;
  result: unknown;
  error: { code: string; message: string } | null;
}

export interface StateBroadcast {
  op: 'state';
  ts: number;
  state: DeviceState;
}

export type InboundFrame = ResponseEnvelope | StateBroadcast;

export function isStateBroadcast(frame: InboundFrame): frame is StateBroadcast {
  return frame.op === 'state' && 'state' in frame;
}

export function newReqId(): string {
  // RN's polyfill exposes crypto.randomUUID on Hermes 0.74+; fall back if absent.
  if (typeof crypto !== 'undefined' && 'randomUUID' in crypto) {
    return (crypto as { randomUUID: () => string }).randomUUID();
  }
  return `r-${Date.now().toString(36)}-${Math.random().toString(36).slice(2, 10)}`;
}

export function nowEpochSeconds(): number {
  return Math.floor(Date.now() / 1000);
}
