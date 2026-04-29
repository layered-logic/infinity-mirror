import serial, time, sys

port = sys.argv[1] if len(sys.argv) > 1 else "COM3"
duration = int(sys.argv[2]) if len(sys.argv) > 2 else 12

s = serial.Serial(port, 115200, timeout=1)
s.dtr = False
s.rts = True
time.sleep(0.05)
s.rts = False
time.sleep(0.05)

end = time.time() + duration
while time.time() < end:
    line = s.readline()
    if line:
        try:
            print(line.decode("utf-8", errors="replace").rstrip())
        except Exception:
            pass
s.close()
