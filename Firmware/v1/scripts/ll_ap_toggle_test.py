"""STA-AP toggle test for LL-055 Session D — MANUAL.

Requires Bill at the keyboard to physically toggle the AP the mirror is
connected to (turn the Pixel hotspot off for ~30s, then back on).

How it runs:
  1. Opens a WS to the mirror, pings every 3s, records uptime + RTT.
  2. When the AP goes away, the WS receive will eventually fail (TCP
     keepalive timeout or LWIP socket cleanup, typically 30-60s).
  3. Script then tries to reconnect every 5s until it succeeds.
  4. Reports time-to-detect-disconnect, time-to-reconnect, and whether
     uptime monotonically resumed (confirming the firmware did NOT
     reboot through the outage — it should re-join via the SM's
     scanning + 5/15/60s backoff cycle in provisioning.c).

Usage:
  python ll_ap_toggle_test.py [--duration 180]

  Then at any point during the run, toggle the AP off. After ~30s,
  toggle it back on. Watch the script output.
"""
import argparse, asyncio, json, time, websockets

MIRROR = 'ws://192.168.5.229/ws'

class Phase:
    CONNECTED    = 'connected'
    DISCONNECTED = 'disconnected'
    RECONNECTING = 'reconnecting'

async def probe_once(timeout=4.0):
    """Single open + ping; returns (ok, uptime, rtt_ms, err)."""
    t0 = time.monotonic()
    try:
        async with websockets.connect(MIRROR, ping_interval=None,
                                       open_timeout=timeout, close_timeout=2) as ws:
            await ws.send(json.dumps({'op':'ping','req_id':'apt','ts':0}))
            r = json.loads(await asyncio.wait_for(ws.recv(), timeout))
            return (True, r.get('result',{}).get('uptime_s'),
                    (time.monotonic()-t0)*1000, None)
    except Exception as e:
        return (False, None, (time.monotonic()-t0)*1000, repr(e))

async def main():
    p = argparse.ArgumentParser()
    p.add_argument('--duration', type=int, default=180,
                   help='max seconds to run before exiting')
    args = p.parse_args()

    phase = Phase.CONNECTED
    t_start = time.monotonic()
    t_lost  = None
    t_back  = None
    last_uptime = None

    print('AP toggle test — toggle your hotspot OFF when ready, then back ON after ~30s.')
    print(f'Mirror = {MIRROR}    max-duration = {args.duration}s\n')

    while time.monotonic() - t_start < args.duration:
        ok, up, rtt, err = await probe_once()
        t_now = time.monotonic() - t_start

        if ok:
            if phase == Phase.RECONNECTING or phase == Phase.DISCONNECTED:
                t_back = time.monotonic()
                gap = t_back - t_lost if t_lost else None
                rebooted = (last_uptime is not None and up is not None and up < last_uptime)
                print(f'[{t_now:5.1f}s] BACK UP — uptime={up}s rtt={rtt:.0f}ms '
                      f'gap={gap:.1f}s rebooted={rebooted}')
                phase = Phase.CONNECTED
            else:
                print(f'[{t_now:5.1f}s] ok    uptime={up}s rtt={rtt:.0f}ms')
            last_uptime = up
        else:
            if phase == Phase.CONNECTED:
                t_lost = time.monotonic()
                print(f'[{t_now:5.1f}s] LOST  err={err}')
                phase = Phase.RECONNECTING
            else:
                print(f'[{t_now:5.1f}s] still down ({err.split(",")[0][:60]}...)')

        await asyncio.sleep(3.0 if phase == Phase.CONNECTED else 5.0)

    print('\n--- summary ---')
    if t_lost and t_back:
        print(f'outage detected at  +{t_lost-t_start:.1f}s')
        print(f'connection back at  +{t_back-t_start:.1f}s')
        print(f'observed gap        {t_back-t_lost:.1f}s')
    elif t_lost and not t_back:
        print(f'outage at +{t_lost-t_start:.1f}s — never recovered within {args.duration}s')
    else:
        print('no outage observed during the run')

asyncio.run(main())
