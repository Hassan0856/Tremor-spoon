// tremor_spoon.ino  -  PHASE 4: tremor cancellation, test modes and logging.
// Files in this folder: tremor_spoon.ino, config.h, imu.h, servo_out.h, cmd.h, bench.h,
//                       filters.h, wflc.h, axis.h, testmodes.h
#include "config.h"
#include "imu.h"
#include "servo_out.h"
#include "cmd.h"
#include "bench.h"
#include "filters.h"
#include "wflc.h"
#include "axis.h"
#include "testmodes.h"

uint32_t tNext, n = 0;
char line[32];

void setup() {
  Serial.begin(921600);
  delay(1500);
  imuSetup();
  servoAttach(sRoll); servoAttach(sPitch);
  servoWriteDeg(sRoll, 0); servoWriteDeg(sPitch, 0);
  axisRoll.begin(&sRoll, ah.roll);
  axisPitch.begin(&sPitch, ah.pitch);
  Serial.println("# ready. Commands: 0 off, 1 on, a A/B test, x exercise, d12 horizon");
  tNext = micros();
}

void loop() {
  waitTick(tNext);
  imuUpdate();

  if (cmdPoll(line, sizeof line)) {                         // typed commands
    if (!benchCommand(line)) modeCommand(line);
    tNext = micros();
  }
  // A/B timer and the once-a-second '#' line
  testModesTick();

  if (testMode == 'x') {                                    // exercise: both servos follow a sine
    float q = exerciseDeg();
    servoWriteDeg(sRoll, q); servoWriteDeg(sPitch, q);
  } else {                                                  // the real thing
    axisRoll.step(ah.roll,  ap.roll,  stabilize);
    axisPitch.step(ah.pitch, ap.pitch, stabilize);
  }

  if (++n % 5 == 0)                                         // log at 100 Hz
    Serial.printf("%lu,%.3f,%.3f,%.3f,%.3f,%d\n", millis(),
                  ah.roll, ah.pitch, ap.roll, ap.pitch, stabilize ? 1 : 0);
}
