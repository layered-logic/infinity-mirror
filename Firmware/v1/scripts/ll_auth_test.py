"""
LL-057-D D3 verification — paired-mode auth gate + replay protection.

Exercises the auth gate, set_auth_mode / rotate_secret ops, and the
per-socket monotonic-ts + req_id-dedup replay guard against a live mirror.

Run (PowerShell, IDF venv):
    & "C:\\Espressif\\python_env\\idf5.5_py3.14_env\\Scripts\\python.exe" `
        Firmware\\v1\\scripts\\ll_auth_test.py

Canonicalization mirrors auth_logic.c: the signed region is the compact
envelope JSON minus its closing brace; the frame is that region followed
by  ,"hmac":"<hex>"}  . The device HMACs the received prefix, so the two
byte strings match with no re-serialization.

Leaves the mirror in OPEN mode. If the script aborts mid-run it makes a
best-effort attempt to restore open mode in a finally block; if that
fails the recessed-button factory reset is the recovery path.
"""

import asyncio
import hashlib
import hmac
import json
import time
import uuid

import websockets

MIRROR = "ws://192.168.5.229/ws"

S1 = "ll-pair-secret-alpha"
S2 = "ll-pair-secret-bravo"
S3 = "ll-pair-secret-charlie"

results = []


def record(name, ok, detail=""):
    results.append((name, ok))
    tag = "PASS" if ok else "FAIL"
    print(f"  [{tag}] {name}" + (f" — {detail}" if detail else ""))


def rid():
    return str(uuid.uuid4())


def _region(op, req_id, ts, payload):
    body = json.dumps({"op": op, "req_id": req_id, "ts": ts, "payload": payload},
                      separators=(",", ":"))
    assert body.endswith("}")
    return body[:-1]


def _hmac_hex(secret, region):
    return hmac.new(secret.encode(), region.encode(), hashlib.sha256).hexdigest()


def signed(secret, op, req_id, ts, payload):
    region = _region(op, req_id, ts, payload)
    return region + ',"hmac":"' + _hmac_hex(secret, region) + '"}'


def signed_badmac(secret, op, req_id, ts, payload):
    region = _region(op, req_id, ts, payload)
    h = _hmac_hex(secret, region)
    flipped = ("1" if h[0] == "0" else "0") + h[1:]
    return region + ',"hmac":"' + flipped + '"}'


def unsigned(op, req_id, ts, payload):
    return json.dumps({"op": op, "req_id": req_id, "ts": ts, "payload": payload},
                      separators=(",", ":"))


async def rpc(ws, frame):
    """Send a frame, return the matching response. Unsolicited state
    broadcasts ({"op":"state",...}) are emitted after every state-changing
    op and would otherwise desync the request/response stream — skip them."""
    await ws.send(frame)
    while True:
        msg = json.loads(await asyncio.wait_for(ws.recv(), 6))
        if msg.get("op") == "state":
            continue  # unsolicited broadcast, not our response
        await asyncio.sleep(0.2)  # stay well under the 10 msg/s rate limit
        return msg


def err_code(resp):
    e = resp.get("error")
    return e.get("code") if isinstance(e, dict) else None


async def restore_open(secret_guess):
    """Best-effort: drag the mirror back to open mode after a failure."""
    try:
        async with websockets.connect(MIRROR, ping_interval=None) as ws:
            ts = int(time.time()) + 99
            for frame in (unsigned("set_auth_mode", rid(), ts, {"mode": "open"}),
                          signed(secret_guess, "set_auth_mode", rid(), ts + 1,
                                 {"mode": "open"}),
                          signed(S1, "set_auth_mode", rid(), ts + 2,
                                 {"mode": "open"})):
                try:
                    r = await rpc(ws, frame)
                    if r.get("ok"):
                        print("  restore_open: mirror set back to open mode")
                        return
                except Exception:
                    pass
    except Exception as exc:
        print(f"  restore_open failed ({exc}) — factory reset may be needed")


