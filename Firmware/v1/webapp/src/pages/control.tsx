import { useEffect, useState } from 'preact/hooks';
import { MirrorClient } from '../ws-client';
import { BRAND_SWATCHES, DeviceState, PatternId } from '../protocol';

interface ControlPageProps {
  client: MirrorClient;
  state: DeviceState | null;
  canSend: boolean;
  err: string | null;
  setErr: (s: string | null) => void;
}

// 7 builtin patterns from control-protocol-spec.md §6.1.
const PATTERN_IDS: PatternId[] = [
  'solid', 'rainbow', 'scanner', 'spinner', 'random', 'breathing', 'twinkle',
];

function normalizeHex(value: string): string {
  const v = value.startsWith('#') ? value : `#${value}`;
  return v.toUpperCase();
}

export function ControlPage({ client, state, canSend, err, setErr }: ControlPageProps) {
  // Tracks the last brightness we observed while the device was on. Lets
  // the power-on path restore a sensible brightness when the LEDs were
  // physically dark (state.brightness=0) before the user toggled on.
  const [lastActiveBrightness, setLastActiveBrightness] = useState<number>(50);
  useEffect(() => {
    if (state && state.brightness > 0) setLastActiveBrightness(state.brightness);
  }, [state?.brightness]);

  // The firmware's pattern_interp goes dark whenever state.on is false,
  // regardless of brightness — see core/pattern_interp/pattern_interp.c
  // around the `if (!s->on) push_frame(0)` branch. So the webapp's power
  // toggle has to mutate state.on, not just brightness, or the LEDs
  // never turn on. Mirrors the hardware button's LL_EV_POWER_TOGGLE
  // semantics exactly.
  const isOn = state?.on ?? false;
  const turnOn = (extras?: Record<string, unknown>) => {
    setErr(null);
    const patch: Record<string, unknown> = { on: true, ...extras };
    // If the device is currently dark (brightness=0), restore a usable
    // value so "turn on" actually produces visible light. Otherwise the
    // user gets an "on" indicator with no LEDs lit, which is exactly
    // the bug we're fixing.
    if ((state?.brightness ?? 0) === 0) patch.brightness = lastActiveBrightness;
    client.setState(patch).catch((e: Error) => setErr(e.message));
  };
  const turnOff = () => {
    setErr(null);
    client.setState({ on: false }).catch((e: Error) => setErr(e.message));
  };
  const togglePower = () => (isOn ? turnOff() : turnOn());

  // Color / pattern selections imply "I want the mirror to be on, with
  // this color / pattern" — auto-power-on when the device is off so the
  // user doesn't have to toggle power separately. Single-press hardware
  // gesture has the same auto-on behavior.
  const sendColor = (hex: string) => {
    if (!isOn) turnOn({ base_color: normalizeHex(hex) });
    else {
      setErr(null);
      client.setState({ base_color: normalizeHex(hex) }).catch((e: Error) => setErr(e.message));
    }
  };
  const sendPattern = (id: string) => {
    if (!isOn) turnOn({ pattern_id: id });
    else {
      setErr(null);
      client.setState({ pattern_id: id }).catch((e: Error) => setErr(e.message));
    }
  };
  const sendBrightness = (value: number) => {
    setErr(null);
    // Dragging the slider above 0 implies the user wants the LEDs visible.
    // Couple state.on with the slider so brightness↑ auto-turns-on, and
    // dragging to 0 turns off (matches the webapp's prior mental model).
    const patch: Record<string, unknown> = { brightness: value };
    if (value > 0 && !isOn) patch.on = true;
    if (value === 0 && isOn) patch.on = false;
    client.setState(patch).catch((e: Error) => setErr(e.message));
  };

  // Local-state-during-drag: thumb tracks cursor smoothly; we only send
  // on release. Avoids saturating esp_http_server's WS task with mid-drag
  // traffic (see post-mini-sprint-bugs.md #1).
  const [dragBrightness, setDragBrightness] = useState<number | null>(null);
  const sliderValue = dragBrightness ?? state?.brightness ?? 0;

  return (
    <>
      {err && (
        <section class="panel panel--error">
          <p class="hint err">{err}</p>
        </section>
      )}

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
          {BRAND_SWATCHES.map((s) => (
            <button
              key={s.name}
              type="button"
              class="preset"
              style={{ background: s.hex }}
              disabled={!canSend}
              onClick={() => sendColor(s.hex)}
              aria-label={`set color ${s.name}`}
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
    </>
  );
}
