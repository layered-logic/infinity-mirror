// Wire types — mirrors docs/control-protocol-spec.md.
// This module is Preact-free so the RN app can import it verbatim in Session 7.

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
  // RFC 4122 not required by the protocol — just needs to be unique per client.
  // crypto.randomUUID is available in all evergreen browsers + RN's polyfill.
  if (typeof crypto !== 'undefined' && 'randomUUID' in crypto) {
    return crypto.randomUUID();
  }
  return `r-${Date.now().toString(36)}-${Math.random().toString(36).slice(2, 10)}`;
}

export function nowEpochSeconds(): number {
  return Math.floor(Date.now() / 1000);
}
