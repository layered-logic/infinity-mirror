import asyncio, json, time, websockets

MIRROR = 'ws://192.168.5.229/ws'

async def drain(ws, secs=0.3):
    end = time.monotonic() + secs
    while time.monotonic() < end:
        try:
            await asyncio.wait_for(ws.recv(), end - time.monotonic())
        except asyncio.TimeoutError:
            break

async def collect(ws, window_s):
    end = time.monotonic() + window_s
    frames = []
    t0 = time.monotonic()
    while time.monotonic() < end:
        try:
            raw = await asyncio.wait_for(ws.recv(), end - time.monotonic())
            msg = json.loads(raw)
            frames.append((round(time.monotonic() - t0, 3), msg))
        except asyncio.TimeoutError:
            break
    return frames

def show(frames):
    for t, m in frames:
        op = m.get('op', '?')
        ok = m.get('ok', '-')
        rid = m.get('req_id', '-')
        print(f'  t={t:6.3f}s  op={op:<22} ok={str(ok):<5} req_id={rid}')

async def main():
    async with websockets.connect(MIRROR, ping_interval=None) as ws:
        await ws.send(json.dumps({'op':'ping','req_id':'p','ts':0}))
        r = json.loads(await asyncio.wait_for(ws.recv(), 5))
        print('PING:', r.get('result'))
        await drain(ws, 0.5)

        print()
        print('=== TEST A: single set_state {brightness:60} — 2s capture ===')
        await ws.send(json.dumps({'op':'set_state','req_id':'a1','ts':0,
                                  'payload':{'brightness':60}}))
        show(await collect(ws, 2.0))
        await drain(ws, 0.5)

        print()
        print('=== TEST B: multi-field set_state {on, brightness, base_color} — 2s capture ===')
        await ws.send(json.dumps({'op':'set_state','req_id':'b1','ts':0,
                                  'payload':{'on':True,'brightness':40,'base_color':'#3214FF'}}))
        show(await collect(ws, 2.0))
        await drain(ws, 0.5)

        print()
        print('=== TEST C: 6 rapid color sets ~30ms apart ===')
        colors = ['#FF0000','#FF7F00','#FFFF00','#00FF00','#0000FF','#7F00FF']
        t0 = time.monotonic()
        for i, c in enumerate(colors):
            await ws.send(json.dumps({'op':'set_state','req_id':f'c{i}','ts':0,
                                      'payload':{'base_color':c}}))
            await asyncio.sleep(0.030)
        # Capture from t0 for 2.5s total
        end = t0 + 2.5
        frames = []
        while time.monotonic() < end:
            try:
                raw = await asyncio.wait_for(ws.recv(), end - time.monotonic())
                msg = json.loads(raw)
                frames.append((round(time.monotonic() - t0, 3), msg))
            except asyncio.TimeoutError:
                break
        show(frames)

asyncio.run(main())
