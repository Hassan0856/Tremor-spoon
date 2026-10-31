// cmd.h - reads one typed command line from the USB serial port.
// Create in Phase 3. Never edited.
// In the Arduino Serial Monitor set the line ending (bottom right) to "Newline".
#pragma once
#include "config.h"

// Returns true once a full line has arrived; the text is in out[].
inline bool cmdPoll(char *out, size_t n) {
  static size_t len = 0;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (len) { out[len] = 0; len = 0; return true; }
    } else if (len < n - 1) {
      out[len++] = c;
    }
  }
  return false;
}
