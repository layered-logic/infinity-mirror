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
  View,
} from 'react-native';

// SafeAreaView from react-native core only applies iOS notch insets — on
// Android it's a no-op, so the title overlaps the status bar. Manually
// add padding for the status bar height on Android.
const ANDROID_STATUS_BAR_PADDING =
  Platform.OS === 'android' ? StatusBar.currentHeight ?? 0 : 0;

import { ConnState, MirrorClient } from './src/ws-client';
import { BRAND_SWATCHES, DeviceState, PatternId } from './src/protocol';
import { findMirror } from './src/find-mirror';

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

  const findOnSubnet = async () => {
    setScanning(true);
    setLastError(null);
    try {
      const r = await findMirror();
      setUrl(r.url);
    } catch (e) {
      setLastError((e as Error).message);
    } finally {
      setScanning(false);
    }
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
      <ScrollView contentContainerStyle={styles.body}>
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
            <Text style={styles.subsection}>Wi-Fi</Text>
            <Text style={styles.muted}>
              Network: <Text style={styles.bold}>{state.wifi_ssid ?? '(not on a network)'}</Text>
              {'\n'}
              Mode: {state.provisioning_active ? 'setup mode (mirror is its own Wi-Fi)' : (state.wifi_ssid ? 'connected to your Wi-Fi' : 'unknown')}
            </Text>
            <Pressable
              onPress={() => { setSsid(''); setPassword(''); setReconfiguringWifi(true); }}
              style={[styles.btn, styles.btnSecondary]}
            >
              <Text style={styles.btnText}>Reconfigure Wi-Fi</Text>
            </Pressable>

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
  body: { padding: 16, gap: 8, paddingBottom: 64, paddingTop: 16 + ANDROID_STATUS_BAR_PADDING },
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
