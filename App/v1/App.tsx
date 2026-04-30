import { useEffect, useRef, useState } from 'react';
import {
  Alert,
  Pressable,
  SafeAreaView,
  ScrollView,
  StatusBar,
  StyleSheet,
  Text,
  TextInput,
  View,
} from 'react-native';

import { ConnState, MirrorClient } from './src/ws-client';
import { DeviceState, PatternId } from './src/protocol';

const DEFAULT_URL = 'ws://10.123.210.61/ws';

const SWATCHES: { name: string; hex: string }[] = [
  { name: 'red',     hex: '#FF1A1A' },
  { name: 'orange',  hex: '#FF7A00' },
  { name: 'yellow',  hex: '#FFD400' },
  { name: 'green',   hex: '#22DD55' },
  { name: 'cyan',    hex: '#22DDDD' },
  { name: 'blue',    hex: '#1A55FF' },
  { name: 'indigo',  hex: '#3214FF' },
  { name: 'magenta', hex: '#DD22DD' },
  { name: 'white',   hex: '#FFFFFF' },
];

const BRIGHTNESS_STEPS = [25, 50, 75, 100];

const PATTERNS: PatternId[] = [
  'solid', 'rainbow', 'scanner', 'spinner', 'random', 'breathing', 'twinkle',
];

function App() {
  const [url, setUrl] = useState(DEFAULT_URL);
  const [conn, setConn] = useState<ConnState>('idle');
  const [lastError, setLastError] = useState<string | null>(null);
  const [state, setState] = useState<DeviceState | null>(null);
  const clientRef = useRef<MirrorClient | null>(null);

  useEffect(() => {
    return () => clientRef.current?.disconnect();
  }, []);

  // After the socket opens, fetch initial state so the controls reflect
  // current values without waiting for a user-driven broadcast.
  useEffect(() => {
    if (conn !== 'open') return;
    clientRef.current?.getState().then(setState).catch((e) => setLastError((e as Error).message));
  }, [conn]);

  const connect = () => {
    clientRef.current?.disconnect();
    setLastError(null);
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

  return (
    <SafeAreaView style={styles.root}>
      <StatusBar barStyle="light-content" backgroundColor="#0B0A0F" />
      <ScrollView contentContainerStyle={styles.body}>
        <Text style={styles.h1}>LL Mirror</Text>

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

        {ready && state && (
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
              {SWATCHES.map((s) => {
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

            <Text style={styles.section}>Danger</Text>
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
  body: { padding: 16, gap: 8, paddingBottom: 64 },
  h1: { color: '#F4EFE6', fontSize: 26, marginBottom: 8 },
  label: { color: '#8A8A8E', fontSize: 12, marginTop: 12 },
  section: { color: '#F4EFE6', fontSize: 16, marginTop: 20, marginBottom: 6 },
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
  pill: {
    color: '#0B0A0F',
    paddingVertical: 4,
    paddingHorizontal: 10,
    borderRadius: 12,
    fontWeight: '600',
  },
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
  mono: { color: '#8A8A8E', fontFamily: 'monospace', marginTop: 4 },
  err: { color: '#f88', marginTop: 12 },
});

export default App;
