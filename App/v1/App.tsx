import { useEffect, useRef, useState } from 'react';
import {
  Alert,
  Platform,
  Pressable,
  SafeAreaView,
  ScrollView,
  StatusBar,
  StyleSheet,
  Text,
  TextInput,
  Vibration,
  View,
} from 'react-native';

// Top-padding fallback for the title bar on Android. The native
// SafeAreaView (RN core) only handles iOS notches; on Android the
// status bar gets drawn over by default. Previously we read
// StatusBar.currentHeight at module-load and applied it as static
// padding, which broke after cold-start races on Android 15 — the
// native value can be 0 until the activity has laid out, leaving the
// title under the status bar with no easy way to tap into Settings.
//
// New approach: read currentHeight at render time and floor it at
// MIN_TOP_PADDING. The floor matches the largest typical Android
// status bar (Pixel 9 / Android 15 ≈ 28-44dp depending on cutout) so
// even if the native value comes back stale-zero, the title is
// guaranteed to land below the system bars.
const MIN_TOP_PADDING = 32;
function topPadding(): number {
  if (Platform.OS !== 'android') return 0;
  return Math.max(StatusBar.currentHeight ?? 0, MIN_TOP_PADDING);
}

import { ConnState, MirrorClient } from './src/ws-client';
import { BRAND_SWATCHES, DeviceState, PatternId, WifiNetwork } from './src/protocol';
import { FindResult, findMirrors } from './src/find-mirror';

const HOME_URL = 'ws://10.123.210.61/ws';
const SOFTAP_URL = 'ws://192.168.4.1/ws';
const DEFAULT_OTA_URL = 'http://192.168.223.176:8000/layered_logic_mirror_standard.bin';

// Matches firmware's LL_APPLY_FALLBACK_US in provisioning.c. If they
// drift, the user just sees a misleading hint, not a real failure.
const FALLBACK_SECONDS = 15;

const BRIGHTNESS_STEPS = [25, 50, 75, 100];

const PATTERNS: PatternId[] = [
  'solid', 'rainbow', 'scanner', 'spinner', 'random', 'breathing', 'twinkle',
];

type SubmitState = 'idle' | 'submitting' | 'submitted';
type Route = 'controls' | 'settings';

