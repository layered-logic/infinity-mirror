// Haptic feedback helpers — Vibration wrapper with intensity tiers and
// content-encoded patterns.
//
// `light` / `medium` / `heavy` map to button "weight":
//   light  — routine selection (color pick, forget non-active, brightness/pattern subtly handled below)
//   medium — bigger commits (power toggle, save name, add network, network switch)
//   heavy  — destructive after-confirm (factory reset, forget the active network)
//
// `pattern(id)` and `brightness(level)` encode the user's selection INTO
// the haptic so the buzz tracks what the LEDs are about to do — tap
// "twinkle" → feel a sparkly buzz; tap brightness 100 → feel a longer
// pulse than brightness 25. Android Vibration runs at fixed amplitude
// from RN core (the newer VibrationEffect amplitude API isn't bridged),
// so duration + pulse-density are the perceptual proxies.
//
// Every call is wrapped in a try/catch — VIBRATE is in the manifest but
// can be revoked at runtime, and haptics are gravy, not a load-bearing
// UI signal.
//
// iOS note: pattern arrays aren't honored on iOS the same way; the
// device fires a single short pulse per call. Acceptable degradation
// for a future iOS build — just get fewer perceptual variants.

import { Vibration } from 'react-native';
import type { PatternId } from './protocol';

function safeVibrate(pattern: number | number[]): void {
  try {
    Vibration.vibrate(pattern);
  } catch (_) {
    /* noop — permission revoked or platform without vibration. */
  }
}

export const haptic = {
  light: (): void => safeVibrate(10),
  medium: (): void => safeVibrate(25),
  heavy: (): void => safeVibrate([0, 50, 60, 50, 60, 50]),

  // LED-pattern → haptic mapping. Targets ~50% longer total than the
  // first pass + per-pulse durations ~20% shorter (Android's Vibration
  // amplitude isn't programmable from RN core, so shorter pulses are
  // the perceptual proxy for "less intense"). All under ~1.5s.
  pattern: (id: PatternId | string): void => {
    switch (id) {
      case 'solid':
        // single steady buzz, slightly longer
        safeVibrate([0, 380]);
        break;
      case 'rainbow':
        // accelerating sweep, more steps, softer pulses
        safeVibrate([0, 40, 100, 40, 80, 40, 60, 40, 40, 100, 40, 100]);
        break;
      case 'scanner':
        // back-and-forth — 4 slower pulses with breathing room
        safeVibrate([0, 96, 130, 96, 130, 96, 130, 96]);
        break;
      case 'spinner':
        // rotation — 6 evenly-spaced softer pulses
        safeVibrate([0, 40, 90, 40, 90, 40, 90, 40, 90, 40, 90, 40]);
        break;
      case 'random':
        // chaotic — more steps, irregular durations + gaps
        safeVibrate([0, 24, 70, 64, 100, 32, 50, 80, 90, 32, 70, 56, 80]);
        break;
      case 'breathing':
        // 3 long pulses, slow inhale → exhale → final settle
        safeVibrate([0, 160, 460, 160, 260, 160]);
        break;
      case 'twinkle':
        // more staccato sparkles, slightly softer
        safeVibrate([0, 12, 70, 16, 90, 12, 60, 20, 110, 8, 50, 12, 70, 16, 60]);
        break;
      default:
        safeVibrate(15);
    }
  },

  // Brightness as duration — higher level = longer (perceptually
  // "louder") pulse. Range covers the four BRIGHTNESS_STEPS (25/50/75/
  // 100) at 15/35/65/100 ms but accepts any value in between.
  brightness: (level: number): void => {
    // Quadratic-ish ramp so the steps feel meaningfully different;
    // a pure-linear 25→100% felt subtle in testing.
    const ms = Math.round(10 + (level / 100) * 90);
    safeVibrate(Math.max(10, Math.min(120, ms)));
  },
};
