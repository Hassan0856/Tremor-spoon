// wflc.h - Weighted-frequency Fourier Linear Combiner: learns the tremor's frequency and
// amplitude, and predicts it D samples ahead. Create in Phase 4. Never edited.
#pragma once
#include "config.h"

class WFLC {
 public:
  static const int M = 1;                         // harmonics; try 2 later
  float w0, mu0, mu1, wmin, wmax, phi = 0, phiNow = 0, w[2 * M] = {0};
  WFLC(float fs = FS, float f0 = WFLC_F0, float m0 = WFLC_MU0, float m1 = WFLC_MU1)
    : w0(2 * PI * f0 / fs), mu0(m0), mu1(m1), wmin(2 * PI * 3 / fs), wmax(2 * PI * 12 / fs) {}

  float update(float s) {                         // s = tremor angle in RADIANS
    float x[2 * M], y = 0, sum = 0;
    for (int r = 1; r <= M; r++) { x[r - 1] = sinf(r * phi); x[M + r - 1] = cosf(r * phi); }
    for (int i = 0; i < 2 * M; i++) y += w[i] * x[i];
    float e = s - y;
    for (int r = 1; r <= M; r++) sum += r * (w[r - 1] * x[M + r - 1] - w[M + r - 1] * x[r - 1]);
    // frequency estimate, clamped to 3-12 Hz
    w0 = constrain(w0 + 2 * mu0 * e * sum, wmin, wmax);
    for (int i = 0; i < 2 * M; i++) w[i] += 2 * mu1 * e * x[i];  // weights
    phiNow = phi;
    phi += w0; if (phi > 2 * PI) phi -= 2 * PI;
    return y;
  }
  float predict(int D) const {                    // the tremor D samples after the latest sample
    float p = phiNow + D * w0, y = 0;
    for (int r = 1; r <= M; r++) y += w[r - 1] * sinf(r * p) + w[M + r - 1] * cosf(r * p);
    return y;
  }
  float freqHz(float fs = FS) const { return w0 * fs / (2 * PI); }
  // fundamental, in degrees
  float ampDeg() const { return sqrtf(w[0] * w[0] + w[M] * w[M]) * RAD_TO_DEG; }
};
