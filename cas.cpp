#include <arduino.h>

#include "cas.h"
#include "utils.h"

int       cas_calibration(int cas_pin, bool log_cas) {
  int               cas_error = 512;
  unsigned long     ms_current = 0;
  unsigned long     ms_last = 0;

  cas_state         last_calibration;

  while (last_calibration.sample - cas_error != 0) {
    Serial.print("Adjusted CAS val :");
    Serial.println(last_calibration.sample - cas_error);
    last_calibration = cas_tick(last_calibration, analogRead(cas_pin));

    ms_current = millis();

    cas_error = last_calibration.sample;
    ms_last = ms_current;

    // JUST LOGGING
    if (log_cas) {
      if (ms_current / (10 SEC) > ms_last / (10 SEC)) {
        Serial.write("Calibrating CAS");
      }
      if (ms_current > 10 SEC && ms_current / 500 > ms_last / 500) {
        Serial.write(".");
        if (ms_current % (10 SEC) < 500) {
          Serial.println();
          Serial.write("last_calibration.sample: "); Serial.println(last_calibration.sample);
          Serial.write("cas_error: "); Serial.println(cas_error);
        }
      }
      if (ms_current / (10 SEC) > ms_last / (10 SEC)) {
        Serial.write("Calibrating CAS");
      }
    }
    // END LOGGING
  }

  return (cas_error);
}

cas_state cas_tick(cas_state state, int sample) {
  cas_state         newstate;

  // Near-zero -> zero
  if (sample >= -CAS_MARGIN && sample <= CAS_MARGIN) {
    newstate.sample = 0;
  } else {
    newstate.sample = sample;
  }

  if (state.shape == FALLING && sample >= state.sample + CAS_MARGIN) {
    // Higher than falling = change
    state.shape = CHANGE;
  } else if (state.shape == RISING && sample <= state.sample - CAS_MARGIN) {
    // Lower than rising = change
    state.shape = CHANGE;
  } else {
    if (sample < state.sample - CAS_MARGIN) {
      // Lesser than before by at least MARGIN
      newstate.shape = FALLING;
    } else if (sample > state.sample + CAS_MARGIN) {
      // Greater than before by at least MARGIN
      newstate.shape = RISING;
    } else {
      // Nothing detected, assume no change
      newstate.shape = state.shape;
    }
  }
  if (sample != state.sample) {
      newstate.tick[0] = state.tick[1];
      newstate.tick[1] = state.tick[2];
      newstate.tick[2] = micros();
    } else {
      newstate.tick[0] = state.tick[0];
      newstate.tick[1] = state.tick[1];
      newstate.tick[2] = state.tick[2];
    };
  return (newstate);
}

void  print_cas_state(cas_state state) {
  Serial.println("{");
  Serial.write("\t\"shape\": ");
    Serial.println(SHAPE_NAME(state.shape));
  Serial.write("\t\"sample\": ");
    Serial.println(state.sample);
  Serial.write("\t\"ticks\": [ ");
    Serial.print(state.tick[0]);
      Serial.write(", ");
    Serial.print(state.tick[1]);
      Serial.write(", ");
    Serial.print(state.tick[2]);
      Serial.println(" ]");
  Serial.println("}");

}