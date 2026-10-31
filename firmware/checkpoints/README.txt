TREMOR SPOON FIRMWARE
=====================
Each folder is the complete sketch as it should look at the END of that phase.
If your own copy misbehaves, open the matching folder in the Arduino IDE
(File > Open > tremor_spoon.ino) and compare it with yours.

  phase1_sensing/   files: tremor_spoon.ino config.h imu.h
  phase3_servos/    adds : servo_out.h cmd.h bench.h            (tremor_spoon.ino is REPLACED)
  phase4_tremor/    adds : filters.h wflc.h axis.h testmodes.h  (tremor_spoon.ino is REPLACED)
  phase5_final/     adds : ui.h battery.h                       (tremor_spoon.ino is REPLACED)
  tools/            logger.py analyze.py  (run on the laptop, not on the XIAO)

Arduino IDE 2 settings: Board "XIAO_ESP32S3", USB CDC On Boot "Enabled", ESP32 board package 3.x.
Serial Monitor: 921600 baud, line ending "Newline".
Only config.h is edited after Phase 1 (lines marked  <-- EDIT).
