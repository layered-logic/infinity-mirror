// mDNS-based mirror discovery via Android's NsdManager (see MdnsModule.kt).
// Replaces a prior subnet-scan implementation that fired 254 parallel fetches
// and lost the mirror's response in OkHttp's dispatcher queue. mDNS is also
// subnet-tolerant: if the phone and mirror end up on different /24s of the
// same routed network, scan never finds it but mDNS still does.

import { NativeModules } from 'react-native';
import { DeviceInfo } from './protocol';

interface MdnsResult {
  host: string;
  port: number;
  attributes: Record<string, string>;
}

interface MdnsNative {
  findMirrors(timeoutMs: number): Promise<MdnsResult[]>;
}

const Mdns: MdnsNative = NativeModules.Mdns;

const DISCOVERY_TIMEOUT_MS = 2500;
const PROBE_TIMEOUT_MS = 1500;
const PROBE_PATH = '/api/info';

export interface FindResult {
  ip: string;
  url: string;          // ws://<ip>[:<port>]/ws — ready for MirrorClient
  id: string;           // lowercase 6-hex MAC suffix
  name: string;         // user-set; empty string if unnamed
}

// Hit /api/info to get the human-readable name. mDNS TXT records carry
// {variant, version, id, auth} but not `name` — the user-rename feature
// (firmware welcome sequence, Apr 2026) lives in NVS-backed state, not
// the mDNS record. One HTTP roundtrip per mirror is cheap; typically 1-2
// mirrors on a network, never the 254 the old subnet scan sent.
async function fetchInfo(ip: string, port: number): Promise<FindResult | null> {
  const ctrl = new AbortController();
  const t = setTimeout(() => ctrl.abort(), PROBE_TIMEOUT_MS);
  try {
    const portSuffix = port === 80 ? '' : `:${port}`;
    const r = await fetch(`http://${ip}${portSuffix}${PROBE_PATH}`, {
      signal: ctrl.signal,
    });
    if (!r.ok) return null;
    const body = (await r.json()) as Partial<DeviceInfo>;
    if (body?.product !== 'layered-logic-mirror' || typeof body.id !== 'string') {
      return null;
    }
    return {
      ip,
      url: `ws://${ip}${portSuffix}/ws`,
      id: body.id,
      name: typeof body.name === 'string' ? body.name : '',
    };
  } catch {
    return null;
  } finally {
    clearTimeout(t);
  }
}

// Discover all _layeredlogic._tcp. mirrors on the local network and resolve
// each to a FindResult. Returns [] when nothing's found within the
// discovery window — caller decides how to surface "nothing here".
export async function findMirrors(): Promise<FindResult[]> {
  const services = await Mdns.findMirrors(DISCOVERY_TIMEOUT_MS);
  if (services.length === 0) return [];

  const probes = services.map((s) => fetchInfo(s.host, s.port));
  const results = await Promise.all(probes);
  const found = results.filter((r): r is FindResult => r !== null);

  // Dedupe by id — multiple mDNS announcements (e.g. on multi-homed networks)
  // can surface the same device twice. Keep the first response per id.
  const seen = new Set<string>();
  return found.filter((r) => {
    if (seen.has(r.id)) return false;
    seen.add(r.id);
    return true;
  });
}
