// tremor_spoon.ino  -  PHASE 3: calibrate the servos and measure their delay.
// Files in this folder: tremor_spoon.ino, config.h, imu.h, servo_out.h, cmd.h, bench.h
#include "config.h"
#include "imu.h"
#include "servo_out.h"
#include "cmd.h"
#include "bench.h"

uint32_t tNext;
char line[32];

void setup() {
  Serial.begin(921600);
  delay(1500);
  imuSetup();
  servoAttach(sRoll); servoAttach(sPitch);
  servoWriteDeg(sRoll, 0); servoWriteDeg(sPitch, 0);        // both servos to centre
  Serial.println("# ready. Commands: r1500  p1500  m  tp  tr   (line ending: Newline)");
  tNext = micros();
}

void loop() {
  waitTick(tNext);
  imuUpdate();
  if (cmdPoll(line, sizeof line)) {
    // runs the command (some take a few seconds)
    benchCommand(line);
    tNext = micros();                                       // restart the 2 ms clock afterwards
  }
}
