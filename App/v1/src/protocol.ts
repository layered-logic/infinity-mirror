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
  ip?: string | null;
  // Identity. `id` is the lowercase 6-hex MAC suffix — read-only, stable
  // across factory resets, used as a fallback display label when `name` is
  // empty. `name` is user-set via set_state and persists in NVS.
  id?: string;
  name?: string;
  // Multi-network store size — how many entries are saved in ll_wifi.
  // Added with LL-046 step 4. The full list lives behind list_wifi_networks
  // to keep state broadcasts small; this count is enough for the badge on
  // the Settings page and for clients to know whether a refetch is worth
  // doing after a notification.
  wifi_saved_count?: number;
}

// Multi-network store entry — what list_wifi_networks returns. Passwords
// never leave the device, so this is read-only. last_used is monotonic
// device uptime in seconds, used for ordering only (not a wall clock).
export interface WifiNetwork {
  ssid: string;
  is_active: boolean;
  last_used: number;
}

export interface AddWifiNetworkPayload {
  ssid: string;
  password: string;
}

export interface AddWifiNetworkResult {
  ssid: string;
  inserted: boolean;
  updated: boolean;
}

export interface RemoveWifiNetworkPayload {
  ssid: string;
}

export interface RemoveWifiNetworkResult {
  ssid: string;
  removed: boolean;
  was_active: boolean;
}

export interface ConnectWifiNetworkResult {
  ssid: string;
  switching: boolean;
}

// GET /api/info response. Lightweight discovery payload — same id+name
// used in DeviceState, but reachable in one HTTP roundtrip without
// opening a WebSocket. Used by the RN app's findMirrors subnet scan.
export interface DeviceInfo {
  product: 'layered-logic-mirror';
  id: string;
  name: string;
  fw_version: string;
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

// Canonical preset colors — single source of truth for both webapp and RN
// so a swatch labeled "red" lands on the same hex no matter which client
// the user picks it from.
//
// Three brand indigos (button-interface §4.1 sets Indigo Signal as the
// firmware default) anchor the row, followed by eight hues sampled at 45°
// from the button's 16-step HSV cycle (button-interface §4.3) so the app
// and the physical single-press cycle pick from the same vocabulary.
export interface BrandSwatch { name: string; hex: string; }
export const BRAND_SWATCHES: BrandSwatch[] = [
  { name: 'indigo reference', hex: '#4A25FF' },
  { name: 'indigo signal',    hex: '#3214FF' },
  { name: 'indigo stone',     hex: '#4F3FB0' },
  { name: 'red',     hex: '#FF0000' },
  { name: 'amber',   hex: '#FFBF00' },
  { name: 'lime',    hex: '#80FF00' },
  { name: 'mint',    hex: '#00FF40' },
  { name: 'cyan',    hex: '#00FFFF' },
  { name: 'blue',    hex: '#0040FF' },
  { name: 'violet',  hex: '#8000FF' },
  { name: 'magenta', hex: '#FF00BF' },
  { name: 'white',   hex: '#FFFFFF' },
];
