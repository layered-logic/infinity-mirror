import { useEffect, useState } from 'preact/hooks';
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
  // Paired-mode secret this browser holds (null = none) + the setter that
  // syncs it to component state, the client, and localStorage.
  pairSecret: string | null;
  applySecret: (secret: string | null) => void;
}

export function SettingsPage({
  client, state, canSend, err, setErr, onNavigate, pairSecret, applySecret,
}: SettingsPageProps) {
  const [resetting, setResetting] = useState(false);
  const [resetDone, setResetDone] = useState(false);

  // Rename UI. Local input buffer seeded from state, separate save state
  // so the button can show idle / saving / saved without bouncing the
  // whole page. Cap matches the firmware's ll_state_t.name (32 + null).
  const [nameInput, setNameInput] = useState<string>('');
  const [renameState, setRenameState] = useState<'idle' | 'saving' | 'saved'>('idle');
  useEffect(() => {
    setNameInput(state?.name ?? '');
  }, [state?.name]);

  const submitRename = () => {
    const trimmed = nameInput.trim();
    if (trimmed.length > 32) {
      setErr('name must be 32 characters or fewer');
      return;
    }
    setErr(null);
    setRenameState('saving');
    client.setState({ name: trimmed }).then(
      () => {
        setRenameState('saved');
        setTimeout(() => setRenameState('idle'), 1500);
      },
      (e: Error) => { setRenameState('idle'); setErr(e.message); },
    );
  };

  // Pairing UI (LL-078). `pairForm` selects which form is open; the new /
  // confirm / old fields are shared by the enable and rotate forms.
  const [pairForm, setPairForm] = useState<'none' | 'enable' | 'rotate'>('none');
  const [pairNew, setPairNew] = useState('');
  const [pairNewConfirm, setPairNewConfirm] = useState('');
  const [pairOld, setPairOld] = useState('');
  const [pairBusy, setPairBusy] = useState(false);

  const closePairForm = () => {
    setPairForm('none');
    setPairNew('');
    setPairNewConfirm('');
    setPairOld('');
  };

  // Enable paired mode. The set_auth_mode frame goes out unsigned (device
  // still open); on success the app adopts the secret so later frames sign.
  const submitEnablePairing = () => {
    if (pairNew.length < 1) { setErr('enter a passphrase'); return; }
    if (pairNew !== pairNewConfirm) { setErr('passphrases don’t match'); return; }
    setErr(null);
    setPairBusy(true);
    client.setAuthMode('paired', pairNew).then(
      () => { applySecret(pairNew); setPairBusy(false); closePairForm(); },
      (e: Error) => { setErr(e.message); setPairBusy(false); },
    );
  };

  // Change the secret in place. The rotate_secret frame is signed with the
  // current secret; on success the app adopts the new one.
  const submitRotate = () => {
    if (pairNew.length < 1) { setErr('enter a new passphrase'); return; }
    if (pairNew !== pairNewConfirm) { setErr('new passphrases don’t match'); return; }
    setErr(null);
    setPairBusy(true);
    client.rotateSecret(pairOld, pairNew).then(
      () => { applySecret(pairNew); setPairBusy(false); closePairForm(); },
      (e: Error) => { setErr(e.message); setPairBusy(false); },
    );
  };

  const removePairing = () => {
    const ok = window.confirm(
      'Remove the passphrase?\n\n' +
        'Anyone on your Wi-Fi will be able to control this mirror again.',
    );
    if (!ok) return;
    setErr(null);
    client.setAuthMode('open').then(
      () => applySecret(null),
      (e: Error) => setErr(e.message),
    );
  };

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
        <h2 class="panel-title">name</h2>
        <dl class="kv">
          <dt>id</dt>
          <dd><code>{state?.id ?? <span class="muted">—</span>}</code></dd>
        </dl>
        <p class="hint muted" style={{ margin: '8px 0' }}>
          Used by the LL app to disambiguate when multiple mirrors share a network.
          Empty falls back to the hardware id.
        </p>
        <input
          type="text"
          value={nameInput}
          maxLength={32}
          placeholder="Living Room"
          autoCapitalize="words"
          onInput={(e) => setNameInput((e.currentTarget as HTMLInputElement).value)}
          disabled={!canSend}
          style={{
            width: '100%',
            boxSizing: 'border-box',
            padding: '8px 10px',
            marginBottom: '8px',
          }}
        />
        <button
          type="button"
          class="btn-secondary"
          onClick={submitRename}
          disabled={
            !canSend ||
            renameState === 'saving' ||
            nameInput.trim() === (state?.name ?? '')
          }
        >
          {renameState === 'idle' && 'save name'}
          {renameState === 'saving' && 'saving…'}
          {renameState === 'saved' && 'saved ✓'}
        </button>
      </section>

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
        <h2 class="panel-title">telemetry</h2>
        <p class="hint muted" style={{ margin: '0 0 10px 0' }}>
          Share anonymous diagnostics: once-a-day beacon with uptime, free heap, RSSI,
          and the last reboot reason. No location, no usage data, no account link.
          Opt-in; off by default.
        </p>
        <label style={{ display: 'flex', alignItems: 'center', gap: '8px' }}>
          <input
            type="checkbox"
            checked={state?.telemetry_enabled ?? false}
            disabled={!canSend}
            onChange={(e) => {
              const enabled = (e.currentTarget as HTMLInputElement).checked;
              setErr(null);
              client.setState({ telemetry_enabled: enabled }).catch((x: Error) => setErr(x.message));
            }}
          />
          <span>share anonymous diagnostics</span>
        </label>
      </section>

      <section class="panel">
        <h2 class="panel-title">pairing</h2>

        {!state && <p class="hint">waiting for snapshot…</p>}

        {state && state.auth_mode === 'open' && pairForm !== 'enable' && (
          <>
            <p class="hint muted" style={{ margin: '0 0 10px 0' }}>
              This mirror is open — anyone on your Wi-Fi can control it. Add a
              passphrase to require it for control.
            </p>
            <button
              type="button"
              class="btn-secondary"
              disabled={!canSend}
              onClick={() => {
                setErr(null);
                setPairNew('');
                setPairNewConfirm('');
                setPairForm('enable');
              }}
            >
              require a passphrase
            </button>
          </>
        )}

        {state && state.auth_mode === 'open' && pairForm === 'enable' && (
          <>
            <p class="hint muted" style={{ margin: '0 0 10px 0' }}>
              Set a passphrase. Every controller will need it. If it's lost, a
              factory reset (hold the recessed button 10s) is the only way back in.
            </p>
            <input
              type="password"
              value={pairNew}
              placeholder="passphrase"
              maxLength={48}
              onInput={(e) => setPairNew((e.currentTarget as HTMLInputElement).value)}
              style={{ width: '100%', boxSizing: 'border-box', padding: '8px 10px', marginBottom: '8px' }}
            />
            <input
              type="password"
              value={pairNewConfirm}
              placeholder="confirm passphrase"
              maxLength={48}
              onInput={(e) => setPairNewConfirm((e.currentTarget as HTMLInputElement).value)}
              style={{ width: '100%', boxSizing: 'border-box', padding: '8px 10px', marginBottom: '8px' }}
            />
            <div style={{ display: 'flex', gap: '8px', flexWrap: 'wrap' }}>
              <button type="button" class="btn-secondary" onClick={closePairForm} disabled={pairBusy}>
                cancel
              </button>
              <button
                type="button"
                class="btn-secondary"
                onClick={submitEnablePairing}
                disabled={!canSend || pairBusy}
              >
                {pairBusy ? 'enabling…' : 'enable'}
              </button>
            </div>
          </>
        )}

        {state && state.auth_mode === 'paired' && pairForm !== 'rotate' && (
          <>
            <p class="hint muted" style={{ margin: '0 0 10px 0' }}>
              Paired — control requires the passphrase.
              {!pairSecret &&
                ' This browser doesn’t have it stored; reconnect and enter it to manage pairing.'}
            </p>
            {pairSecret && (
              <div style={{ display: 'flex', gap: '8px', flexWrap: 'wrap' }}>
                <button
                  type="button"
                  class="btn-secondary"
                  disabled={!canSend}
                  onClick={() => {
                    setErr(null);
                    setPairOld('');
                    setPairNew('');
                    setPairNewConfirm('');
                    setPairForm('rotate');
                  }}
                >
                  change passphrase
                </button>
                <button type="button" class="btn-secondary" onClick={removePairing} disabled={!canSend}>
                  remove passphrase
                </button>
              </div>
            )}
          </>
        )}

        {state && state.auth_mode === 'paired' && pairForm === 'rotate' && (
          <>
            <p class="hint muted" style={{ margin: '0 0 10px 0' }}>
              Change the passphrase. Every controller will need the new one.
            </p>
            <input
              type="password"
              value={pairOld}
              placeholder="current passphrase"
              maxLength={48}
              onInput={(e) => setPairOld((e.currentTarget as HTMLInputElement).value)}
              style={{ width: '100%', boxSizing: 'border-box', padding: '8px 10px', marginBottom: '8px' }}
            />
            <input
              type="password"
              value={pairNew}
              placeholder="new passphrase"
              maxLength={48}
              onInput={(e) => setPairNew((e.currentTarget as HTMLInputElement).value)}
              style={{ width: '100%', boxSizing: 'border-box', padding: '8px 10px', marginBottom: '8px' }}
            />
            <input
              type="password"
              value={pairNewConfirm}
              placeholder="confirm new passphrase"
              maxLength={48}
              onInput={(e) => setPairNewConfirm((e.currentTarget as HTMLInputElement).value)}
              style={{ width: '100%', boxSizing: 'border-box', padding: '8px 10px', marginBottom: '8px' }}
            />
            <div style={{ display: 'flex', gap: '8px', flexWrap: 'wrap' }}>
              <button type="button" class="btn-secondary" onClick={closePairForm} disabled={pairBusy}>
                cancel
              </button>
              <button
                type="button"
                class="btn-secondary"
                onClick={submitRotate}
                disabled={!canSend || pairBusy}
              >
                {pairBusy ? 'changing…' : 'change'}
              </button>
            </div>
          </>
        )}
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
            <dt>id</dt><dd><code>{state.id ?? '—'}</code></dd>
            <dt>name</dt><dd>{state.name && state.name.length > 0 ? state.name : <span class="muted">—</span>}</dd>
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
