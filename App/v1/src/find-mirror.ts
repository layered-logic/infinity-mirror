// Subnet-scan-based mirror discovery. Avoids needing react-native-zeroconf
// (deep-dep, MAX_PATH risk on Windows builds) and works on any Android API
// that supports WifiManager.connectionInfo.

import { NativeModules } from 'react-native';
import { DeviceInfo } from './protocol';

interface WifiInfoNative {
  getIpAddress(): Promise<string>;
}

const WifiInfo: WifiInfoNative = NativeModules.WifiInfo;

const PROBE_TIMEOUT_MS = 1500;
const PROBE_PATH = '/api/info';

export interface FindResult {
  ip: string;
  url: string;          // ws://<ip>/ws — ready for MirrorClient
  id: string;           // lowercase 6-hex MAC suffix
  name: string;         // user-set; empty string if unnamed
}

// Probe one IP. Resolves to a FindResult if it's our mirror, null otherwise.
// Returning the parsed body in one call avoids a second roundtrip per match
// — the picker needs id+name immediately to label the row.
async function probe(ip: string, signal: AbortSignal): Promise<FindResult | null> {
  try {
    const r = await fetch(`http://${ip}${PROBE_PATH}`, { signal });
    if (!r.ok) return null;
    const body = (await r.json()) as Partial<DeviceInfo>;
    if (body?.product !== 'layered-logic-mirror' || typeof body.id !== 'string') {
      return null;
    }
    return {
      ip,
      url: `ws://${ip}/ws`,
      id: body.id,
      name: typeof body.name === 'string' ? body.name : '',
    };
  } catch {
    return null;
  }
}

// Scans the /24 of `selfIp` and returns ALL mirrors that respond within
// PROBE_TIMEOUT_MS. Resolves with [] if none found — the caller (App.tsx)
// decides how to surface "nothing here". Order is whatever order the
// individual probes resolve in, which on a typical home LAN is roughly
// IP order modulo per-host latency variance.
export async function findMirrors(): Promise<FindResult[]> {
  const selfIp = await WifiInfo.getIpAddress();
  const parts = selfIp.split('.');
  if (parts.length !== 4) throw new Error(`unexpected IP: ${selfIp}`);
  const subnet = `${parts[0]}.${parts[1]}.${parts[2]}`;
  const selfLast = Number(parts[3]);

  const ctrl = new AbortController();
  const deadline = setTimeout(() => ctrl.abort(), PROBE_TIMEOUT_MS);

  const probes: Promise<FindResult | null>[] = [];
  for (let i = 1; i <= 254; i++) {
    if (i === selfLast) continue;
    probes.push(probe(`${subnet}.${i}`, ctrl.signal));
  }

  const results = await Promise.all(probes);
  clearTimeout(deadline);

  const found = results.filter((r): r is FindResult => r !== null);
  // Dedupe by id in case of NAT or multi-IP devices (defensive — we don't
  // expect duplicates in practice, but a single mirror responding twice
  // would otherwise show up as two picker rows).
  const seen = new Set<string>();
  return found.filter((r) => {
    if (seen.has(r.id)) return false;
    seen.add(r.id);
    return true;
  });
}
