// axis.h - the controller for ONE axis (roll or pitch). Two copies are created at the bottom.
// Create in Phase 4. Never edited (tune the numbers in config.h).
#pragma once
#include "config.h"
#include "servo_out.h"
#include "filters.h"
#include "wflc.h"

// true = cancelling the tremor. Set by serial command or the button.
bool stabilize = false;

struct Axis {
  Biquad intendedLP, errLP;                       // slow-motion filters
  WFLC   wf;                                      // tremor predictor
  float  integ = 0;                               // slow trim
  ServoCal *servo = nullptr;

  void begin(ServoCal *s, float startAngle) {     // call at start-up and after a re-calibration
    servo = s;
    intendedLP.lowpass(F_INTENDED, FS); errLP.lowpass(F_INTENDED, FS);
    intendedLP.reset(startAngle); errLP.reset(0); integ = 0;
  }

  // thetaH = handle angle, thetaP = platform angle, degrees. Returns the servo command (deg).
  float step(float thetaH, float thetaP, bool on) {
    float intended = intendedLP.process(thetaH);              // what the user means to do
    float tremor   = thetaH - intended;                       // the shaking
    wf.update(tremor * DEG_TO_RAD);                           // learn it
    // where it will be D samples from now
    float pred = wf.predict(D_SAMPLES) * RAD_TO_DEG;
    float err  = errLP.process(intended - thetaP);            // slow error of the spoon
    float q = 0;
    if (on) {
      integ = constrain(integ + KI_TRIM * err / FS, -5.0f, 5.0f);
      q = -pred + KP_TRIM * err + integ;                      // move OPPOSITE to the tremor
    } else {
      integ = 0;
    }
    servoWriteDeg(*servo, q);
    return q;
  }
};

Axis axisRoll, axisPitch;
