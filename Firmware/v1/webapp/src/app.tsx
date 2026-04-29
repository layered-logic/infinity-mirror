import { useEffect, useMemo, useState } from 'preact/hooks';
import { ConnState, MirrorClient, defaultDeviceUrl } from './ws-client';
import { DeviceState } from './protocol';

function statusClass(s: ConnState): string {
  if (s === 'open') return 'status status--ok';
  if (s === 'connecting') return 'status status--warn';
  if (s === 'closed') return 'status status--err';
  return 'status status--idle';
}

// Single-press base-color cycle from button-interface.md §4.3.
// Reused as a one-click preset row for the demo.
const PRESETS: string[] = [
  '#FF0000', '#FF8000', '#FFFF00', '#00FF00', '#00FFFF', '#0000FF', '#FF00FF',
];

function normalizeHex(value: string): string {
  const v = value.startsWith('#') ? value : `#${value}`;
  return v.toUpperCase();
}

export function App() {
  const [conn, setConn] = useState<ConnState>('idle');
  const [state, setState] = useState<DeviceState | null>(null);
  const [err, setErr] = useState<string | null>(null);

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

  // On open: pull a snapshot so the UI reflects current device state immediately
  // rather than waiting for the next broadcast.
  useEffect(() => {
    if (conn !== 'open') return;
    let cancelled = false;
    client.getState().then(
      (s) => { if (!cancelled) setState(s); },
      (e: Error) => { if (!cancelled) setErr(e.message); },
    );
    return () => { cancelled = true; };
  }, [conn, client]);

  const canSend = conn === 'open';

  const sendColor = (hex: string) => {
    setErr(null);
    client.setState({ base_color: normalizeHex(hex) }).catch((e: Error) => setErr(e.message));
  };

  return (
    <main class="shell">
      <header class="bar">
        <span class="title">Layered Logic Mirror</span>
        <span class={statusClass(conn)}>{conn}</span>
      </header>

      <section class="panel">
        <h2 class="panel-title">connection</h2>
        <dl class="kv">
          <dt>endpoint</dt><dd>{defaultDeviceUrl()}</dd>
          <dt>status</dt><dd>{conn}</dd>
          {err && (<><dt>last error</dt><dd class="err">{err}</dd></>)}
        </dl>
      </section>

      <section class="panel">
        <h2 class="panel-title">color</h2>
        <div class="color-row" style={{ marginBottom: '14px' }}>
          <span class="swatch swatch--lg" style={{ background: state?.base_color ?? '#000' }} />
          <code>{state?.base_color ?? '—'}</code>
          <input
            type="color"
            class="color-input"
            value={state?.base_color ?? '#000000'}
            disabled={!canSend}
            onChange={(e) => sendColor((e.currentTarget as HTMLInputElement).value)}
            aria-label="pick base color"
          />
        </div>
        <div class="presets" role="group" aria-label="preset colors">
          {PRESETS.map((c) => (
            <button
              key={c}
              type="button"
              class="preset"
              style={{ background: c }}
              disabled={!canSend}
              onClick={() => sendColor(c)}
              aria-label={`set color ${c}`}
            />
          ))}
        </div>
      </section>

      <section class="panel">
        <h2 class="panel-title">state</h2>
        {state ? (
          <dl class="kv">
            <dt>on</dt><dd>{String(state.on)}</dd>
            <dt>pattern_id</dt><dd>{state.pattern_id}</dd>
            <dt>base_color</dt>
            <dd class="color-row">
              <span class="swatch" style={{ background: state.base_color }} />
              <code>{state.base_color}</code>
            </dd>
            <dt>brightness</dt><dd>{state.brightness}</dd>
            <dt>led_count</dt><dd>{state.led_count}</dd>
            <dt>auth_mode</dt><dd>{state.auth_mode}</dd>
          </dl>
        ) : (
          <p class="hint">waiting for snapshot…</p>
        )}
      </section>
    </main>
  );
}
