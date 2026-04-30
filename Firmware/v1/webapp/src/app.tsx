import { useEffect, useMemo, useState } from 'preact/hooks';
import { ConnState, MirrorClient, defaultDeviceUrl } from './ws-client';
import { DeviceState } from './protocol';
import { Header } from './components/Header';
import { useHashRoute } from './hooks';
import { ControlPage } from './pages/control';
import { SettingsPage } from './pages/settings';
import { SetupPage } from './pages/setup';

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

  const client = useMemo(
    () =>
      new MirrorClient({
        url: defaultDeviceUrl(),
        onConn: setConn,
        onState: setState,
        onError: setErr,
      }),
    [],
  );

  useEffect(() => {
    client.connect();
    return () => client.disconnect();
  }, [client]);

  // On open: pull a snapshot so the UI reflects current device state immediately.
  useEffect(() => {
    if (conn !== 'open') return;
    let cancelled = false;
    client.getState().then(
      (s) => { if (!cancelled) setState(s); },
      (e: Error) => { if (!cancelled) setErr(e.message); },
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
    </main>
  );
}
