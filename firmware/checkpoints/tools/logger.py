# logger.py - saves the angle log from the XIAO to a CSV file.
# Usage:  python logger.py COM5 data/run01.csv 60 a
#                          port  file            seconds  [commands to send, e.g. a  or  1]
# Close the Arduino Serial Monitor first: only one program can use the port.
import serial, sys, time

port, out, secs = sys.argv[1], sys.argv[2], float(sys.argv[3])
cmds = sys.argv[4:]
s = serial.Serial()
s.port, s.baudrate, s.timeout = port, 921600, 1
s.dtr = False; s.rts = False               # do not reset the board when the port opens
s.open()
print("waiting for the board (keep the handle still) ...")
t0 = time.time()
while True:
    line = s.readline().decode(errors="ignore").strip()
    if line.startswith("#"):
        print(line)
    if line.startswith("# ready") or line.count(",") == 5:   # ready, or already running
        break
    if time.time() - t0 > 20:
        sys.exit("Nothing received: press the board's reset button, check the port name "
                 "and that USB CDC On Boot is Enabled.")
for c in cmds:                             # e.g. 'a' starts the A/B test
    s.write((c + "\n").encode()); time.sleep(0.2)
with open(out, "w") as f:
    f.write("t_ms,hand_roll,hand_pitch,plat_roll,plat_pitch,mode\n")
    end = time.time() + secs
    while time.time() < end:
        line = s.readline().decode(errors="ignore").strip()
        if line.startswith("#"):
            print(line)                    # live feedback, shown but not saved
        elif line.count(",") == 5:
            f.write(line + "\n")
s.write(b"0\n")                            # stabilization off at the end
print("saved", out)
