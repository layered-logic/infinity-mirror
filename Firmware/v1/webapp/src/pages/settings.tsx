import { useState } from 'preact/hooks';
import { MirrorClient } from '../ws-client';
import { DeviceState } from '../protocol';
import { Route } from '../hooks';

interface SettingsPageProps {
  client: MirrorClient;
  state: DeviceState | null;
  canSend: boolean;
  err: string | null;
  setErr: (s: string | null) => void;
  onNavigate: (r: Route) => void;
}

export function SettingsPage({ client, state, canSend, err, setErr, onNavigate }: SettingsPageProps) {
  const [resetting, setResetting] = useState(false);
  const [resetDone, setResetDone] = useState(false);

  const tryFactoryReset = () => {
    const ok = window.confirm(
      'Factory reset will erase saved patterns, Wi-Fi credentials, and pairing data.\n\n' +
        'The mirror will return to its setup state. This cannot be undone.\n\nContinue?',
    );
    if (!ok) return;
    setErr(null);
    setResetting(true);
    client.factoryReset().then(
      () => { setResetting(false); setResetDone(true); },
      (e: Error) => { setResetting(false); setErr(e.message); },
    );
  };

  return (
    <>
      {err && (
        <section class="panel panel--error">
          <p class="hint err">{err}</p>
        </section>
      )}

      <section class="panel">
        <h2 class="panel-title">wi-fi</h2>
        <dl class="kv">
          <dt>network</dt>
          <dd>{state?.wifi_ssid ?? <span class="muted">— not on a network —</span>}</dd>
          <dt>mode</dt>
          <dd>
            {state?.provisioning_active
              ? 'setup mode (mirror is its own wi-fi)'
              : state?.wifi_ssid
                ? 'connected to your wi-fi'
                : 'unknown'}
          </dd>
        </dl>
        <button
          type="button"
          class="btn-secondary"
          style={{ marginTop: '12px' }}
          onClick={() => onNavigate('/setup')}
          disabled={!canSend}
        >
          reconfigure wi-fi
        </button>
      </section>

      <section class="panel">
        <h2 class="panel-title">diagnostics</h2>
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
            <dt>telemetry</dt><dd>{String(state.telemetry_enabled)}</dd>
            <dt>provisioning_active</dt><dd>{String(state.provisioning_active ?? false)}</dd>
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
          disabled={!canSend || resetting}
        >
          {resetting ? 'resetting…' : 'factory reset'}
        </button>
        {resetDone && (
          <p class="hint stub-notice" style={{ marginTop: '10px' }}>
            reset issued — device is returning to setup mode. you may need to rejoin its
            SoftAP to reconfigure.
          </p>
        )}
      </section>
    </>
  );
}
