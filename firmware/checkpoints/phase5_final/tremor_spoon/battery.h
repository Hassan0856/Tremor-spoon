// battery.h - battery warning and a safe shut-down, for a pack WITHOUT a full protection board.
// Create in Phase 5. Never edited (the numbers are in config.h).
//   D8: pack + -> 100k -> D8 -> 47k -> GND
//   D9 (optional): cell midpoint -> 100k -> D9 -> 100k -> GND
#pragma once
#include "esp_sleep.h"
#include "config.h"
#include "imu.h"
#include "servo_out.h"

// the LED code shows a fast blink while this is true
bool batteryLow = false;

inline void shutDown() {
  servoWriteDeg(sRoll, 0); servoWriteDeg(sPitch, 0); delay(300);   // centre the gimbal
  // no pulses: the servos go limp
  ledcDetach(sRoll.pin); ledcDetach(sPitch.pin);
  imuWrite(IMU_H, 0x6B, 0x40); imuWrite(IMU_P, 0x6B, 0x40);         // IMUs to sleep
  digitalWrite(PIN_LED, LOW);
  // no wake-up source: only switching the pack off and on restarts it
  esp_deep_sleep_start();
}

// Type  v  in the Serial Monitor: prints the voltages so you can compare them with a multimeter.
inline void batteryReport() {
  float vPack = K_PACK * analogReadMilliVolts(PIN_VBAT) / 1000.0f;
  if (HAS_MID) {
    float c1 = K_MID * analogReadMilliVolts(PIN_VMID) / 1000.0f;
    Serial.printf("# pack %.2f V   cell 1 %.2f V   cell 2 %.2f V\n", vPack, c1, vPack - c1);
  } else {
    Serial.printf("# pack %.2f V   (about %.2f V per cell)\n", vPack, vPack / 2);
  }
}

inline void batteryCheck() {                      // call every 100 ms
  static uint8_t lowCount = 0;
  float vPack = K_PACK * analogReadMilliVolts(PIN_VBAT) / 1000.0f;
  // pack off: running from USB, nothing to protect
  if (vPack < 5.0f) { lowCount = 0; batteryLow = false; return; }
  float vWeak = vPack / 2, cut = V_CELL_CUT;                        // average cell
  if (HAS_MID) {
    float c1 = K_MID * analogReadMilliVolts(PIN_VMID) / 1000.0f;    // cell 1
    vWeak = min(c1, vPack - c1); cut = V_CELL_CUT_MID;              // the weaker of the two cells
  }
  batteryLow = vWeak < V_CELL_WARN;
  lowCount = (vWeak < cut) ? lowCount + 1 : 0;
  // 2 s below the limit, so short servo current dips do not trip it
  if (lowCount >= 20) shutDown();
}
