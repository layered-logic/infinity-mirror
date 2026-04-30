// Subnet-scan-based mirror discovery. Avoids needing react-native-zeroconf
// (deep-dep, MAX_PATH risk on Windows builds) and works on any Android API
// that supports WifiManager.connectionInfo.

import { NativeModules } from 'react-native';

interface WifiInfoNative {
  getIpAddress(): Promise<string>;
}

const WifiInfo: WifiInfoNative = NativeModules.WifiInfo;

const PROBE_TIMEOUT_MS = 1200;
const TITLE_MARKER = '<title>Layered Logic Mirror</title>';

export interface FindResult {
  ip: string;
  url: string;
}

// Probes one IP. Resolves to the IP if it's our mirror, null otherwise.
async function probe(ip: string, signal: AbortSignal): Promise<string | null> {
  try {
    const r = await fetch(`http://${ip}/`, { signal });
    if (!r.ok) return null;
    const body = await r.text();
    if (body.includes(TITLE_MARKER)) return ip;
    return null;
  } catch {
    return null;
  }
}

// Scans the /24 of `selfIp` for a host serving the mirror's webapp.
// Resolves with the first match (race) or rejects if none found within
// the deadline.
export async function findMirror(): Promise<FindResult> {
  const selfIp = await WifiInfo.getIpAddress();
  const parts = selfIp.split('.');
  if (parts.length !== 4) throw new Error(`unexpected IP: ${selfIp}`);
  const subnet = `${parts[0]}.${parts[1]}.${parts[2]}`;
  const selfLast = Number(parts[3]);

  const ctrl = new AbortController();
  const deadline = setTimeout(() => ctrl.abort(), PROBE_TIMEOUT_MS);

  const probes: Promise<string | null>[] = [];
  for (let i = 1; i <= 254; i++) {
    if (i === selfLast) continue;
    probes.push(probe(`${subnet}.${i}`, ctrl.signal));
  }

  // Promise.any-like: resolve on the first non-null, reject if all are null.
  const found = await new Promise<string | null>((resolve) => {
    let pending = probes.length;
    for (const p of probes) {
      p.then((ip) => {
        if (ip) {
          ctrl.abort(); // cancel the rest
          resolve(ip);
        } else if (--pending === 0) {
          resolve(null);
        }
      });
    }
  });

  clearTimeout(deadline);
  if (!found) throw new Error('no mirror found on this network');
  return { ip: found, url: `ws://${found}/ws` };
}
