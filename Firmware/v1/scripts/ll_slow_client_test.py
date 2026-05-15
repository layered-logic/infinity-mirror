"""Inbound-responsiveness-under-slow-client test for LL-055 Session B.

One "slow" WS client opens a connection, sends a ping to register itself
with the httpd WS framework, then stops reading. The firmware's broadcast
fanout will hit that socket and block (or queue up) on send to that fd.

Meanwhile a "fast" client measures set_state -> response latency. If the
new broadcast task isolates the slow-fanout from inbound dispatch, the
fast client's RTT stays low regardless of how clogged the slow socket is.

Pre-fix (broadcast on esp_timer task, sends inline in the loop): a slow
client's blocking send would tie up the timer task; the timer task is
shared with other timers across the firmware, and esp_http_server's
internal work-queue can saturate while broadcasts are mid-flight.

Post-fix (broadcast on dedicated FreeRTOS task, queue depth 2): slow
sends block only that task; httpd's inbound dispatch is untouched.
"""
import asyncio, json, time, statistics, websockets

MIRROR = 'ws://192.168.5.229/ws'
N_PROBES = 20

async def slow_client(ready_evt, stop_evt):
    async with websockets.connect(MIRROR, ping_interval=None) as ws:
        # Register as a WS client by completing one round-trip.
        await ws.send(json.dumps({'op':'ping','req_id':'slow-init','ts':0}))
        await ws.recv()
        ready_evt.set()
        # Sit here, ignoring inbound frames. Firmware's broadcast loop
        # will hit this fd and back up against the unread buffer.
        await stop_evt.wait()

async def fast_probe(label):
    samples = []
    async with websockets.connect(MIRROR, ping_interval=None) as ws:
        # Warm-up: one set_state + drain.
        await ws.send(json.dumps({'op':'set_state','req_id':'warm','ts':0,
                                  'payload':{'brightness':50}}))
        end = time.monotonic() + 0.4
        while time.monotonic() < end:
            try:
                await asyncio.wait_for(ws.recv(), end - time.monotonic())
            except asyncio.TimeoutError:
                break

        for i in range(N_PROBES):
            t0 = time.monotonic()
            await ws.send(json.dumps({'op':'set_state','req_id':f'p{i}','ts':0,
                                      'payload':{'brightness': 40 + (i % 20)}}))
            # First frame back from this socket should be the set_state response
            # (req_id-matched). Broadcasts may arrive too — filter to the response.
            ack_t = None
            deadline = t0 + 1.5
            while time.monotonic() < deadline:
                try:
                    raw = await asyncio.wait_for(ws.recv(), deadline - time.monotonic())
                    msg = json.loads(raw)
                    if msg.get('req_id') == f'p{i}':
                        ack_t = time.monotonic() - t0
                        break
                except asyncio.TimeoutError:
                    break
            if ack_t is not None:
                samples.append(ack_t * 1000)
            else:
                samples.append(float('nan'))
            # 100ms = 10 msg/s, at the per-IP rate-limit (spec §7.1).
            # Faster than this trips close 1008 and isn't what this test
            # is measuring (rate-limit verification lives in ll_ratelimit_test.py).
            await asyncio.sleep(0.10)

    valid = [s for s in samples if s == s]  # filter NaN
    if not valid:
        print(f'[{label}] no responses received')
        return None
    print(f'[{label}] n={len(valid)}/{N_PROBES} '
          f'min={min(valid):.1f}ms  median={statistics.median(valid):.1f}ms  '
          f'p90={sorted(valid)[int(len(valid)*0.9)-1]:.1f}ms  max={max(valid):.1f}ms')
    return valid

async def main():
    print(f'=== Phase 1: BASELINE (no slow client) ===')
    await fast_probe('baseline')

    print(f'\n=== Phase 2: WITH slow client holding the socket open ===')
    ready = asyncio.Event()
    stop = asyncio.Event()
    slow_task = asyncio.create_task(slow_client(ready, stop))
    await ready.wait()
    await asyncio.sleep(0.5)  # let firmware register the second client
    try:
        await fast_probe('slow-pinned')
    finally:
        stop.set()
        try:
            await asyncio.wait_for(slow_task, 2.0)
        except asyncio.TimeoutError:
            slow_task.cancel()

asyncio.run(main())
