"""Trigger an OTA and capture the streaming ota_progress events on the
same WS socket. Verifies LL-057 Session B end-to-end.

Pass criteria: ≥3 progress events received before the socket closes;
first phase is 'downloading'; final pre-close phase is 'rebooting' (or
'failed' if anything went wrong).
"""
import asyncio, json, sys, time, websockets

MIRROR = 'ws://192.168.5.229/ws'
OTA_URL = 'http://192.168.1.98:8000/layered_logic_mirror_standard.bin'

async def main():
    ws = await websockets.connect(MIRROR, ping_interval=None, close_timeout=5)
    env = {'op': 'start_ota', 'req_id': 'otaWrap', 'ts': 0, 'payload': {'url': OTA_URL}}
    await ws.send(json.dumps(env))
    t0 = time.monotonic()
    events = []
    response = None
    try:
        while True:
            raw = await asyncio.wait_for(ws.recv(), 60)
            msg = json.loads(raw)
            dt = time.monotonic() - t0
            op = msg.get('op')
            if op == 'ota_progress':
                phase = msg['phase']
                percent = msg['percent']
                print(f'  [t={dt:5.2f}s] ota_progress  phase={phase:<11}  percent={percent}')
                events.append((dt, percent, phase))
            elif op == 'start_ota':
                print(f'  [t={dt:5.2f}s] start_ota response  ok={msg.get("ok")}  '
                      f'error={msg.get("error")}')
                response = msg
            else:
                print(f'  [t={dt:5.2f}s] other op={op}')
    except asyncio.TimeoutError:
        print(f'  [t={time.monotonic()-t0:5.2f}s] no message for 60s (probably rebooted)')
    except websockets.ConnectionClosed as e:
        print(f'  [t={time.monotonic()-t0:5.2f}s] socket closed code={e.code}')

    print('\n--- summary ---')
    print(f'total events: {len(events)}')
    if events:
        print(f'first: t={events[0][0]:.2f}s phase={events[0][2]} percent={events[0][1]}')
        print(f'last:  t={events[-1][0]:.2f}s phase={events[-1][2]} percent={events[-1][1]}')
        phases_seen = sorted(set(p for _, _, p in events))
        print(f'phases observed: {phases_seen}')
    if response is not None and not response.get('ok'):
        print(f'OTA failed at firmware: {response.get("error")}')

asyncio.run(main())
