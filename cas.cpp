#include <arduino.h>

#include "cas.h"
#include "utils.h"

int       cas_calibration(int cas_pin, bool log_cas) {
  int               cas_error = 512;
  unsigned long     ms_current = 0;
  unsigned long     ms_last = 0;

  static cas_state  last_calibration;

  while (last_calibration.sample - cas_error != 0) {
    last_calibration = cas_tick(last_calibration, analogRead(cas_pin));

    ms_current = millis();
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
    cas_error = last_calibration.sample + cas_error;
    ms_last = ms_current;
  }

  return (cas_error);
}

cas_state cas_tick(cas_state state, int sample) {
  cas_state         newstate;

  Serial.println("cas_tick newstate:");
  print_cas_state(newstate);

  // Near-zero -> zero
  if (sample > -1 && sample < 1) {
    newstate.sample = 0;
  }
  if (sample < state.sample -1 && sample > 0) {
    newstate.shape = FALLING;
  } else if (sample > state.sample + 1 && sample < 0) {
    newstate.shape = RISING;
  } else {
    newstate.shape = state.shape;
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
  // Pardon, c'est sale.
  # define SHAPE_NAME(shp) shp == RISING \
    ? "RISING" \
    : shp == FALLING ? "FALLING" \
    : "UNKNOWN"
  // int shape = STABLE;
  // int sample = 0;
  // int tick[3] = {0,0,0};
  Serial.println("{");
  Serial.write("\"shape\": ");
    Serial.println(SHAPE_NAME(state.shape));
  Serial.write("\"sample\": ");
    Serial.println(state.sample);
  Serial.write("\"ticks\": [ ");
    Serial.write(state.tick[0]);
      Serial.write(", ");
    Serial.write(state.tick[1]);
      Serial.write(", ");
    Serial.write(state.tick[2]);
      Serial.println(" ]");
  Serial.println("}");

}