async def main():
    base = int(time.time())

    # ----- Phase 1: open-mode regression -----------------------------
    print("\nPhase 1 — open-mode regression (gate must be dormant)")
    async with websockets.connect(MIRROR, ping_interval=None) as ws:
        r = await rpc(ws, unsigned("ping", rid(), 0, {}))
        fw = r.get("result", {}).get("fw_version", "?")
        record("unsigned ping", r.get("ok") is True, f"fw_version={fw}")

        r = await rpc(ws, unsigned("get_state", rid(), 0, {}))
        mode = r.get("result", {}).get("auth_mode")
        record("unsigned get_state", r.get("ok") is True and mode == "open",
               f"auth_mode={mode}")
        brightness = r.get("result", {}).get("brightness", 75)

        r = await rpc(ws, unsigned("set_state", rid(), 0, {"brightness": brightness}))
        record("unsigned set_state", r.get("ok") is True,
               "no-op write of current brightness")

    # ----- Phase 2: enable paired mode + gate ------------------------
    print("\nPhase 2 — enable paired mode + auth gate")
    async with websockets.connect(MIRROR, ping_interval=None) as ws:
        r = await rpc(ws, unsigned("set_auth_mode", rid(), 0,
                                   {"mode": "paired", "secret": S1}))
        record("set_auth_mode -> paired (unsigned, open mode)",
               r.get("ok") is True and r.get("result", {}).get("mode") == "paired")
        await asyncio.sleep(0.6)  # let auth_mode settle through the state bus

        r = await rpc(ws, unsigned("get_state", rid(), base, {}))
        record("unsigned get_state now rejected", err_code(r) == "auth_required",
               f"error={err_code(r)}")

        r = await rpc(ws, signed(S1, "get_state", rid(), base, {}))
        mode = r.get("result", {}).get("auth_mode") if r.get("ok") else None
        record("signed get_state accepted", r.get("ok") is True and mode == "paired",
               f"auth_mode={mode}")

        r = await rpc(ws, signed_badmac(S1, "get_state", rid(), base + 1, {}))
        record("tampered HMAC rejected", err_code(r) == "bad_hmac",
               f"error={err_code(r)}")

        # Replay — req_id dedup: send one signed frame twice verbatim.
        replay_frame = signed(S1, "set_state", rid(), base + 2,
                              {"brightness": brightness})
        r = await rpc(ws, replay_frame)
        first_ok = r.get("ok") is True
        r = await rpc(ws, replay_frame)
        record("replay (duplicate req_id) rejected",
               first_ok and err_code(r) == "stale_ts", f"error={err_code(r)}")

        # Replay — per-socket monotonic ts: fresh req_id, ts below last.
        r = await rpc(ws, signed(S1, "get_state", rid(), base + 1, {}))
        record("stale ts (below last accepted) rejected",
               err_code(r) == "stale_ts", f"error={err_code(r)}")

        # ----- Phase 3: rotate_secret --------------------------------
        print("\nPhase 3 — rotate_secret")
        r = await rpc(ws, signed(S1, "rotate_secret", rid(), base + 3,
                                 {"old_secret": S1, "new_secret": S2}))
        record("rotate_secret S1 -> S2 accepted", r.get("ok") is True)

        r = await rpc(ws, signed(S1, "get_state", rid(), base + 4, {}))
        record("old secret no longer verifies", err_code(r) == "bad_hmac",
               f"error={err_code(r)}")

        r = await rpc(ws, signed(S2, "get_state", rid(), base + 4, {}))
        record("new secret verifies", r.get("ok") is True)

        r = await rpc(ws, signed(S2, "rotate_secret", rid(), base + 5,
                                 {"old_secret": "wrong-old", "new_secret": S3}))
        record("rotate_secret with wrong old_secret rejected",
               err_code(r) == "bad_payload", f"error={err_code(r)}")

        # ----- Phase 4: disable paired mode --------------------------
        print("\nPhase 4 — disable paired mode (authenticated unpair)")
        r = await rpc(ws, signed(S2, "set_auth_mode", rid(), base + 6,
                                 {"mode": "open"}))
        record("set_auth_mode -> open (signed) accepted",
               r.get("ok") is True and r.get("result", {}).get("mode") == "open")

    await asyncio.sleep(0.6)

    # ----- Phase 5: confirm open mode --------------------------------
    print("\nPhase 5 — confirm open mode restored")
    async with websockets.connect(MIRROR, ping_interval=None) as ws:
        r = await rpc(ws, unsigned("get_state", rid(), 0, {}))
        mode = r.get("result", {}).get("auth_mode") if r.get("ok") else None
        record("unsigned get_state works again", r.get("ok") is True and mode == "open",
               f"auth_mode={mode}")
        r = await rpc(ws, unsigned("ping", rid(), 0, {}))
        record("unsigned ping works again", r.get("ok") is True)

    passed = sum(1 for _, ok in results if ok)
    print(f"\n==== {passed}/{len(results)} checks passed ====")
    return passed == len(results)


if __name__ == "__main__":
    ok = False
    try:
        ok = asyncio.run(main())
    finally:
        if not ok:
            print("\nrun incomplete or failing — attempting open-mode restore")
            asyncio.run(restore_open(S2))
    raise SystemExit(0 if ok else 1)
