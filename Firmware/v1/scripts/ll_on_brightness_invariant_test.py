"""Verify the post-mini-sprint-bugs.md #2 fix (on/brightness coupling
invariants) on the live mirror.

Invariants tested:
  (A) After LL_EV_BRIGHTNESS with value > 0: on == true.
  (B) After LL_EV_BRIGHTNESS with value == 0: on == false.
  (C) After LL_EV_POWER_TOGGLE on=true while brightness == 0:
      brightness is auto-restored to LL_BRIGHTNESS_RESUME_DEFAULT (75).
  (D) After LL_EV_POWER_TOGGLE on=false: brightness is PRESERVED (not zeroed).
  (E) Coupled webapp envelopes ({on:true, brightness:N} and
      {on:false, brightness:0}) both land in consistent states regardless
      of apply order.

Each test reads state before+after and prints a pass/fail line.
"""
import asyncio, json, websockets

MIRROR = 'ws://192.168.5.229/ws'

async def get_state(ws):
    await ws.send(json.dumps({'op':'get_state','req_id':'gs','ts':0}))
    while True:
        raw = await asyncio.wait_for(ws.recv(), 3)
        msg = json.loads(raw)
        if msg.get('req_id') == 'gs':
            return msg['result']  # get_state's `result` IS the state object

async def set_state(ws, payload, label):
    await ws.send(json.dumps({'op':'set_state','req_id':label,'ts':0,'payload':payload}))
    while True:
        raw = await asyncio.wait_for(ws.recv(), 3)
        msg = json.loads(raw)
        if msg.get('req_id') == label:
            return
    # Allow a beat for the broadcast to settle
    await asyncio.sleep(0.1)

def check(label, state, pred, expected):
    ok = pred(state)
    marker = 'PASS' if ok else 'FAIL'
    print(f'  [{marker}] {label}: on={state["on"]} brightness={state["brightness"]} '
          f'(expected: {expected})')
    return ok

async def main():
    async with websockets.connect(MIRROR, ping_interval=None) as ws:
        # Initial probe
        s = await get_state(ws)
        print(f'initial: on={s["on"]} brightness={s["brightness"]}\n')

        print('Test A: {brightness: 60} alone -> on should become true')
        await set_state(ws, {'brightness': 60}, 'A')
        await asyncio.sleep(0.2)
        s = await get_state(ws)
        check('A', s, lambda x: x['on'] and x['brightness'] == 60, 'on=true, brightness=60')

        print('\nTest B: {brightness: 0} alone -> on should become false')
        await set_state(ws, {'brightness': 0}, 'B')
        await asyncio.sleep(0.2)
        s = await get_state(ws)
        check('B', s, lambda x: not x['on'] and x['brightness'] == 0, 'on=false, brightness=0')

        print('\nTest C: {on: true} alone while brightness==0 -> brightness should restore to 75')
        await set_state(ws, {'on': True}, 'C')
        await asyncio.sleep(0.2)
        s = await get_state(ws)
        check('C', s, lambda x: x['on'] and x['brightness'] == 75, 'on=true, brightness=75 (auto-restored)')

        print('\nTest D: {on: false} alone -> brightness preserved (not zeroed)')
        # First make sure brightness is something memorable
        await set_state(ws, {'brightness': 60}, 'D-setup')
        await asyncio.sleep(0.2)
        await set_state(ws, {'on': False}, 'D')
        await asyncio.sleep(0.2)
        s = await get_state(ws)
        check('D', s, lambda x: not x['on'] and x['brightness'] == 60, 'on=false, brightness=60 (preserved)')

        print('\nTest E1: coupled {on:true, brightness:80} -> (true, 80)')
        await set_state(ws, {'on': True, 'brightness': 80}, 'E1')
        await asyncio.sleep(0.2)
        s = await get_state(ws)
        check('E1', s, lambda x: x['on'] and x['brightness'] == 80, 'on=true, brightness=80')

        print('\nTest E2: coupled {on:false, brightness:0} -> (false, 0)')
        await set_state(ws, {'on': False, 'brightness': 0}, 'E2')
        await asyncio.sleep(0.2)
        s = await get_state(ws)
        check('E2', s, lambda x: not x['on'] and x['brightness'] == 0, 'on=false, brightness=0')

        # Cleanup: restore the mirror to a usable state
        print('\nCleanup: restore to (on=true, brightness=75)')
        await set_state(ws, {'on': True, 'brightness': 75}, 'cleanup')
        await asyncio.sleep(0.2)
        s = await get_state(ws)
        print(f'final: on={s["on"]} brightness={s["brightness"]}')

asyncio.run(main())
