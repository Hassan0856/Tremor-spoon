// tremor_spoon.ino  -  PHASE 5 (final): button, LED, battery protection added to Phase 4.
// Files in this folder: tremor_spoon.ino, config.h, imu.h, servo_out.h, cmd.h, bench.h,
//                       filters.h, wflc.h, axis.h, testmodes.h, ui.h, battery.h
#include "config.h"
#include "imu.h"
#include "servo_out.h"
#include "cmd.h"
#include "bench.h"
#include "filters.h"
#include "wflc.h"
#include "axis.h"
#include "testmodes.h"
#include "ui.h"
#include "battery.h"

uint32_t tNext, n = 0, battNext = 0;
char line[32];

void setup() {
  Serial.begin(921600);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  analogReadResolution(12);
  delay(1500);
  imuSetup();
  servoAttach(sRoll); servoAttach(sPitch);
  servoWriteDeg(sRoll, 0); servoWriteDeg(sPitch, 0);
  axisRoll.begin(&sRoll, ah.roll);
  axisPitch.begin(&sPitch, ah.pitch);
  Serial.println("# ready");
  tNext = micros();
}

void loop() {
  waitTick(tNext);
  imuUpdate();

  if (cmdPoll(line, sizeof line)) {                         // serial commands still work
    if (line[0] == 'v') batteryReport();                    // v = print the battery voltages
    else if (!benchCommand(line)) modeCommand(line);
    tNext = micros();
  }
  testModesTick();

  Press p = buttonPoll();
  if (p == SHORT_PRESS) { stabilize = !stabilize; testMode = 'n'; }
  // hold 3 s, handle still: measure the offsets again
  if (p == LONG_PRESS) {
    stabilize = false;
    servoWriteDeg(sRoll, 0); servoWriteDeg(sPitch, 0);
    imuRecalibrate();
    axisRoll.begin(&sRoll, ah.roll); axisPitch.begin(&sPitch, ah.pitch);
    tNext = micros();
  }

  if ((int32_t)(millis() - battNext) >= 0) { battNext = millis() + 100; batteryCheck(); }
  ledUpdate(stabilize, batteryLow);

  if (testMode == 'x') {
    float q = exerciseDeg();
    servoWriteDeg(sRoll, q); servoWriteDeg(sPitch, q);
  } else {
    axisRoll.step(ah.roll,  ap.roll,  stabilize);
    axisPitch.step(ah.pitch, ap.pitch, stabilize);
  }

  if (++n % 5 == 0)
    Serial.printf("%lu,%.3f,%.3f,%.3f,%.3f,%d\n", millis(),
                  ah.roll, ah.pitch, ap.roll, ap.pitch, stabilize ? 1 : 0);
}
