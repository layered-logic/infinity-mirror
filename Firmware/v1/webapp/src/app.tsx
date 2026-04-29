import { useEffect, useMemo, useState } from 'preact/hooks';
import { ConnState, MirrorClient, defaultDeviceUrl } from './ws-client';
import { DeviceState, PatternId } from './protocol';

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

// 7 builtin patterns from control-protocol-spec.md §6.1.
const PATTERN_IDS: PatternId[] = [
  'solid', 'rainbow', 'scanner', 'spinner', 'random', 'breathing', 'twinkle',
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

  const sendBrightness = (value: number) => {
    setErr(null);
    client.setState({ brightness: value }).catch((e: Error) => setErr(e.message));
  };

  const sendPattern = (id: string) => {
    setErr(null);
    client.setState({ pattern_id: id }).catch((e: Error) => setErr(e.message));
  };

  // Local-state-during-drag: thumb tracks cursor smoothly via dragBrightness;
  // we only send to the device on release (`onChange`). Avoids saturating
  // esp_http_server's WS task with mid-drag traffic.
  const [dragBrightness, setDragBrightness] = useState<number | null>(null);
  const sliderValue = dragBrightness ?? state?.brightness ?? 0;

  // "Power" toggle = brightness 0 / restore last non-zero. The device has no
  // separate power concept here — brightness=0 *is* off, and the firmware's
  // physical button + RN app will follow the same convention.
  const [lastActiveBrightness, setLastActiveBrightness] = useState<number>(50);
  useEffect(() => {
    if (state && state.brightness > 0) setLastActiveBrightness(state.brightness);
  }, [state?.brightness]);
  const isOn = (state?.brightness ?? 0) > 0;
  const togglePower = () => sendBrightness(isOn ? 0 : lastActiveBrightness);

  // Factory reset is a UI mockup only this session. The wire op
  // (`factory_reset` → LL_EV_FACTORY_RESET) gets added later in the sprint
  // alongside the captive-portal / provisioning work it interacts with.
  const [stubNotice, setStubNotice] = useState<string | null>(null);
  const tryFactoryReset = () => {
    const ok = window.confirm(
      'Factory reset will erase all saved patterns, Wi-Fi credentials, and pairing data.\n\nThis cannot be undone. Continue?',
    );
    if (!ok) return;
    setStubNotice('factory reset stub — wire op lands later in sprint');
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
        <div class="controls-grid">
          <div class="control-cell">
            <h2 class="panel-title">power</h2>
            <button
              type="button"
              class={`power ${isOn ? 'power--on' : 'power--off'}`}
              disabled={!canSend || !state}
              onClick={togglePower}
              aria-pressed={isOn}
            >
              <span class="power-dot" aria-hidden="true" />
              <span class="power-label">{isOn ? 'on' : 'off'}</span>
            </button>
          </div>
          <div class="control-cell">
            <h2 class="panel-title">pattern</h2>
            <select
              class="pattern-select"
              value={state?.pattern_id ?? ''}
              disabled={!canSend}
              onChange={(e) => sendPattern((e.currentTarget as HTMLSelectElement).value)}
              aria-label="pattern"
            >
              {PATTERN_IDS.map((id) => (
                <option key={id} value={id}>{id}</option>
              ))}
            </select>
          </div>
        </div>
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
        <h2 class="panel-title">brightness</h2>
        <div class="brightness-row">
          <input
            type="range"
            min={0}
            max={100}
            step={1}
            value={sliderValue}
            disabled={!canSend}
            onInput={(e) => setDragBrightness(Number((e.currentTarget as HTMLInputElement).value))}
            onChange={(e) => {
              const v = Number((e.currentTarget as HTMLInputElement).value);
              setDragBrightness(null);
              sendBrightness(v);
            }}
            aria-label="brightness"
          />
          <code class="brightness-value">{sliderValue}</code>
        </div>
        <div class="brightness-marks" aria-hidden="true">
          <span>0</span><span>25</span><span>50</span><span>75</span><span>100</span>
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

      <section class="panel panel--danger">
        <h2 class="panel-title">danger zone</h2>
        <button
          type="button"
          class="danger-btn"
          onClick={tryFactoryReset}
        >
          factory reset
        </button>
        {stubNotice && <p class="hint stub-notice">{stubNotice}</p>}
      </section>
    </main>
  );
}
