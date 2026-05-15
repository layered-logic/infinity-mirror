"""Reconnect-hammer test for LL-055 Session D.

30x WS open → ping → close cycle in rapid succession. Tests:
  - WS handshake latency stays bounded across iterations (no fd-leak slowdown).
  - Rate-limit table doesn't accumulate stale entries (each ping consumes
    1 token; with 30 pings spread over a few seconds, we burst-eat tokens
    fast, but each open is a fresh TCP handshake from the same IP so the
    bucket should refill at 10/s and never hit the 1008 close).
  - Firmware uptime stays consistent (no reboot from hammering connects).

Pass criteria: 30/30 successful ping round-trips, no rate-limit closes,
uptime monotonically increases across all 30 probes.
"""
import asyncio, json, time, statistics, websockets

MIRROR = 'ws://192.168.5.229/ws'
N = 30

async def main():
    samples = []
    uptimes = []
    failures = []
    t_start = time.monotonic()
    for i in range(N):
        t0 = time.monotonic()
        try:
            async with websockets.connect(MIRROR, ping_interval=None, close_timeout=2) as ws:
                await ws.send(json.dumps({'op':'ping','req_id':f'h{i}','ts':0}))
                raw = await asyncio.wait_for(ws.recv(), 3)
                msg = json.loads(raw)
                up = msg.get('result',{}).get('uptime_s')
                rtt = (time.monotonic() - t0) * 1000
                samples.append(rtt)
                uptimes.append(up)
        except Exception as e:
            failures.append((i, repr(e)))
        await asyncio.sleep(0.15)  # 6.6 iter/sec → respects rate-limit
    dt = time.monotonic() - t_start

    print(f'N={N}  elapsed={dt:.2f}s  successes={len(samples)}/{N}  failures={len(failures)}')
    if samples:
        print(f'  open+ping RTT: min={min(samples):.1f}ms  median={statistics.median(samples):.1f}ms  '
              f'p90={sorted(samples)[int(len(samples)*0.9)-1]:.1f}ms  max={max(samples):.1f}ms')
    if uptimes:
        print(f'  uptime first={uptimes[0]}s last={uptimes[-1]}s '
              f'monotonic={all(uptimes[i] <= uptimes[i+1] for i in range(len(uptimes)-1))}')
    for i, e in failures[:5]:
        print(f'  fail #{i}: {e}')

asyncio.run(main())
