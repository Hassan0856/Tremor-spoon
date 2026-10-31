// testmodes.h - serial commands and the on/off test blocks used for the hand tests.
// Create in Phase 4. Never edited.
//
//   1   stabilization ON        0   stabilization OFF (normal mode)
//   a   A/B test: ON and OFF switch every 10 s, in random order inside each pair
//   x   exercise: both servos follow a 6 Hz +/-10 deg sine (battery test, handle in a stand)
//   d14 set the prediction horizon D to 14 samples (no re-flash needed)
#pragma once
#include "config.h"
#include "axis.h"

char     testMode = 'n';                          // 'n' normal, 'a' A/B blocks, 'x' exercise
uint32_t abNext = 0, abBlock = 0, infoNext = 0;
bool     abFirst = false;
float    exPhase = 0;

inline bool modeCommand(const char *c) {          // returns true if the line was one of ours
  switch (c[0]) {
    case '0': stabilize = false; testMode = 'n'; break;
    case '1': stabilize = true;  testMode = 'n'; break;
    case 'a': testMode = 'a'; abBlock = 0; abNext = millis(); break;
    case 'x': testMode = 'x'; break;
    case 'd':
      D_SAMPLES = constrain(atoi(c + 1), 0, 40);
      Serial.printf("# D = %d samples\n", D_SAMPLES); break;
    default: return false;
  }
  return true;
}

inline float exerciseDeg() {                      // next value of the 6 Hz, +/-10 deg sine
  exPhase += 2 * PI * 6.0f / FS; if (exPhase > 2 * PI) exPhase -= 2 * PI;
  return 10.0f * sinf(exPhase);
}

inline void testModesTick() {                     // call once per loop
  uint32_t now = millis();
  if (testMode == 'a' && (int32_t)(now - abNext) >= 0) {
    if (abBlock % 2 == 0) abFirst = random(2);    // new random order for each on/off pair
    stabilize = (abBlock % 2 == 0) ? abFirst : !abFirst;
    abBlock++; abNext += 10000;                   // 10 s blocks
  }
  if ((int32_t)(now - infoNext) >= 0) {           // live feedback once a second
    infoNext = now + 1000;
    Serial.printf("# roll %.1f Hz +/-%.1f deg | pitch %.1f Hz +/-%.1f deg | %s\n",
                  axisRoll.wf.freqHz(), axisRoll.wf.ampDeg(),
                  axisPitch.wf.freqHz(), axisPitch.wf.ampDeg(),
                  stabilize ? "ON" : "off");
  }
}
