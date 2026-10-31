// tremor_spoon.ino  -  PHASE 1: read both IMUs and print the angles.
// Files in this folder: tremor_spoon.ino, config.h, imu.h
#include "config.h"
#include "imu.h"

uint32_t tNext;
uint32_t n = 0;

void setup() {
  Serial.begin(921600);
  delay(1500);
  // wake both IMUs, measure gyro offsets (keep still for 2 s)
  imuSetup();
  Serial.println("# ready");
  tNext = micros();
}

void loop() {
  waitTick(tNext);                              // wait for the next 2 ms tick
  // read both IMUs, update ah (handle) and ap (platform)
  imuUpdate();
  if (++n % 5 == 0)                             // print at 100 Hz
    Serial.printf("%lu,%.3f,%.3f,%.3f,%.3f,0\n", millis(), ah.roll, ah.pitch, ap.roll, ap.pitch);
}
