import { useEffect, useMemo, useState } from 'preact/hooks';
import { ConnState, MirrorClient, defaultDeviceUrl } from './ws-client';
import { DeviceState } from './protocol';
import { Header } from './components/Header';
import { useHashRoute } from './hooks';
import { ControlPage } from './pages/control';
import { SettingsPage } from './pages/settings';
import { SetupPage } from './pages/setup';

// Paired-mode shared secret (LL-078). Persisted in localStorage so a
// paired mirror doesn't re-prompt on every page load — localStorage works
// over plain http, unlike crypto.subtle which needs a secure context.
// Per-origin, i.e. per device host. Wrapped in try/catch because
// localStorage access throws when the browser has storage disabled.
const SECRET_KEY = 'll_mirror_secret';
function loadSecret(): string | null {
  try {
    return localStorage.getItem(SECRET_KEY);
  } catch {
    return null;
  }
}
function saveSecret(s: string | null): void {
  try {
    if (s) localStorage.setItem(SECRET_KEY, s);
    else localStorage.removeItem(SECRET_KEY);
  } catch {
    /* storage disabled — fall back to in-memory only for this session */
  }
}

export function App() {
  const [conn, setConn] = useState<ConnState>('idle');
  const [state, setState] = useState<DeviceState | null>(null);
  const [err, setErr] = useState<string | null>(null);
  const [route, navigate] = useHashRoute();
  // True after the user has submitted creds via /setup. Used to suppress
  // the cycling status badge in the header while the device intentionally
  // tears down its SoftAP — a "closed" / "connecting" cycle is expected
  // there, not a failure to surface.
  const [provSubmitted, setProvSubmitted] = useState(false);

  // Paired-mode auth (LL-078). `pairSecret` is the secret this browser
  // holds (seeded from localStorage); `needsSecret` is set when a connect
  // is rejected for a missing/wrong secret and drives the unlock screen.
  const [pairSecret, setPairSecret] = useState<string | null>(loadSecret);
  const [needsSecret, setNeedsSecret] = useState(false);
  const [unlockInput, setUnlockInput] = useState('');
  const [unlockBusy, setUnlockBusy] = useState(false);

  const client = useMemo(() => {
    const c = new MirrorClient({
      url: defaultDeviceUrl(),
      onConn: setConn,
      onState: setState,
      onError: setErr,
    });
    // Apply the persisted secret before connect, so the first frame to a
    // paired mirror is already signed.
    const saved = loadSecret();
    if (saved) c.setSecret(saved);
    return c;
  }, []);

  // Update the secret in one place: component state, the client (so the
  // next frame is signed), and localStorage (so it survives a reload).
  // Called by the pairing handlers and the unlock screen.
  const applySecret = (secret: string | null) => {
    setPairSecret(secret);
    client.setSecret(secret);
    saveSecret(secret);
  };

  // Unlock a paired mirror this browser has no (or a wrong) secret for.
  // There is no server "unlock" op — adopt the entered secret and prove
  // it by retrying get_state. A bad_hmac means a wrong passphrase.
  const submitUnlock = () => {
    const entered = unlockInput.trim();
    if (!entered) return;
    setErr(null);
    setUnlockBusy(true);
    client.setSecret(entered);
    client.getState().then(
      (s) => {
        setState(s);
        applySecret(entered);
        setNeedsSecret(false);
        setUnlockInput('');
        setUnlockBusy(false);
      },
      (e: Error) => {
        client.setSecret(pairSecret); // revert — entered passphrase didn't work
        setErr(
          e.message.includes('bad_hmac')
            ? 'That passphrase didn’t match. Try again.'
            : e.message,
        );
        setUnlockBusy(false);
      },
    );
  };

  useEffect(() => {
    client.connect();
    return () => client.disconnect();
  }, [client]);

  // On open: pull a snapshot so the UI reflects current device state
  // immediately. A paired mirror rejects an unsigned (or wrong-secret)
  // get_state — surface that as the unlock screen, not a raw error.
  useEffect(() => {
    if (conn !== 'open') return;
    let cancelled = false;
    client.getState().then(
      (s) => { if (!cancelled) { setState(s); setNeedsSecret(false); } },
      (e: Error) => {
        if (cancelled) return;
        if (e.message.includes('auth_required') || e.message.includes('bad_hmac')) {
          setNeedsSecret(true);
        } else {
          setErr(e.message);
        }
      },
    );
    return () => { cancelled = true; };
  }, [conn, client]);

  // Auto-route to /setup when the device says it's in provisioning mode,
  // unless the user has explicitly navigated somewhere else. The "first
  // observed value" guard prevents bouncing the user back to /setup if
  // they manually navigate to /settings while still on the SoftAP.
  const [autoRouted, setAutoRouted] = useState(false);
  useEffect(() => {
    if (autoRouted) return;
    if (state?.provisioning_active && route === '/') {
      navigate('/setup');
      setAutoRouted(true);
    } else if (state && state.provisioning_active === false) {
      // Device is on real wifi. If the user is still on /setup from a
      // prior auto-route, send them home. Otherwise leave the route alone.
      if (route === '/setup') navigate('/');
      setAutoRouted(true);
    }
  }, [state?.provisioning_active, route, autoRouted]);

  const canSend = conn === 'open';

  const headerOverride = provSubmitted
    ? ({ label: 'switching networks…', tone: 'warn' } as const)
    : undefined;

  return (
    <main class="shell">
      <Header
        conn={conn}
        route={route}
        onNavigate={navigate}
        connLabelOverride={headerOverride}
      />

      {conn === 'open' && needsSecret ? (
        <section class="panel">
          <h2 class="panel-title">mirror is locked</h2>
          <p class="hint muted" style={{ margin: '0 0 10px 0' }}>
            This mirror needs a passphrase. Enter it to control the mirror.
            If it's lost, hold the recessed button for 10 seconds to
            factory-reset the mirror back to open mode.
          </p>
          <input
            type="password"
            value={unlockInput}
            placeholder="passphrase"
            onInput={(e) => setUnlockInput((e.currentTarget as HTMLInputElement).value)}
            style={{ width: '100%', boxSizing: 'border-box', padding: '8px 10px', marginBottom: '8px' }}
          />
          <button
            type="button"
            class="btn-secondary"
            onClick={submitUnlock}
            disabled={unlockBusy || unlockInput.trim().length === 0}
          >
            {unlockBusy ? 'checking…' : 'unlock'}
          </button>
          {err && <p class="hint err" style={{ marginTop: '10px' }}>{err}</p>}
        </section>
      ) : (
        <>
          {route === '/' && (
            <ControlPage
              client={client}
              state={state}
              canSend={canSend}
              err={err}
              setErr={setErr}
            />
          )}
          {route === '/settings' && (
            <SettingsPage
              client={client}
              state={state}
              canSend={canSend}
              err={err}
              setErr={setErr}
              onNavigate={navigate}
              pairSecret={pairSecret}
              applySecret={applySecret}
            />
          )}
          {route === '/setup' && (
            <SetupPage
              client={client}
              state={state}
              canSend={canSend}
              err={err}
              setErr={setErr}
              onNavigate={navigate}
              onSubmittedChange={setProvSubmitted}
            />
          )}
        </>
      )}
    </main>
  );
}
