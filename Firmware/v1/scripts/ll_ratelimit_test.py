"""Per-IP rate-limit verification for LL-055 Session C (spec §7.1).

Three scenarios:
  1. Steady 10 msg/s for 3 seconds — should run clean (right at the limit).
  2. Flood at 50 msg/s — burst of 10 succeeds, then close 1008 from firmware.
  3. After-close recovery — wait 2s, reconnect, send 1 msg, should be fine
     (bucket should have refilled).

Reported per scenario: number of responses received, the close code (if any),
and time-to-close from first send.
"""
import asyncio, json, time, websockets

MIRROR = 'ws://192.168.5.229/ws'

async def scenario_steady():
    print('=== Scenario 1: steady 10 msg/s for 3s ===')
    t0 = time.monotonic()
    responses = 0
    close_code = None
    try:
        async with websockets.connect(MIRROR, ping_interval=None, close_timeout=2) as ws:
            async def reader():
                nonlocal responses
                while True:
                    raw = await ws.recv()
                    msg = json.loads(raw)
                    if msg.get('req_id', '').startswith('s'):
                        responses += 1
            reader_task = asyncio.create_task(reader())
            try:
                for i in range(30):  # 30 messages over 3s = 10 msg/s
                    await ws.send(json.dumps({'op':'set_state','req_id':f's{i}','ts':0,
                                              'payload':{'brightness': 30 + (i % 10)}}))
                    await asyncio.sleep(0.100)
                await asyncio.sleep(0.5)  # drain remaining responses
            finally:
                reader_task.cancel()
    except websockets.ConnectionClosed as e:
        close_code = e.code
    dt = time.monotonic() - t0
    print(f'  sent=30 responses={responses} close_code={close_code} elapsed={dt:.2f}s')

async def scenario_flood():
    print('\n=== Scenario 2: flood at 50 msg/s ===')
    t0 = time.monotonic()
    sent = 0
    responses = 0
    close_code = None
    close_at = None
    try:
        async with websockets.connect(MIRROR, ping_interval=None, close_timeout=2) as ws:
            async def reader():
                nonlocal responses
                while True:
                    raw = await ws.recv()
                    msg = json.loads(raw)
                    if msg.get('req_id', '').startswith('f'):
                        responses += 1
            reader_task = asyncio.create_task(reader())
            try:
                for i in range(50):
                    await ws.send(json.dumps({'op':'set_state','req_id':f'f{i}','ts':0,
                                              'payload':{'brightness': 30 + (i % 10)}}))
                    sent += 1
                    await asyncio.sleep(0.020)  # 50 msg/s
                await asyncio.sleep(0.5)
            finally:
                reader_task.cancel()
    except websockets.ConnectionClosed as e:
        close_code = e.code
        close_at = time.monotonic() - t0
    dt = time.monotonic() - t0
    print(f'  sent={sent} responses={responses} close_code={close_code} '
          f'close_at={close_at and f"{close_at:.2f}s"} elapsed={dt:.2f}s')

async def scenario_recovery():
    print('\n=== Scenario 3: after-close recovery (sleep 2s, reconnect, 1 msg) ===')
    await asyncio.sleep(2.0)
    try:
        async with websockets.connect(MIRROR, ping_interval=None, close_timeout=2) as ws:
            await ws.send(json.dumps({'op':'ping','req_id':'r1','ts':0}))
            r = json.loads(await asyncio.wait_for(ws.recv(), 3))
            print(f'  ping OK: fw_version={r.get("result",{}).get("fw_version")}')
    except Exception as e:
        print(f'  ERROR: {e!r}')

async def main():
    await scenario_steady()
    await scenario_flood()
    await scenario_recovery()

asyncio.run(main())
