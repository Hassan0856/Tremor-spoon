// imu.h - reads both MPU6050 boards and turns them into roll and pitch angles.
// Create in Phase 1. Never edited again.
#pragma once
#include <Wire.h>
#include "config.h"

struct Raw   { float ax, ay, az, gx, gy, gz; };   // accelerometer in g, gyro in deg/s
struct Angle { float roll = 0, pitch = 0; };      // degrees

Raw   rh, rp;                 // latest readings: handle (h) and platform (p)
Angle ah, ap;                 // fused angles:    handle (h) and platform (p)
float biasH[3], biasP[3];     // gyro offsets measured at start-up
uint32_t imuErrors = 0;       // failed reads (should stay 0)

inline void imuWrite(uint8_t a, uint8_t reg, uint8_t v) {
  Wire.beginTransmission(a); Wire.write(reg); Wire.write(v); Wire.endTransmission();
}
inline void imuInit(uint8_t a) {
  imuWrite(a, 0x6B, 0x01);    // wake up, gyro clock
  imuWrite(a, 0x1A, 0x01);    // on-chip low-pass about 188 Hz
  imuWrite(a, 0x19, 0x00);    // 1 kHz internal sample rate
  imuWrite(a, 0x1B, 0x08);    // gyro +/-500 deg/s  -> 65.5 LSB per deg/s
  imuWrite(a, 0x1C, 0x08);    // accel +/-4 g       -> 8192 LSB per g
}

// Read 14 bytes starting at register 0x3B: accel, temperature, gyro.
inline bool imuRead(uint8_t a, Raw &o) {
  Wire.beginTransmission(a); Wire.write(0x3B);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(a, (uint8_t)14) != 14) return false;
  int16_t v[7];
  for (int i = 0; i < 7; i++) {
    uint8_t hi = Wire.read(); uint8_t lo = Wire.read();   // two separate reads, in this order
    v[i] = (int16_t)((hi << 8) | lo);
  }
  o.ax = v[0] / 8192.0f; o.ay = v[1] / 8192.0f; o.az = v[2] / 8192.0f;   // v[3] is temperature
  o.gx = v[4] / 65.5f;   o.gy = v[5] / 65.5f;   o.gz = v[6] / 65.5f;
#if HANDLE_IMU_VERTICAL
  // the board stands vertical, chip facing the rear: rotate into the handle frame
  if (a == IMU_H) {
    Raw b = o;
    o.ax = -b.az; o.ay = b.ay; o.az = b.ax;
    o.gx = -b.gz; o.gy = b.gy; o.gz = b.gx;
  }
#endif
  return true;
}

// Wait for the next 2 ms tick (keeps the loop at exactly 500 Hz). t starts as micros().
inline void waitTick(uint32_t &t) {
  while ((int32_t)(micros() - t) < 0) {}
  t += LOOP_US;
}

// Tilt from gravity alone (accelerometer).
inline void accelAngles(const Raw &r, float &roll, float &pitch) {
  roll  = atan2f(r.ay, r.az) * RAD_TO_DEG;
  pitch = atan2f(-r.ax, sqrtf(r.ay * r.ay + r.az * r.az)) * RAD_TO_DEG;
}
inline void initAngle(const Raw &r, Angle &a) { accelAngles(r, a.roll, a.pitch); }

// Complementary filter: gyro for fast changes, accelerometer to cancel the drift.
inline void fuse(const Raw &r, const float bias[3], Angle &a) {
  float accRoll, accPitch;
  accelAngles(r, accRoll, accPitch);
  a.roll  = ALPHA * (a.roll  + (r.gx - bias[0]) * DT) + (1 - ALPHA) * accRoll;
  a.pitch = ALPHA * (a.pitch + (r.gy - bias[1]) * DT) + (1 - ALPHA) * accPitch;
}

inline void calibrateBias(uint8_t a, float b[3]) {        // keep the IMU still for 2 s
  double s[3] = {0, 0, 0}; Raw r;
  for (int i = 0; i < 1000; i++) {
    if (imuRead(a, r)) { s[0] += r.gx; s[1] += r.gy; s[2] += r.gz; }
    delay(2);
  }
  for (int k = 0; k < 3; k++) b[k] = s[k] / 1000.0;
}

// Read both boards and update both fused angles. Call once per loop.
inline void imuUpdate() {
  if (imuRead(IMU_H, rh)) fuse(rh, biasH, ah); else imuErrors++;
  if (imuRead(IMU_P, rp)) fuse(rp, biasP, ap); else imuErrors++;
}

// Start-up (and re-calibration): measure gyro offsets and start the angles from gravity.
inline void imuRecalibrate() {
  calibrateBias(IMU_H, biasH); calibrateBias(IMU_P, biasP);
  if (imuRead(IMU_H, rh)) initAngle(rh, ah);
  if (imuRead(IMU_P, rp)) initAngle(rp, ap);
}
inline void imuSetup() {
  Wire.begin(PIN_SDA, PIN_SCL, 400000);
  imuInit(IMU_H); imuInit(IMU_P); delay(100);
  imuRecalibrate();
}
