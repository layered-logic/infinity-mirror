"""End-to-end test through ll_ws_proxy. Configurable target via TARGET env
var (default 127.0.0.1:9000). Runs the same shape as ll_debounce_test.py
+ a basic single-client load run, so we can compare RTT and broadcast
count vs the direct-to-mirror runs.
"""
import asyncio, json, os, statistics, time, websockets

TARGET = os.environ.get('LL_TARGET', 'ws://127.0.0.1:9000/ws')

async def drain(ws, secs=0.5):
    end = time.monotonic() + secs
    while time.monotonic() < end:
        try:
            await asyncio.wait_for(ws.recv(), end - time.monotonic())
        except asyncio.TimeoutError:
            break

async def main():
    print(f'TARGET = {TARGET}')
    async with websockets.connect(TARGET, ping_interval=None, close_timeout=5) as ws:
        await ws.send(json.dumps({'op':'ping','req_id':'p','ts':0}))
        r = json.loads(await asyncio.wait_for(ws.recv(), 8))
        print(f'PING ok  fw_version={r.get("result",{}).get("fw_version")}  '
              f'uptime_s={r.get("result",{}).get("uptime_s")}')
        await drain(ws, 0.5)

        print('\n=== single set_state probes (10 iter, 200ms spacing — 5 msg/s, under rate-limit) ===')
        rtts = []
        for i in range(10):
            t0 = time.monotonic()
            await ws.send(json.dumps({'op':'set_state','req_id':f'j{i}','ts':0,
                                      'payload':{'brightness': 30 + i}}))
            ack = None
            deadline = t0 + 3.0
            while time.monotonic() < deadline:
                try:
                    raw = await asyncio.wait_for(ws.recv(), deadline - time.monotonic())
                    msg = json.loads(raw)
                    if msg.get('req_id') == f'j{i}':
                        ack = time.monotonic() - t0
                        break
                except asyncio.TimeoutError:
                    break
            if ack is not None:
                rtts.append(ack * 1000)
            await asyncio.sleep(0.2)
        valid = [r for r in rtts if r == r]
        if valid:
            print(f'  n={len(valid)}/10  min={min(valid):.1f}ms  median={statistics.median(valid):.1f}ms  '
                  f'p90={sorted(valid)[int(len(valid)*0.9)-1]:.1f}ms  max={max(valid):.1f}ms')
        else:
            print('  no responses received')

asyncio.run(main())
