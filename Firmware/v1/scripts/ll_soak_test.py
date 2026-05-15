"""5-minute soak test for LL-055 Session D.

Sustained 5 msg/s of set_state through one WS client. Reports
uptime_s every minute via a parallel ping channel — a reboot would
show as uptime resetting. Tracks RTT samples + any errors.

Pass criteria: 5-minute run completes, uptime monotonically increases
(no reboots), RTT distribution stays stable across the run (no
runaway memory/queue growth), no rate-limit closes.
"""
import asyncio, json, time, statistics, websockets

MIRROR = 'ws://192.168.5.229/ws'
DURATION_S = 300       # 5 minutes
SEND_PERIOD_S = 0.2    # 5 msg/s — comfortably under the 10/s rate-limit
PING_PERIOD_S = 60     # uptime check every minute

async def pinger(ws_url, stop_evt, uptimes):
    while not stop_evt.is_set():
        try:
            async with websockets.connect(ws_url, ping_interval=None, close_timeout=2) as ws:
                await ws.send(json.dumps({'op':'ping','req_id':'pp','ts':0}))
                r = json.loads(await asyncio.wait_for(ws.recv(), 5))
                up = r.get('result',{}).get('uptime_s')
                fw = r.get('result',{}).get('fw_version')
                uptimes.append((time.monotonic(), up, fw))
                print(f'  [{int(time.monotonic())}s] uptime={up}s fw={fw}', flush=True)
        except Exception as e:
            print(f'  [{int(time.monotonic())}s] ping FAIL: {e!r}', flush=True)
        try:
            await asyncio.wait_for(stop_evt.wait(), PING_PERIOD_S)
        except asyncio.TimeoutError:
            pass

async def main():
    print(f'soak: duration={DURATION_S}s  send_period={SEND_PERIOD_S}s  ping_period={PING_PERIOD_S}s')
    stop = asyncio.Event()
    uptimes = []
    pinger_task = asyncio.create_task(pinger(MIRROR, stop, uptimes))

    rtts = []
    errors = 0
    rate_closes = 0
    t_end = time.monotonic() + DURATION_S
    iter_count = 0
    try:
        async with websockets.connect(MIRROR, ping_interval=None, close_timeout=5) as ws:
            while time.monotonic() < t_end:
                t0 = time.monotonic()
                try:
                    await ws.send(json.dumps({'op':'set_state','req_id':f'sk{iter_count}','ts':0,
                                              'payload':{'brightness': 30 + (iter_count % 30)}}))
                    deadline = t0 + 2.0
                    while time.monotonic() < deadline:
                        try:
                            raw = await asyncio.wait_for(ws.recv(), deadline - time.monotonic())
                            msg = json.loads(raw)
                            if msg.get('req_id') == f'sk{iter_count}':
                                rtts.append((time.monotonic() - t0) * 1000)
                                break
                        except asyncio.TimeoutError:
                            errors += 1
                            break
                except websockets.ConnectionClosed as e:
                    if e.code == 1008:
                        rate_closes += 1
                    print(f'  send-loop: connection closed code={e.code}', flush=True)
                    break
                iter_count += 1
                # pace
                sleep_left = SEND_PERIOD_S - (time.monotonic() - t0)
                if sleep_left > 0:
                    await asyncio.sleep(sleep_left)
    finally:
        stop.set()
        try:
            await asyncio.wait_for(pinger_task, 5.0)
        except asyncio.TimeoutError:
            pinger_task.cancel()

    print(f'\nsoak DONE  iter={iter_count}  rtts={len(rtts)}  errors={errors}  rate_closes={rate_closes}')
    if rtts:
        h = len(rtts) // 2
        first_half = rtts[:h]
        second_half = rtts[h:]
        print(f'  overall RTT: min={min(rtts):.1f}ms  median={statistics.median(rtts):.1f}ms  '
              f'p90={sorted(rtts)[int(len(rtts)*0.9)-1]:.1f}ms  max={max(rtts):.1f}ms')
        print(f'  first-half median={statistics.median(first_half):.1f}ms  '
              f'second-half median={statistics.median(second_half):.1f}ms  '
              f'(drift = {statistics.median(second_half)-statistics.median(first_half):+.1f}ms)')
    if uptimes:
        first_up = uptimes[0][1]
        last_up = uptimes[-1][1]
        monotonic = all(uptimes[i][1] <= uptimes[i+1][1] for i in range(len(uptimes)-1))
        print(f'  uptime first={first_up}s last={last_up}s monotonic={monotonic} '
              f'(delta={last_up-first_up}s vs run duration ~{int(uptimes[-1][0]-uptimes[0][0])}s)')

asyncio.run(main())
