// Cloudflare Worker for telemetry.layeredlogic.cc/v1/beacon.
//
// Accepts JSON POSTs from Layered Logic mirrors (firmware-spec §4.10) on
// a jittered 24h cadence when the user has opted in. Stores each beacon
// in KV with a 90-day TTL plus a no-TTL "latest" snapshot per device.
//
// Beacon schema (LL-057-C MVP — versioned via `v` field for forward
// compatibility when fields evolve):
//
//   {
//     "v": 1,
//     "device_id": "b2332c",          // lowercase 3-byte MAC suffix, hex
//     "fw_version": "651afe8-dirty",  // git describe output
//     "uptime_s": 12345,
//     "heap_free_min": 89000,         // esp_get_minimum_free_heap_size
//     "boot_reason": "POWERON_RESET", // esp_reset_reason() as string
//     "rssi": -52,                    // current STA RSSI, dBm
//     "led_count": 66,
//     "panic": {                      // optional — present if panic happened since last beacon
//       "filename": "main.c",
//       "line": 42,
//       "task_name": "state_bus"
//     }
//   }
//
// Endpoints:
//   POST /v1/beacon        — accept + store a beacon
//   GET  /v1/healthz       — liveness probe (returns 200 always)
//   GET  /v1/latest/:id    — return the latest beacon for a device (admin-ish; KV-public scope is fine for this hardware id, but consider auth before exposing externally)
//
// Anti-abuse: 16 KiB body cap, JSON-shape validation, device_id format
// guard. No auth on POST — Bill's call once paired-mode HMAC lands
// firmware-side (LL-057-D); for V1 the worst an attacker can do with a
// known device_id is push false metrics into KV.

export interface Env {
  LL_TELEMETRY_KV: KVNamespace;
}

interface BeaconPayload {
  v: number;
  device_id: string;
  fw_version: string;
  uptime_s: number;
  heap_free_min: number;
  boot_reason: string;
  rssi: number;
  led_count: number;
  panic?: {
    filename: string;
    line: number;
    task_name: string;
  };
}

const MAX_BODY_BYTES = 16 * 1024;
const DEVICE_ID_RE = /^[0-9a-f]{6}$/;
const BEACON_TTL_S = 90 * 24 * 60 * 60; // 90 days

function badRequest(message: string): Response {
  return new Response(JSON.stringify({ ok: false, error: message }), {
    status: 400,
    headers: { 'content-type': 'application/json' },
  });
}

function ok(body: object, status = 200): Response {
  return new Response(JSON.stringify(body), {
    status,
    headers: { 'content-type': 'application/json' },
  });
}

// Validate the beacon shape. Returns the validated payload on success or
// a string error message on failure. Strict enough to reject garbage but
// tolerant of forward-compat additions (extra fields ignored).
function validateBeacon(b: unknown): BeaconPayload | string {
  if (typeof b !== 'object' || b === null) return 'body must be a JSON object';
  const o = b as Record<string, unknown>;
  if (o.v !== 1) return `unsupported beacon version: ${String(o.v)}`;
  if (typeof o.device_id !== 'string' || !DEVICE_ID_RE.test(o.device_id))
    return 'device_id must be 6 lowercase hex chars';
  if (typeof o.fw_version !== 'string' || o.fw_version.length === 0 || o.fw_version.length > 64)
    return 'fw_version must be a non-empty string ≤64 chars';
  if (typeof o.uptime_s !== 'number' || !Number.isFinite(o.uptime_s) || o.uptime_s < 0)
    return 'uptime_s must be a non-negative finite number';
  if (typeof o.heap_free_min !== 'number' || !Number.isFinite(o.heap_free_min) || o.heap_free_min < 0)
    return 'heap_free_min must be a non-negative finite number';
  if (typeof o.boot_reason !== 'string' || o.boot_reason.length === 0 || o.boot_reason.length > 32)
    return 'boot_reason must be a non-empty string ≤32 chars';
  if (typeof o.rssi !== 'number' || !Number.isFinite(o.rssi) || o.rssi > 0 || o.rssi < -120)
    return 'rssi must be a finite number in [-120, 0]';
  if (typeof o.led_count !== 'number' || !Number.isInteger(o.led_count) || o.led_count < 1 || o.led_count > 1024)
    return 'led_count must be an integer in [1, 1024]';
  if (o.panic !== undefined) {
    if (typeof o.panic !== 'object' || o.panic === null) return 'panic must be an object if present';
    const p = o.panic as Record<string, unknown>;
    if (typeof p.filename !== 'string' || p.filename.length > 64) return 'panic.filename must be a string ≤64 chars';
    if (typeof p.line !== 'number' || !Number.isInteger(p.line) || p.line < 0) return 'panic.line must be a non-negative integer';
    if (typeof p.task_name !== 'string' || p.task_name.length > 32) return 'panic.task_name must be a string ≤32 chars';
  }
  return b as BeaconPayload;
}

async function handleBeacon(req: Request, env: Env): Promise<Response> {
  const lenHeader = req.headers.get('content-length');
  if (lenHeader !== null) {
    const len = Number(lenHeader);
    if (Number.isFinite(len) && len > MAX_BODY_BYTES) return badRequest('body too large');
  }

  let raw: string;
  try {
    raw = await req.text();
  } catch {
    return badRequest('could not read body');
  }
  if (raw.length > MAX_BODY_BYTES) return badRequest('body too large');

  let parsed: unknown;
  try {
    parsed = JSON.parse(raw);
  } catch {
    return badRequest('body must be valid JSON');
  }

  const v = validateBeacon(parsed);
  if (typeof v === 'string') return badRequest(v);

  const nowMs = Date.now();
  const beaconKey = `beacon:${v.device_id}:${nowMs}`;
  const latestKey = `latest:${v.device_id}`;
  const stored = JSON.stringify({ ...v, received_at_ms: nowMs });

  // Two writes: per-timestamp entry with TTL, plus a no-TTL "latest"
  // pointer. KV writes are eventually-consistent and roughly free at
  // this volume (one mirror, one POST per day → 365 writes/year/device).
  await Promise.all([
    env.LL_TELEMETRY_KV.put(beaconKey, stored, { expirationTtl: BEACON_TTL_S }),
    env.LL_TELEMETRY_KV.put(latestKey, stored),
  ]);

  return ok({ ok: true, stored_at: beaconKey });
}

async function handleLatest(env: Env, deviceId: string): Promise<Response> {
  if (!DEVICE_ID_RE.test(deviceId)) return badRequest('invalid device_id');
  const val = await env.LL_TELEMETRY_KV.get(`latest:${deviceId}`);
  if (val === null) return new Response('not found', { status: 404 });
  return new Response(val, {
    status: 200,
    headers: { 'content-type': 'application/json' },
  });
}

export default {
  async fetch(req: Request, env: Env): Promise<Response> {
    const url = new URL(req.url);

    if (req.method === 'POST' && url.pathname === '/v1/beacon') {
      return handleBeacon(req, env);
    }
    if (req.method === 'GET' && url.pathname === '/v1/healthz') {
      return ok({ ok: true, service: 'll-mirror-telemetry' });
    }
    if (req.method === 'GET' && url.pathname.startsWith('/v1/latest/')) {
      const id = url.pathname.slice('/v1/latest/'.length);
      return handleLatest(env, id);
    }

    return new Response('not found', { status: 404 });
  },
};
