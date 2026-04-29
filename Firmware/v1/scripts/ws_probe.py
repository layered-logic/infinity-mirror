"""Quick WS probe: connect, send one op, print response.

Usage:
    python ws_probe.py <op> [json_payload]

Examples:
    python ws_probe.py get_state
    python ws_probe.py set_state '{"brightness": 50}'
    python ws_probe.py set_wifi_creds '{"ssid": "x", "password": "y"}'

Defaults to ws://192.168.4.1/ws. Override via WS_URL env var.
"""

import asyncio
import json
import os
import sys

import websockets


async def main():
    op = sys.argv[1] if len(sys.argv) > 1 else "get_state"
    payload = json.loads(sys.argv[2]) if len(sys.argv) > 2 else None
    url = os.environ.get("WS_URL", "ws://192.168.4.1/ws")

    envelope = {"op": op, "req_id": "probe", "ts": 0}
    if payload is not None:
        envelope["payload"] = payload

    async with websockets.connect(url) as ws:
        await ws.send(json.dumps(envelope))
        resp = await asyncio.wait_for(ws.recv(), 5)
        print(json.dumps(json.loads(resp), indent=2))


asyncio.run(main())
