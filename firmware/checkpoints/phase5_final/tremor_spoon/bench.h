// bench.h - typed commands for calibrating the servos and measuring their delay.
// Create in Phase 3. Never edited. Keep the handle CLAMPED while you use these.
//
//   r1500   send 1500 microseconds to the ROLL servo      p1500   same for the PITCH servo
//   m       print the platform angle relative to the handle (average of 0.5 s)
//   tp      step-response test of the pitch servo (10 steps), prints dead time and half-rise time
//   tr      the same for the roll servo
#pragma once
#include "config.h"
#include "imu.h"
#include "servo_out.h"

inline void measureRelative() {
  uint32_t t = micros(); double sr = 0, sp = 0;
  for (int i = 0; i < 250; i++) {
    waitTick(t); imuUpdate();
    sr += ap.roll - ah.roll; sp += ap.pitch - ah.pitch;
  }
  Serial.printf("# platform minus handle: roll %.2f deg, pitch %.2f deg\n", sr / 250, sp / 250);
}

inline void stepTest(bool pitch) {
  ServoCal &s = pitch ? sPitch : sRoll;
  float sumDead = 0, sumHalf = 0; int n = 0;
  for (int rep = 1; rep <= 10; rep++) {
    servoWriteDeg(s, 0);
    uint32_t t = micros();
    // 0.8 s: settle at the centre
    for (int i = 0; i < 400; i++) { waitTick(t); imuUpdate(); }
    // angle before the step
    float a0 = pitch ? ap.pitch : ap.roll;
    int dead = -1, half = -1;
    for (int i = 0; i < 300; i++) {                                         // 0.6 s: record
      waitTick(t); imuUpdate();
      if (i == 50) servoWriteDeg(s, 10.0f);                                 // the step command
      if (i > 50) {
        float rate = pitch ? rp.gy - biasP[1] : rp.gx - biasP[0];
        float da   = (pitch ? ap.pitch : ap.roll) - a0;
        if (dead < 0 && fabsf(rate) > 20.0f) dead = i - 50;                 // first movement
        // half of the 10 deg step
        if (half < 0 && fabsf(da) >= 5.0f)   half = i - 50;
      }
    }
    if (dead > 0 && half > 0) {
      Serial.printf("# step %d: dead time %d ms, half-rise %d ms\n", rep, dead * 2, half * 2);
      sumDead += dead * 2; sumHalf += half * 2; n++;
    } else {
      Serial.printf("# step %d: no clear response (check the servo, the sign, the clamp)\n", rep);
    }
  }
  servoWriteDeg(s, 0);
  if (n) Serial.printf("# AVERAGE of %d: dead %.0f ms, half-rise %.0f ms -> start D = %.0f\n",
                       n, sumDead / n, sumHalf / n, (sumHalf / n) / 2.0f);
}

// Returns true if the line was one of ours.
inline bool benchCommand(const char *c) {
  if (c[0] == 'r') {
    servoWriteUs(sRoll.pin, atof(c + 1));
    Serial.printf("# roll servo %s us\n", c + 1); return true;
  }
  if (c[0] == 'p') {
    servoWriteUs(sPitch.pin, atof(c + 1));
    Serial.printf("# pitch servo %s us\n", c + 1); return true;
  }
  if (c[0] == 'm') { measureRelative(); return true; }
  if (c[0] == 't') { stepTest(c[1] != 'r'); return true; }
  return false;
}
