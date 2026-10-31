// servo_out.h - turns an angle in degrees into a servo pulse.
// Create in Phase 3. Never edited (the calibration numbers live in config.h).
#pragma once
#include "config.h"

struct ServoCal { int pin; float centerUs; float usPerDeg; int dir; };
ServoCal sRoll  = {PIN_ROLL_SERVO,  ROLL_CENTER_US,  ROLL_US_PER_DEG,  ROLL_DIR};
ServoCal sPitch = {PIN_PITCH_SERVO, PITCH_CENTER_US, PITCH_US_PER_DEG, PITCH_DIR};

// Arduino-ESP32 core 3.x. (On core 2.x use ledcSetup() + ledcAttachPin() instead.)
inline void servoAttach(const ServoCal &s) { ledcAttach(s.pin, SERVO_HZ, SERVO_BITS); }

inline void servoWriteUs(int pin, float us) {
  us = constrain(us, 800.0f, 2200.0f);                       // never ask for a silly pulse
  uint32_t duty = (uint32_t)(us * SERVO_HZ * (float)(1 << SERVO_BITS) / 1e6f);
  ledcWrite(pin, duty);
}
inline void servoWriteDeg(const ServoCal &s, float deg) {
  deg = constrain(deg, -LIMIT_DEG, LIMIT_DEG);               // software limit
  servoWriteUs(s.pin, s.centerUs + s.dir * s.usPerDeg * deg);
}
