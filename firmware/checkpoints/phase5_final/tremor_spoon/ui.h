// ui.h - button and status LED. Create in Phase 5. Never edited.
//   short press : stabilization on / off        hold 3 s : re-calibrate (keep the handle still)
//   LED         : steady = stabilizing, short blink once a second = off, fast blink = battery low
#pragma once
#include "config.h"

enum Press { NO_PRESS, SHORT_PRESS, LONG_PRESS };

inline Press buttonPoll() {                       // call every loop
  static bool raw = false, stable = false, longSent = false;
  static uint32_t tChange = 0, tDown = 0;
  // pressed = LOW (button to GND, internal pull-up)
  bool r = (digitalRead(PIN_BUTTON) == LOW);
  uint32_t now = millis();
  if (r != raw) { raw = r; tChange = now; }
  if (raw != stable && now - tChange >= 30) {     // 30 ms debounce
    stable = raw;
    if (stable) { tDown = now; longSent = false; }
    else if (!longSent) return SHORT_PRESS;       // released before 3 s
  }
  if (stable && !longSent && now - tDown >= 3000) { longSent = true; return LONG_PRESS; }
  return NO_PRESS;
}

inline void ledUpdate(bool stabilizing, bool batteryLow) {
  uint32_t t = millis();
  bool on;
  if (batteryLow)       on = (t / 100) % 2 == 0;  // fast blink
  else if (stabilizing) on = true;                // steady
  else                  on = (t % 1000) < 100;    // short blink
  digitalWrite(PIN_LED, on ? HIGH : LOW);
}
