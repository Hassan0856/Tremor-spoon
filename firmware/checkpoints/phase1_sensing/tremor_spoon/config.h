// config.h - every number you may ever need to change is in THIS file.
// Create it in Phase 1 and leave it open. Later phases only change the values marked  <-- EDIT
#pragma once
#include <Arduino.h>

// ---------- Timing ----------
const float    FS      = 500.0f;          // control loop rate, Hz
const uint32_t LOOP_US = 2000;            // microseconds per loop = 1,000,000 / FS
const float    DT      = 1.0f / FS;       // seconds per loop

// ---------- XIAO ESP32-S3 pins (GPIO numbers) ----------
const int PIN_SDA         = 5;            // D4  I2C data, both IMUs
const int PIN_SCL         = 6;            // D5  I2C clock, both IMUs
const int PIN_ROLL_SERVO  = 1;            // D0
const int PIN_PITCH_SERVO = 2;            // D1
const int PIN_BUTTON      = 3;            // D2  button to GND
const int PIN_LED         = 4;            // D3  LED through 330 ohm
const int PIN_VBAT        = 7;            // D8  battery divider
const int PIN_VMID        = 8;            // D9  optional cell-midpoint divider

// ---------- IMUs (Phase 1) ----------
const uint8_t IMU_H = 0x68;               // handle IMU,   AD0 tied to GND
const uint8_t IMU_P = 0x69;               // platform IMU, AD0 tied to 3V3
const float   ALPHA = 0.998f;             // complementary filter: share given to the gyro
#ifndef HANDLE_IMU_VERTICAL
// <-- EDIT in Phase 5: set to 1 when the handle IMU stands on its bulkhead
#define HANDLE_IMU_VERTICAL 0
#endif

// ---------- Servos (Phase 3: fill in from the calibration) ----------
const int   SERVO_HZ   = 333;             // PWM frame rate; use 50 if your servo jitters
const int   SERVO_BITS = 14;              // PWM resolution
const float LIMIT_DEG  = 25.0f;           // software travel limit, each way
const float ROLL_CENTER_US  = 1500.0f;    // <-- EDIT  pulse that gives 0 deg
const float ROLL_US_PER_DEG = 10.0f;      // <-- EDIT  pulse change per degree
const int   ROLL_DIR        = +1;         // <-- EDIT  +1 or -1
const float PITCH_CENTER_US  = 1500.0f;   // <-- EDIT
const float PITCH_US_PER_DEG = 10.0f;     // <-- EDIT
const int   PITCH_DIR        = +1;        // <-- EDIT

// ---------- Tremor cancellation (Phase 4: tune these) ----------
// <-- EDIT  prediction horizon in samples (1 sample = 2 ms)
int         D_SAMPLES  = 12;
const float F_INTENDED = 1.5f;            // Hz, slow motion below this is the user's own movement
const float KP_TRIM    = 0.5f;            // slow trim gains
const float KI_TRIM    = 0.5f;
const float WFLC_F0    = 6.0f;            // Hz, starting guess for the tremor frequency
const float WFLC_MU0   = 5e-4f;           // frequency adaptation rate
const float WFLC_MU1   = 0.01f;           // weight adaptation rate

// ---------- Battery (Phase 5) ----------
// pack voltage = K_PACK x D8 voltage (100k / 47k divider)
const float K_PACK = 3.128f;
// cell 1 voltage = K_MID x D9 voltage (100k / 100k divider)
const float K_MID  = 2.0f;
// <-- EDIT  true if you built the optional per-cell sensing
const bool  HAS_MID = false;
const float V_CELL_WARN    = 3.40f;       // LED warning below this cell voltage
const float V_CELL_CUT     = 3.30f;       // shut down below this (average cell voltage)
// shut down below this (weakest cell, with per-cell sensing)
const float V_CELL_CUT_MID = 3.20f;
