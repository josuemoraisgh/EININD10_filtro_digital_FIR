#!/usr/bin/env python3
import argparse, asyncio, socket

def make_udp_sender(host, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    addr = (host, port)
    def send_lines(chunk: bytes):
        if not chunk:
            return
        for line in chunk.splitlines():
            if not line:
                continue
            # Teleplot: "canal: valor\n"
            sock.sendto(line + b"\n", addr)
    return send_lines

async def handle(reader: asyncio.StreamReader, writer: asyncio.StreamWriter, udp_send):
    peer = writer.get_extra_info('peername')
    print(f"[TCP] conectado: {peer}")
    try:
        while True:
            data = await reader.read(4096)
            if not data: break
            udp_send(data)
    finally:
        print(f"[TCP] desconectado: {peer}")
        try:
            writer.close(); await writer.wait_closed()
        except Exception: pass

async def run_server(host, port, udp_send):
    server = await asyncio.start_server(lambda r,w: handle(r,w,udp_send),
                                        host, port, reuse_port=True)
    addrs = ", ".join(str(s.getsockname()) for s in server.sockets)
    print(f"[SERVIDOR TCP] {addrs} → UDP Teleplot")
    async with server:
        await server.serve_forever()

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tcp-host", default="0.0.0.0")
    ap.add_argument("--tcp-port", type=int, default=5023)
    ap.add_argument("--udp-host", default="127.0.0.1")
    ap.add_argument("--udp-port", type=int, default=47269)  # Teleplot padrão
    args = ap.parse_args()

    udp_send = make_udp_sender(args.udp_host, args.udp_port)
    asyncio.run(run_server(args.tcp_host, args.tcp_port, udp_send))

if __name__ == "__main__":
    main()
