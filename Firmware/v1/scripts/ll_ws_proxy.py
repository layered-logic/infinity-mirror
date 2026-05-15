"""TCP-level proxy for WS traffic to the mirror. Supports configurable
one-way latency (per-byte forwarding delay) and frame-loss probability.

Usage:
  python ll_ws_proxy.py --latency-ms 200 --listen-port 9000
  python ll_ws_proxy.py --loss 0.05 --listen-port 9000

Then point any of the WS test scripts at ws://127.0.0.1:9000/ws by
overriding MIRROR via the LL_MIRROR env var (or temporarily edit the
script). The proxy forwards bytes between the local TCP listener and
the upstream mirror; latency adds an asyncio.sleep before each forward,
loss randomly drops chunks before forwarding.

Caveats:
  - Loss is at the TCP-byte-chunk level, NOT the WS-frame level. Drops
    will tear the stream and force TCP retransmits or socket closes.
    That's the realistic packet-loss surface; if you want frame-only
    drops, that requires WS-aware parsing which would defeat the
    purpose (proxy would need to track frame boundaries).
  - Latency is per-direction. End-to-end RTT gains 2× the configured
    latency.
"""
import argparse, asyncio, random, sys

UPSTREAM_HOST = '192.168.5.229'
UPSTREAM_PORT = 80

async def pipe(reader, writer, latency_s, loss_p, label):
    try:
        while True:
            data = await reader.read(4096)
            if not data:
                break
            if loss_p > 0 and random.random() < loss_p:
                # Drop this chunk — TCP retransmit will (eventually) resend.
                continue
            if latency_s > 0:
                await asyncio.sleep(latency_s)
            writer.write(data)
            await writer.drain()
    except (ConnectionResetError, BrokenPipeError):
        pass
    finally:
        try:
            writer.close()
        except Exception:
            pass

async def handle_client(client_r, client_w, latency_s, loss_p):
    try:
        up_r, up_w = await asyncio.open_connection(UPSTREAM_HOST, UPSTREAM_PORT)
    except Exception as e:
        print(f'upstream connect failed: {e!r}', file=sys.stderr)
        client_w.close()
        return
    await asyncio.gather(
        pipe(client_r, up_w,   latency_s, loss_p, 'c->u'),
        pipe(up_r,     client_w, latency_s, loss_p, 'u->c'),
    )

async def main():
    p = argparse.ArgumentParser()
    p.add_argument('--listen-port', type=int, default=9000)
    p.add_argument('--latency-ms', type=int, default=0)
    p.add_argument('--loss', type=float, default=0.0, help='0.0-1.0 probability')
    p.add_argument('--seed', type=int, default=None)
    args = p.parse_args()

    if args.seed is not None:
        random.seed(args.seed)
    lat_s = args.latency_ms / 1000.0

    server = await asyncio.start_server(
        lambda r, w: handle_client(r, w, lat_s, args.loss),
        '127.0.0.1', args.listen_port)
    print(f'll_ws_proxy: listen=127.0.0.1:{args.listen_port}  upstream={UPSTREAM_HOST}:{UPSTREAM_PORT}  '
          f'latency_ms={args.latency_ms}  loss={args.loss}', flush=True)
    async with server:
        await server.serve_forever()

asyncio.run(main())
