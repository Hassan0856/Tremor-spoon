// filters.h - a 2nd-order low-pass filter that separates slow (intended) motion from tremor.
// Create in Phase 4. Never edited.
#pragma once
#include "config.h"

// 2nd-order Butterworth low-pass (RBJ biquad), doubles because fc/fs is small
struct Biquad {
  double b0 = 1, b1 = 0, b2 = 0, a1 = 0, a2 = 0, z1 = 0, z2 = 0;
  void lowpass(double fc, double fs) {
    double w = 2 * PI * fc / fs, cw = cos(w), al = sin(w) / (2 * 0.7071067811865476);
    double a0 = 1 + al;
    b0 = (1 - cw) / 2 / a0; b1 = (1 - cw) / a0; b2 = b0; a1 = -2 * cw / a0; a2 = (1 - al) / a0;
  }
  float process(float x) {                        // transposed direct form II
    double y = b0 * x + z1; z1 = b1 * x - a1 * y + z2; z2 = b2 * x - a2 * y; return (float)y;
  }
  // start at steady state v, so nothing swings at start-up
  void reset(float v) { z1 = v * (1 - b0); z2 = v * (b2 - a2); }
};
