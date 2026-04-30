import { useEffect, useState } from 'preact/hooks';
import { MirrorClient } from '../ws-client';
import { DeviceState } from '../protocol';
import { Route } from '../hooks';

interface SetupPageProps {
  client: MirrorClient;
  state: DeviceState | null;
  canSend: boolean;
  err: string | null;
  setErr: (s: string | null) => void;
  onNavigate: (r: Route) => void;
  // Notifies App when the user has acknowledged a creds submission. App
  // uses this to swap the connection-status badge for a static
  // "switching networks…" label so the cycling closed/connecting badge
  // doesn't confuse the user during the intentional SoftAP teardown.
  onSubmittedChange: (submitted: boolean) => void;
}

type SubmitState = 'idle' | 'submitting' | 'submitted';

// Matches the firmware's LL_APPLY_FALLBACK_US (provisioning.c). If those
// drift out of sync the user just sees a misleading hint, not a real
// failure — but worth keeping aligned.
const FALLBACK_SECONDS = 15;

export function SetupPage({
  client, state, canSend, err, setErr, onNavigate, onSubmittedChange,
}: SetupPageProps) {
  const [ssid, setSsid] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [submitState, setSubmitState] = useState<SubmitState>('idle');
  const [submitTime, setSubmitTime] = useState<number | null>(null);
  // Tick-driven seconds elapsed since submit. Drives the countdown so we
  // can show the user when the device should be reachable on the new
  // network, or — if the creds were wrong — when the firmware's fallback
  // timer will return the device to setup mode.
  const [now, setNow] = useState(() => Date.now());
  useEffect(() => {
    if (submitTime === null) return;
    const t = setInterval(() => setNow(Date.now()), 250);
    return () => clearInterval(t);
  }, [submitTime]);
  const elapsed = submitTime ? Math.floor((now - submitTime) / 1000) : 0;
  const secondsLeft = Math.max(0, FALLBACK_SECONDS - elapsed);

  const ssidValid = ssid.length >= 1 && ssid.length <= 32;
  const passwordValid = password.length === 0 || (password.length >= 8 && password.length <= 64);
  const formValid = ssidValid && passwordValid;

  const onSubmit = (ev: Event) => {
    ev.preventDefault();
    if (!formValid || !canSend) return;
    setErr(null);
    setSubmitState('submitting');
    client.setWifiCreds({ ssid, password }).then(
      () => {
        // Device tears down SoftAP within ~100ms of acking us; our socket
        // dies and ws-client's reconnect-with-backoff starts trying. The
        // user has to switch their phone/laptop wifi to the new network
        // before the webapp can reach the device again — the post-submit
        // view explains this explicitly.
        setSubmitState('submitted');
        setSubmitTime(Date.now());
        onSubmittedChange(true);
      },
      (e: Error) => {
        setSubmitState('idle');
        setErr(e.message);
      },
    );
  };

  // Reset the App-level "submitted" flag if the user navigates away from
  // /setup or if this page unmounts. Otherwise the header would stay
  // stuck on "switching networks…" indefinitely.
  useEffect(() => () => onSubmittedChange(false), [onSubmittedChange]);

  if (submitState === 'submitted') {
    return (
      <>
        <section class="panel panel--accent">
          <h2 class="panel-title">credentials sent</h2>
          <p>
            the mirror is leaving its setup network and joining{' '}
            <strong>{ssid}</strong>.
          </p>

          <div class="countdown" aria-live="polite">
            {secondsLeft > 0 ? (
              <>
                <span class="countdown-num">{secondsLeft}</span>
                <span class="countdown-label">
                  if the credentials are wrong, the mirror will return to setup
                  mode in {secondsLeft}s
                </span>
              </>
            ) : (
              <span class="countdown-label">
                if the mirror joined <strong>{ssid}</strong> successfully, it's
                reachable on that network now. if not, it should be back in
                setup mode — re-join <code>LL-Mirror-...</code> and reload to
                retry.
              </span>
            )}
          </div>

          <p style={{ marginTop: '16px' }}>
            <strong>to keep using the webapp:</strong>
          </p>
          <ol class="next-steps">
            <li>
              switch your phone or laptop's wi-fi from the mirror's setup network
              back to <strong>{ssid}</strong>
            </li>
            <li>
              open <code>http://layered-logic-mirror.local/</code>{' '}
              <span class="muted">
                (or whatever IP your router gives the mirror)
              </span>
            </li>
          </ol>
        </section>
      </>
    );
  }

  return (
    <>
      {err && (
        <section class="panel panel--error">
          <p class="hint err">{err}</p>
        </section>
      )}

      {/* Primary affordance for the "I just want to use it now" user.
          Sits above the wifi setup form because direct-control is the
          fastest path to a working demo, and provisioning can be done
          later from /settings → reconfigure. */}
      <section class="panel panel--accent">
        <h2 class="panel-title">control directly</h2>
        <p class="hint" style={{ marginBottom: '12px' }}>
          skip wi-fi setup and use the mirror through this setup network.
          fastest path; you stay connected via the mirror's own wi-fi.
        </p>
        <button
          type="button"
          class="btn-primary"
          style={{ width: '100%' }}
          onClick={() => onNavigate('/')}
        >
          control mirror now
        </button>
      </section>

      <section class="panel">
        <h2 class="panel-title">or — connect mirror to your wi-fi</h2>
        <p class="hint" style={{ marginBottom: '16px' }}>
          give the mirror a network to join. once it's on your wi-fi, you can
          reach it from any device on the same network at{' '}
          <code>http://layered-logic-mirror.local/</code>.
        </p>

        <form class="setup-form" onSubmit={onSubmit}>
          <label class="field">
            <span class="field-label">network name (SSID)</span>
            <input
              type="text"
              class="field-input"
              value={ssid}
              onInput={(e) => setSsid((e.currentTarget as HTMLInputElement).value)}
              placeholder="MyWiFi"
              maxLength={32}
              required
              autoComplete="off"
              autoCapitalize="none"
              autoCorrect="off"
              spellcheck={false}
            />
          </label>

          <label class="field">
            <span class="field-label">password</span>
            <div class="field-input-row">
              <input
                type={showPassword ? 'text' : 'password'}
                class="field-input"
                value={password}
                onInput={(e) => setPassword((e.currentTarget as HTMLInputElement).value)}
                placeholder="(leave empty for open networks)"
                minLength={password.length === 0 ? 0 : 8}
                maxLength={64}
                autoComplete="off"
              />
              <button
                type="button"
                class="field-toggle"
                onClick={() => setShowPassword((v) => !v)}
                aria-label={showPassword ? 'hide password' : 'show password'}
              >
                {showPassword ? 'hide' : 'show'}
              </button>
            </div>
            {password.length > 0 && password.length < 8 && (
              <span class="field-hint err">WPA2 passwords are at least 8 characters.</span>
            )}
          </label>

          <button
            type="submit"
            class="btn-secondary"
            disabled={!formValid || !canSend || submitState !== 'idle'}
          >
            {submitState === 'submitting' ? 'connecting…' : 'connect to wi-fi'}
          </button>
        </form>
      </section>

      {state && state.provisioning_active === false && state.wifi_ssid && (
        <section class="panel">
          <p class="hint">
            mirror is already on <strong>{state.wifi_ssid}</strong>. submitting new
            credentials will switch it to a different network.
          </p>
        </section>
      )}
    </>
  );
}