function App() {
  const [route, setRoute] = useState<Route>('controls');
  const [url, setUrl] = useState(HOME_URL);
  const [conn, setConn] = useState<ConnState>('idle');
  const [lastError, setLastError] = useState<string | null>(null);
  const [state, setState] = useState<DeviceState | null>(null);
  const clientRef = useRef<MirrorClient | null>(null);

  // Provisioning UI state — lives in the App so it survives the socket
  // teardown that follows a successful set_wifi_creds.
  const [setupBypassed, setSetupBypassed] = useState(false);
  const [ssid, setSsid] = useState('');
  const [password, setPassword] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [submitState, setSubmitState] = useState<SubmitState>('idle');
  const [submitTime, setSubmitTime] = useState<number | null>(null);
  const [now, setNow] = useState(() => Date.now());
  const [scanning, setScanning] = useState(false);
  const [otaUrl, setOtaUrl] = useState(DEFAULT_OTA_URL);
  const [otaState, setOtaState] = useState<'idle' | 'sending' | 'rebooting'>('idle');
  // True when the user manually triggered Wi-Fi reconfigure from
  // Settings (vs. the device-driven "provisioning_active" auto-route).
  // Both cases render the same SSID/password form.
  const [reconfiguringWifi, setReconfiguringWifi] = useState(false);
  // Discovery results from the most recent Find-mirror scan. Empty until
  // the user triggers a scan; populated with all responders so the user
  // can disambiguate when multiple mirrors live on the same network.
  const [foundMirrors, setFoundMirrors] = useState<FindResult[]>([]);
  // Rename UI — local input buffer + submit state. Shown in Settings.
  const [nameInput, setNameInput] = useState('');
  const [renameState, setRenameState] = useState<SubmitState>('idle');
  // Multi-network store (LL-046 step 5). `savedNetworks === null` means
  // "haven't fetched yet"; an empty array means "the device knows it has
  // none." Both render distinct UI states. The inline add-network form
  // is gated behind `addingNetwork` so the network list stays compact.
  const [savedNetworks, setSavedNetworks] = useState<WifiNetwork[] | null>(null);
  const [addingNetwork, setAddingNetwork] = useState(false);
  const [addSsid, setAddSsid] = useState('');
  const [addPassword, setAddPassword] = useState('');
  const [addNetState, setAddNetState] = useState<SubmitState>('idle');
  // SSID we're currently asking the mirror to switch to. Drives the
  // "Switching to X…" banner. Cleared when the next state broadcast
  // confirms the new wifi_ssid (success or fallback) — see the useEffect
  // below tied to state?.wifi_ssid. The mirror does the switch async
  // (sub-bus event with a 250ms response-flush yield), and the WS
  // socket dies mid-switch, so this can take a few seconds to clear.
  const [switchingTo, setSwitchingTo] = useState<string | null>(null);

  useEffect(() => {
    return () => clientRef.current?.disconnect();
  }, []);

  useEffect(() => {
    if (conn !== 'open') return;
    clientRef.current?.getState().then(setState).catch((e) => setLastError((e as Error).message));
  }, [conn]);

  // Reset the bypass flag whenever the device transitions back into
  // provisioning mode — otherwise reusing the app across multiple
  // factory-reset cycles would skip Setup forever after the first bypass.
  useEffect(() => {
    if (state?.provisioning_active === false) setSetupBypassed(false);
  }, [state?.provisioning_active]);

  // If we land on Settings without being connected (e.g. socket dropped
  // while user was on the Settings page), fall back to controls — the
  // controls page renders the connect form.
  useEffect(() => {
    if (route === 'settings' && conn !== 'open') setRoute('controls');
  }, [route, conn]);

  // Seed the rename input with the device's current name whenever the
  // user opens Settings, so the field reflects what the mirror's actually
  // called rather than a stale draft from a prior session.
  useEffect(() => {
    if (route === 'settings') setNameInput(state?.name ?? '');
  }, [route, state?.name]);

  // Refetch the saved-networks list when the user opens Settings, and
  // again whenever the device's reported count or active ssid changes
  // (covers cross-client adds/removes AND the switch case where the
  // count is unchanged but the active row moved). Failures are
  // non-fatal — the row UI gracefully shows "couldn't load" if the
  // list is null.
  useEffect(() => {
    if (route !== 'settings' || conn !== 'open') return;
    clientRef.current?.listWifiNetworks()
      .then((next) => {
        setSavedNetworks(next);
        // If the broadcast confirms our requested switch (or settled
        // somewhere else after a fallback), clear the "Switching to…"
        // banner. The new active SSID — whichever one it is — is
        // authoritative now.
        if (switchingTo !== null) {
          const active = next.find((n) => n.is_active);
          if (active && state?.wifi_ssid === active.ssid) {
            setSwitchingTo(null);
          }
        }
      })
      .catch((e) => setLastError((e as Error).message));
    // switchingTo intentionally not in deps — we read it inside the
    // handler but don't want to re-fire just because the banner is up.
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [route, conn, state?.wifi_saved_count, state?.wifi_ssid]);

  // Drive the post-submit countdown.
  useEffect(() => {
    if (submitTime === null) return;
    const t = setInterval(() => setNow(Date.now()), 250);
    return () => clearInterval(t);
  }, [submitTime]);

  const connect = () => {
    clientRef.current?.disconnect();
    setLastError(null);
    setReconfiguringWifi(false);
    setSubmitState('idle');
    setSubmitTime(null);
    const c = new MirrorClient({
      url,
      onConn: setConn,
      onState: setState,
      onError: setLastError,
    });
    clientRef.current = c;
    c.connect();
  };

  const disconnect = () => {
    clientRef.current?.disconnect();
    clientRef.current = null;
    setConn('idle');
  };

  const apply = async (patch: Partial<DeviceState>) => {
    try { await clientRef.current!.setState(patch); }
    catch (e) { setLastError((e as Error).message); }
  };

  const submitCreds = async () => {
    if (!clientRef.current) return;
    const ssidValid = ssid.length >= 1 && ssid.length <= 32;
    const passwordValid = password.length === 0 || (password.length >= 8 && password.length <= 64);
    if (!ssidValid || !passwordValid) return;
    setLastError(null);
    setSubmitState('submitting');
    try {
      await clientRef.current.setWifiCreds({ ssid, password });
      // Device tears down SoftAP within ~100ms; ws-client's reconnect
      // backoff will retry but won't reach the device until the user
      // switches their phone Wi-Fi back to the home network.
      setSubmitState('submitted');
      setSubmitTime(Date.now());
    } catch (e) {
      setSubmitState('idle');
      setLastError((e as Error).message);
    }
  };

  // Add a network to the saved-list. Different from submitCreds —
  // add_wifi_network does NOT switch the active connection (per
  // multi-network-design §7.2). The mirror keeps its current network;
  // the SM will pick this one next time it scans and the saved one
  // happens to be the highest-priority visible match.
  const submitAddNetwork = async () => {
    if (!clientRef.current) return;
    const ssidValid = addSsid.length >= 1 && addSsid.length <= 32;
    const passwordValid =
      addPassword.length === 0 ||
      (addPassword.length >= 8 && addPassword.length <= 64);
    if (!ssidValid || !passwordValid) return;
    setLastError(null);
    setAddNetState('submitting');
    try {
      await clientRef.current.addWifiNetwork({
        ssid: addSsid,
        password: addPassword,
      });
      // Refresh the list immediately — broadcast_state will arrive too,
      // but the inline refetch keeps the UI from looking stale for a frame.
      const next = await clientRef.current.listWifiNetworks();
      setSavedNetworks(next);
      setAddNetState('idle');
      setAddingNetwork(false);
      setAddSsid('');
      setAddPassword('');
    } catch (e) {
      setAddNetState('idle');
      setLastError((e as Error).message);
    }
  };

  // Switch the active connection to a saved network. Bumps the
  // device-side priority so the SM's scan-and-pick lands on this entry,
  // even if another saved network is currently connected.
  //
  // The actual switch is async on the device — the SM tears down the
  // current STA, scans, picks the bumped entry, connects (or falls back
  // if it's not visible / has bad creds). The WS socket dies mid-switch
  // and auto-reconnect picks up afterward. Until the new state arrives:
  //   - Drop is_active locally (per Bill's UX call) so the user sees
  //     immediate acknowledgement that their tap landed.
  //   - Show a "Switching to X…" banner.
  //   - Vibrate briefly for haptic confirmation that the tap registered
  //     even before any visual update lands.
  // The useEffect above clears switchingTo once the broadcast confirms
  // a settled wifi_ssid.
  const connectToNetwork = async (entry: WifiNetwork) => {
    if (!clientRef.current) return;
    setLastError(null);
    Vibration.vibrate(15);
    setSwitchingTo(entry.ssid);
    setSavedNetworks((prev) =>
      prev?.map((n) => (n.is_active ? { ...n, is_active: false } : n)) ?? prev,
    );
    try {
      await clientRef.current.connectWifiNetwork(entry.ssid);
    } catch (e) {
      setLastError((e as Error).message);
      setSwitchingTo(null);
    }
  };

  // Forget a saved network. Per design-doc §4.3: silent for non-active
  // entries, confirm dialog when removing the network we're currently
  // on (because it triggers a disconnect + scan).
  const forgetNetwork = (entry: WifiNetwork) => {
    const doForget = async () => {
      if (!clientRef.current) return;
      try {
        await clientRef.current.removeWifiNetwork(entry.ssid);
        const next = await clientRef.current.listWifiNetworks();
        setSavedNetworks(next);
      } catch (e) {
        setLastError((e as Error).message);
      }
    };
    if (entry.is_active) {
      Alert.alert(
        `Forget ${entry.ssid}?`,
        'The mirror will disconnect from this network. If it can reach another saved network, it’ll switch automatically; otherwise it’ll wait until one is in range.',
        [
          { text: 'Cancel', style: 'cancel' },
          { text: 'Forget', style: 'destructive', onPress: doForget },
        ],
      );
    } else {
      // Non-active networks are forgotten silently — same UX norm as
      // iOS/macOS for "Remove network" on networks you're not on.
      doForget();
    }
  };

  const findOnSubnet = async () => {
    setScanning(true);
    setLastError(null);
    setFoundMirrors([]);
    try {
      const matches = await findMirrors();
      if (matches.length === 0) {
        setLastError('no mirror found on this network');
      } else if (matches.length === 1) {
        // Exactly one — auto-select. Clear the picker (it'd just have a
        // single row otherwise) and drop the URL into the input.
        setUrl(matches[0].url);
        setFoundMirrors([]);
      } else {
        // Multiple responders — surface the picker. URL stays untouched
        // until the user taps a row.
        setFoundMirrors(matches);
      }
    } catch (e) {
      setLastError((e as Error).message);
    } finally {
      setScanning(false);
    }
  };

  const pickMirror = (m: FindResult) => {
    setUrl(m.url);
    setFoundMirrors([]);
  };

  const startOta = () => {
    Alert.alert(
      'Update firmware?',
      `The mirror will pull a new binary from\n${otaUrl}\n\nIf the URL is unreachable or the binary is invalid, the mirror stays on its current firmware.`,
      [
        { text: 'Cancel', style: 'cancel' },
        {
          text: 'Update',
          onPress: async () => {
            if (!clientRef.current) return;
            setLastError(null);
            setOtaState('sending');
            try {
              await clientRef.current.startOta(otaUrl);
              setOtaState('rebooting');
            } catch (e) {
              setOtaState('idle');
              setLastError((e as Error).message);
            }
          },
        },
      ],
    );
  };

  const submitRename = async () => {
    if (!clientRef.current) return;
    const trimmed = nameInput.trim();
    // Server-side cap is 32 chars (ll_state_t.name is char[33], 1 reserved
    // for null). Empty is a legal "clear back to id" value.
    if (trimmed.length > 32) {
      setLastError('name must be 32 characters or fewer');
      return;
    }
    setLastError(null);
    setRenameState('submitting');
    try {
      await clientRef.current.setState({ name: trimmed });
      setRenameState('submitted');
      // Server broadcasts the new state, which lands via onState — don't
      // also stamp the input back to local state, let it stay editable.
      setTimeout(() => setRenameState('idle'), 1500);
    } catch (e) {
      setRenameState('idle');
      setLastError((e as Error).message);
    }
  };

  const factoryReset = () => {
    Alert.alert(
      'Factory reset?',
      'This wipes Wi-Fi credentials and all settings. The mirror falls back to SoftAP.',
      [
        { text: 'Cancel', style: 'cancel' },
        {
          text: 'Reset',
          style: 'destructive',
          onPress: async () => {
            try { await clientRef.current!.factoryReset(); }
            catch (e) { setLastError((e as Error).message); }
          },
        },
      ],
    );
  };

  const ready = conn === 'open' && state !== null;
  const provisioningSetup = ready && state?.provisioning_active === true && !setupBypassed;
  const inSetup = provisioningSetup || (ready && reconfiguringWifi);
  const elapsed = submitTime ? Math.floor((now - submitTime) / 1000) : 0;
  const secondsLeft = Math.max(0, FALLBACK_SECONDS - elapsed);
  const ssidValid = ssid.length >= 1 && ssid.length <= 32;
  const passwordValid = password.length === 0 || (password.length >= 8 && password.length <= 64);
  const formValid = ssidValid && passwordValid;

  return (
    <SafeAreaView style={styles.root}>
      <StatusBar barStyle="light-content" backgroundColor="#0B0A0F" />
      <ScrollView contentContainerStyle={[styles.body, { paddingTop: 16 + topPadding() }]}>
        <View style={styles.headerBar}>
          <Text style={styles.h1}>
            {route === 'settings' ? 'Settings' : 'LL Mirror'}
          </Text>
          {!inSetup && route === 'controls' && conn === 'open' && (
            <Pressable
              onPress={() => setRoute('settings')}
              style={styles.navBtn}
              hitSlop={10}
            >
              <Text style={styles.navBtnText}>⚙</Text>
            </Pressable>
          )}
          {!inSetup && route === 'settings' && (
            <Pressable
              onPress={() => setRoute('controls')}
              style={styles.navBtn}
              hitSlop={10}
            >
              <Text style={styles.navBtnText}>← Back</Text>
            </Pressable>
          )}
        </View>

        {!inSetup && route === 'controls' && (
          <>
            <Text style={styles.label}>Mirror URL</Text>
            <TextInput
              value={url}
              onChangeText={setUrl}
              autoCapitalize="none"
              autoCorrect={false}
              style={styles.input}
            />
            <View style={styles.row}>
              <Pressable
                onPress={findOnSubnet}
                disabled={scanning}
                style={[styles.urlPreset, scanning && styles.urlPresetDisabled]}
              >
                <Text style={styles.urlPresetText}>
                  {scanning ? 'Searching…' : 'Find mirror'}
                </Text>
              </Pressable>
              <Pressable
                onPress={() => setUrl(SOFTAP_URL)}
                style={[styles.urlPreset, url === SOFTAP_URL && styles.urlPresetActive]}
              >
                <Text style={styles.urlPresetText}>SoftAP</Text>
              </Pressable>
            </View>

            {foundMirrors.length > 0 && (
              <View style={styles.pickerBlock}>
                <Text style={styles.label}>
                  {foundMirrors.length} mirrors found — pick one
                </Text>
                {foundMirrors.map((m) => {
                  const active = url === m.url;
                  const label = m.name && m.name.length > 0 ? m.name : m.id;
                  return (
                    <Pressable
                      key={m.id}
                      onPress={() => pickMirror(m)}
                      style={[styles.pickerRow, active && styles.pickerRowActive]}
                    >
                      <Text style={styles.pickerRowLabel}>{label}</Text>
                      <Text style={styles.pickerRowSub}>
                        {m.ip}
                        {m.name && m.name.length > 0 ? ` • ${m.id}` : ''}
                      </Text>
                    </Pressable>
                  );
                })}
              </View>
            )}

            <View style={styles.row}>
              <Pressable
                onPress={connect}
                disabled={conn === 'open' || conn === 'connecting'}
                style={[styles.btn, (conn === 'open' || conn === 'connecting') && styles.btnDisabled]}
              >
                <Text style={styles.btnText}>Connect</Text>
              </Pressable>
              <View style={styles.gap} />
              <Pressable
                onPress={disconnect}
                disabled={conn === 'idle'}
                style={[styles.btn, conn === 'idle' && styles.btnDisabled]}
              >
                <Text style={styles.btnText}>Disconnect</Text>
              </Pressable>
              <View style={styles.gap} />
              <Text style={[styles.pill, pillStyle(conn)]}>{conn}</Text>
            </View>
          </>
        )}

        {inSetup && submitState === 'submitted' && (
          <View style={styles.setupBlock}>
            <Text style={styles.section}>Credentials sent</Text>
            <Text style={styles.body1}>
              The mirror is leaving its setup network and joining{' '}
              <Text style={styles.bold}>{ssid}</Text>.
            </Text>
            {secondsLeft > 0 ? (
              <>
                <Text style={styles.countdownNum}>{secondsLeft}s</Text>
                <Text style={styles.muted}>
                  If the credentials are wrong, the mirror returns to setup mode in {secondsLeft}s.
                </Text>
              </>
            ) : (
              <Text style={styles.muted}>
                If the mirror joined <Text style={styles.bold}>{ssid}</Text> successfully, it's
                reachable on that network now. Otherwise it's back in setup mode — re-join{' '}
                <Text style={styles.mono}>LL-Mirror-...</Text> and reconnect to retry.
              </Text>
            )}
            <Text style={styles.section}>Next steps</Text>
            <Text style={styles.body1}>
              1. Switch this phone's Wi-Fi from the mirror's setup network back to{' '}
              <Text style={styles.bold}>{ssid}</Text>.{'\n\n'}
              2. Tap <Text style={styles.bold}>Home</Text> above to set the URL to your home-network
              mirror IP, then tap <Text style={styles.bold}>Connect</Text>.
            </Text>
          </View>
        )}

        {inSetup && submitState !== 'submitted' && (
          <View style={styles.setupBlock}>
            <Text style={styles.section}>
              {provisioningSetup ? 'Mirror needs Wi-Fi setup' : 'Reconfigure Wi-Fi'}
            </Text>
            <Text style={styles.muted}>
              {provisioningSetup
                ? "You're connected to the mirror's SoftAP. Give it credentials for your home Wi-Fi to join, or tap below to skip and control it directly over the SoftAP."
                : `Mirror is currently on ${state?.wifi_ssid ?? '(unknown network)'}. Submit new credentials to switch it to a different network.`}
            </Text>

            {provisioningSetup ? (
              <Pressable
                onPress={() => setSetupBypassed(true)}
                style={[styles.btn, styles.btnSecondary]}
              >
                <Text style={styles.btnText}>Control directly (skip Wi-Fi setup)</Text>
              </Pressable>
            ) : (
              <Pressable
                onPress={() => { setReconfiguringWifi(false); setSsid(''); setPassword(''); }}
                style={[styles.btn, styles.btnSecondary]}
              >
                <Text style={styles.btnText}>Cancel</Text>
              </Pressable>
            )}

            <Text style={styles.label}>SSID</Text>
            <TextInput
              value={ssid}
              onChangeText={setSsid}
              autoCapitalize="none"
              autoCorrect={false}
              maxLength={32}
              placeholder="MyWiFi"
              placeholderTextColor="#555"
              style={styles.input}
            />

            <Text style={styles.label}>Password</Text>
            <View style={styles.row}>
              <TextInput
                value={password}
                onChangeText={setPassword}
                autoCapitalize="none"
                autoCorrect={false}
                secureTextEntry={!showPassword}
                maxLength={64}
                placeholder="(empty = open network)"
                placeholderTextColor="#555"
                style={[styles.input, { flex: 1 }]}
              />
              <View style={styles.gap} />
              <Pressable
                onPress={() => setShowPassword((v) => !v)}
                style={[styles.btn, styles.btnSecondary]}
              >
                <Text style={styles.btnText}>{showPassword ? 'Hide' : 'Show'}</Text>
              </Pressable>
            </View>
            {password.length > 0 && password.length < 8 && (
              <Text style={styles.err}>WPA2 passwords are at least 8 characters.</Text>
            )}

            <Pressable
              onPress={submitCreds}
              disabled={!formValid || submitState !== 'idle'}
              style={[styles.btn, (!formValid || submitState !== 'idle') && styles.btnDisabled]}
            >
              <Text style={styles.btnText}>
                {submitState === 'submitting' ? 'Connecting…' : 'Connect to Wi-Fi'}
              </Text>
            </Pressable>
          </View>
        )}

        {ready && !inSetup && route === 'controls' && state && (
          <>
            <Text style={styles.section}>Power</Text>
            <Pressable
              onPress={() => apply({ on: !state.on })}
              style={[styles.btn, state.on ? styles.btnOn : styles.btnOff]}
            >
              <Text style={styles.btnText}>{state.on ? 'On' : 'Off'}</Text>
            </Pressable>

            <Text style={styles.section}>Color</Text>
            <View style={styles.swatches}>
              {BRAND_SWATCHES.map((s) => {
                const active = state.base_color.toLowerCase() === s.hex.toLowerCase();
                return (
                  <Pressable
                    key={s.name}
                    onPress={() => apply({ base_color: s.hex })}
                    style={[styles.swatch, { backgroundColor: s.hex }, active && styles.swatchActive]}
                  />
                );
              })}
            </View>
            <Text style={styles.mono}>{state.base_color}</Text>

            <Text style={styles.section}>Brightness</Text>
            <View style={styles.row}>
              {BRIGHTNESS_STEPS.map((b) => {
                const active = state.brightness === b;
                return (
                  <Pressable
                    key={b}
                    onPress={() => apply({ brightness: b })}
                    style={[styles.stepBtn, active && styles.stepBtnActive]}
                  >
                    <Text style={[styles.btnText, active && styles.stepBtnTextActive]}>{b}</Text>
                  </Pressable>
                );
              })}
            </View>

            <Text style={styles.section}>Pattern</Text>
            <View style={styles.patterns}>
              {PATTERNS.map((p) => {
                const active = state.pattern_id === p;
                return (
                  <Pressable
                    key={p}
                    onPress={() => apply({ pattern_id: p })}
                    style={[styles.patternBtn, active && styles.patternBtnActive]}
                  >
                    <Text style={[styles.btnText, active && styles.stepBtnTextActive]}>{p}</Text>
                  </Pressable>
                );
              })}
            </View>
          </>
        )}

        {ready && !inSetup && route === 'settings' && state && (
          <>
            <Text style={styles.subsection}>Mirror name</Text>
            <Text style={styles.muted}>
              Hardware ID: <Text style={styles.mono}>{state.id ?? '(unknown)'}</Text>
              {'\n'}
              Shown in Find-mirror when you have multiple on one network. Leave empty to fall back to the hardware ID.
            </Text>
            <TextInput
              value={nameInput}
              onChangeText={setNameInput}
              autoCapitalize="words"
              autoCorrect={false}
              maxLength={32}
              placeholder="Living Room"
              placeholderTextColor="#555"
              style={styles.input}
            />
            <Pressable
              onPress={submitRename}
              disabled={renameState !== 'idle' || nameInput === (state.name ?? '')}
              style={[
                styles.btn,
                (renameState !== 'idle' || nameInput === (state.name ?? '')) &&
                  styles.btnDisabled,
              ]}
            >
              <Text style={styles.btnText}>
                {renameState === 'idle' && 'Save name'}
                {renameState === 'submitting' && 'Saving…'}
                {renameState === 'submitted' && 'Saved ✓'}
              </Text>
            </Pressable>

            <Text style={styles.subsection}>Wi-Fi</Text>
            <Text style={styles.muted}>
              Connected: <Text style={styles.bold}>{state.wifi_ssid ?? '(not on a network)'}</Text>
              {'\n'}
              IP: <Text style={styles.mono}>{state.ip ?? '(none)'}</Text>
            </Text>

            <Text style={styles.label}>Saved networks</Text>
            {switchingTo !== null && (
              <View style={styles.switchingBanner}>
                <Text style={styles.switchingText}>
                  Switching to {switchingTo}… mirror may go briefly
                  offline. App will reconnect when it’s back.
                </Text>
              </View>
            )}
            {savedNetworks === null && (
              <Text style={styles.muted}>Loading…</Text>
            )}
            {savedNetworks !== null && savedNetworks.length === 0 && (
              <Text style={styles.muted}>No saved networks yet.</Text>
            )}
            {savedNetworks?.map((n) => (
              <View key={n.ssid} style={styles.netRow}>
                <Text style={styles.netCheck}>
                  {n.is_active ? '✓' : '  '}
                </Text>
                <Text style={[styles.netSsid, n.is_active && styles.bold]}>
                  {n.ssid}
                </Text>
                {!n.is_active && (
                  <Pressable
                    onPress={() => connectToNetwork(n)}
                    style={[styles.btn, styles.btnSecondary, styles.netForgetBtn]}
                  >
                    <Text style={styles.btnText}>Connect</Text>
                  </Pressable>
                )}
                <Pressable
                  onPress={() => forgetNetwork(n)}
                  style={[styles.btn, styles.btnSecondary, styles.netForgetBtn]}
                  // Tap target stays usable but the action is destructive
                  // for the active entry — the confirm dialog inside
                  // forgetNetwork is what actually gates the disconnect.
                >
                  <Text style={styles.btnText}>Forget</Text>
                </Pressable>
              </View>
            ))}

            {!addingNetwork && savedNetworks !== null && (
              <Pressable
                onPress={() => {
                  setAddSsid('');
                  setAddPassword('');
                  setAddNetState('idle');
                  setAddingNetwork(true);
                }}
                style={[styles.btn, styles.btnSecondary]}
              >
                <Text style={styles.btnText}>+ Add a network</Text>
              </Pressable>
            )}

            {addingNetwork && (
              <View style={styles.addNetForm}>
                <Text style={styles.label}>SSID</Text>
                <TextInput
                  value={addSsid}
                  onChangeText={setAddSsid}
                  autoCapitalize="none"
                  autoCorrect={false}
                  style={styles.input}
                  placeholder="HomeWiFi"
                  placeholderTextColor="#555"
                />
                <Text style={styles.label}>Password</Text>
                <TextInput
                  value={addPassword}
                  onChangeText={setAddPassword}
                  autoCapitalize="none"
                  autoCorrect={false}
                  secureTextEntry={!showPassword}
                  style={styles.input}
                  placeholder="(empty for open networks)"
                  placeholderTextColor="#555"
                />
                <View style={styles.row}>
                  <Pressable
                    onPress={() => setShowPassword((v) => !v)}
                    style={[styles.btn, styles.btnSecondary]}
                  >
                    <Text style={styles.btnText}>
                      {showPassword ? 'Hide password' : 'Show password'}
                    </Text>
                  </Pressable>
                  <Pressable
                    onPress={() => {
                      setAddingNetwork(false);
                      setAddSsid('');
                      setAddPassword('');
                    }}
                    style={[styles.btn, styles.btnSecondary]}
                  >
                    <Text style={styles.btnText}>Cancel</Text>
                  </Pressable>
                  <Pressable
                    onPress={submitAddNetwork}
                    disabled={addNetState !== 'idle'}
                    style={[
                      styles.btn,
                      addNetState !== 'idle' && styles.btnDisabled,
                    ]}
                  >
                    <Text style={styles.btnText}>
                      {addNetState === 'idle' && 'Save'}
                      {addNetState === 'submitting' && 'Saving…'}
                      {addNetState === 'submitted' && 'Saved ✓'}
                    </Text>
                  </Pressable>
                </View>
                <Text style={styles.muted}>
                  Adding a network saves it for later — the mirror won’t
                  switch to it until it’s in range and the current network
                  isn’t.
                </Text>
              </View>
            )}

            <Text style={styles.subsection}>Firmware update</Text>
            <Text style={styles.label}>OTA binary URL</Text>
            <TextInput
              value={otaUrl}
              onChangeText={setOtaUrl}
              autoCapitalize="none"
              autoCorrect={false}
              style={styles.input}
            />
            <Pressable
              onPress={startOta}
              disabled={otaState !== 'idle'}
              style={[styles.btn, otaState !== 'idle' && styles.btnDisabled]}
            >
              <Text style={styles.btnText}>
                {otaState === 'idle' && 'Update firmware'}
                {otaState === 'sending' && 'Sending request…'}
                {otaState === 'rebooting' && 'Mirror downloading + rebooting…'}
              </Text>
            </Pressable>

            <Text style={styles.subsection}>Danger</Text>
            <Pressable onPress={factoryReset} style={[styles.btn, styles.btnDanger]}>
              <Text style={styles.btnText}>Factory reset</Text>
            </Pressable>
          </>
        )}

        {lastError && <Text style={styles.err}>{lastError}</Text>}
      </ScrollView>
    </SafeAreaView>
  );
}

function pillStyle(c: ConnState) {
  switch (c) {
    case 'open': return { backgroundColor: '#2c7' };
    case 'connecting': return { backgroundColor: '#cc7' };
    case 'closed': return { backgroundColor: '#c44' };
    default: return { backgroundColor: '#999' };
  }
}

const styles = StyleSheet.create({
  root: { flex: 1, backgroundColor: '#0B0A0F' },
  body: { padding: 16, gap: 8, paddingBottom: 64, paddingTop: 16 },
  headerBar: {
    paddingVertical: 4,
    marginBottom: 4,
    flexDirection: 'row',
    alignItems: 'center',
    justifyContent: 'space-between',
  },
  navBtn: { paddingVertical: 6, paddingHorizontal: 10 },
  navBtnText: { color: '#F4EFE6', fontSize: 22 },
  h1: { color: '#F4EFE6', fontSize: 22, fontWeight: '600' },
  label: { color: '#8A8A8E', fontSize: 12, marginTop: 12 },
  section: { color: '#F4EFE6', fontSize: 16, marginTop: 20, marginBottom: 6 },
  subsection: { color: '#F4EFE6', fontSize: 14, fontWeight: '600', marginTop: 16, marginBottom: 4 },
  input: {
    backgroundColor: '#1a1924',
    color: '#F4EFE6',
    padding: 10,
    borderRadius: 6,
    fontFamily: 'monospace',
  },
  row: { flexDirection: 'row', alignItems: 'center', marginTop: 8, gap: 8, flexWrap: 'wrap' },
  gap: { width: 12 },
  btn: {
    backgroundColor: '#3214FF',
    paddingVertical: 12,
    paddingHorizontal: 18,
    borderRadius: 8,
    alignSelf: 'flex-start',
  },
  btnText: { color: '#F4EFE6', fontWeight: '600' },
  btnDisabled: { backgroundColor: '#333' },
  btnOn: { backgroundColor: '#3214FF' },
  btnOff: { backgroundColor: '#444' },
  btnDanger: { backgroundColor: '#992020' },
  btnSecondary: { backgroundColor: '#1a1924' },
  switchingBanner: {
    backgroundColor: '#1a1924',
    borderLeftColor: '#3214FF',
    borderLeftWidth: 3,
    paddingVertical: 8,
    paddingHorizontal: 10,
    borderRadius: 4,
    marginTop: 4,
  },
  switchingText: { color: '#F4EFE6', fontSize: 13, lineHeight: 18 },
  netRow: {
    flexDirection: 'row',
    alignItems: 'center',
    paddingVertical: 6,
    paddingHorizontal: 8,
    backgroundColor: '#15141d',
    borderRadius: 6,
    marginTop: 4,
    gap: 8,
  },
  netCheck: {
    color: '#3214FF',
    fontSize: 16,
    width: 16,
    textAlign: 'center',
  },
  netSsid: { color: '#F4EFE6', fontSize: 14, flex: 1 },
  netForgetBtn: { paddingVertical: 6, paddingHorizontal: 10 },
  addNetForm: { marginTop: 8, gap: 4 },
  pill: {
    color: '#0B0A0F',
    paddingVertical: 4,
    paddingHorizontal: 10,
    borderRadius: 12,
    fontWeight: '600',
  },
  urlPreset: {
    backgroundColor: '#1a1924',
    paddingVertical: 8,
    paddingHorizontal: 14,
    borderRadius: 6,
    borderWidth: 1,
    borderColor: 'transparent',
  },
  urlPresetActive: { borderColor: '#3214FF' },
  urlPresetDisabled: { opacity: 0.5 },
  urlPresetText: { color: '#F4EFE6', fontSize: 13 },
  pickerBlock: { marginTop: 8, gap: 6 },
  pickerRow: {
    backgroundColor: '#1a1924',
    paddingVertical: 10,
    paddingHorizontal: 14,
    borderRadius: 6,
    borderWidth: 1,
    borderColor: 'transparent',
  },
  pickerRowActive: { borderColor: '#3214FF' },
  pickerRowLabel: { color: '#F4EFE6', fontSize: 14, fontWeight: '600' },
  pickerRowSub: { color: '#8A8A8E', fontSize: 12, fontFamily: 'monospace', marginTop: 2 },
  swatches: { flexDirection: 'row', flexWrap: 'wrap', gap: 10 },
  swatch: {
    width: 56,
    height: 56,
    borderRadius: 28,
    borderWidth: 2,
    borderColor: 'transparent',
  },
  swatchActive: { borderColor: '#F4EFE6' },
  stepBtn: {
    backgroundColor: '#1a1924',
    paddingVertical: 12,
    paddingHorizontal: 18,
    borderRadius: 8,
    minWidth: 64,
    alignItems: 'center',
  },
  stepBtnActive: { backgroundColor: '#3214FF' },
  stepBtnTextActive: { color: '#fff' },
  patterns: { flexDirection: 'row', flexWrap: 'wrap', gap: 8 },
  patternBtn: {
    backgroundColor: '#1a1924',
    paddingVertical: 10,
    paddingHorizontal: 14,
    borderRadius: 8,
  },
  patternBtnActive: { backgroundColor: '#3214FF' },
  setupBlock: { gap: 8 },
  body1: { color: '#F4EFE6', lineHeight: 20 },
  bold: { fontWeight: '700' },
  muted: { color: '#8A8A8E', lineHeight: 20 },
  countdownNum: { color: '#3214FF', fontSize: 48, fontWeight: '700' },
  mono: { color: '#8A8A8E', fontFamily: 'monospace', marginTop: 4 },
  err: { color: '#f88', marginTop: 12 },
});

export default App;
