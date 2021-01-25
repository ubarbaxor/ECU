#include "utils.h"
#include "cas.h"

# define BUF_RECV_SIZE 64

// Pin for manually adjusting period and pulse width simulation
# define pot_period 0
# define pot_pulse_width 1

# define cas_pin 2

bool verbose = false;
bool log_tick = false;
bool log_cas = true;

int             ledState = LOW;
unsigned long   ms_last = 0;

unsigned int    pulse_width = 512;
unsigned int    period = 1 SEC;
int             pulse_offset = 0;
int             cas_error = 512;
int             cas_val = 0;
unsigned int    cas_BL = 0;
unsigned int    cas_OR = 0;

cas_state       current_cas;

void setup() {
  unsigned long   ms_current = millis();
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Init.");

  cas_error = cas_calibration(cas_pin, log_cas);
  Serial.write("\nCAS Calibrated : ");
  Serial.println(cas_error);
}

void led_tick() {
  // total elapsed time
  unsigned long ms_current = millis();
  // position in current period
  const int elapsed = (ms_current - ms_last) % period;
  if (ms_current - ms_last >= period) {
    ms_last = ms_current - elapsed;
    if (log_tick)
      Serial.println("Tick.");
  }
  // TODO: Convert potential neg values to positive ?
  // Compute position of pulse
  int o = pulse_offset;
  while (o < 0) { o += period; }
  o %= period;
  // unsigned int pulse_start = pulse_offset % period;
  unsigned int pulse_start = o;
  // if (pulse_start < 0) {
  //   pulse_start = period + pulse_start;
  // }
  int pulse_end = (pulse_width + pulse_offset) % period;
  if (pulse_end < 0) {
    pulse_end = pulse_end + period;
  }

  // Default to low, set HIGH if needed
  int output = LOW;
  // For verbose output
  char *shp = (char *)"N/A"; // Shape, for lack of better name
  char *nxt = (char *)"LOW"; // Next, high or low
  // shp = "N/A"; // will set when calculating shit
  // // Default to low, set to high if needed
  // nxt = "LOW";

  // Pulse overlaps with period
  // ex: TPPPTPPPTPPPT
  if (pulse_width >= period) {
    // Serial.println("DAFUQ AM I DOING HERE");
    output = HIGH;
    if (verbose) {
      shp = (char *)"MAX";
      nxt = (char *)"HIGH";
    }
  }
  // Pulse width included in period
  // ex: T.P.T.P.T.P.T
  else if (pulse_end >= pulse_start) {
    if (verbose)
      shp = (char *)"INC";
    // We're in da pulse
    if (elapsed >= pulse_start && elapsed <= pulse_end) {
      output = HIGH;
      if (verbose)
        nxt = (char *)"HIGH";
    }
  }
  // Split / pulse overlaps period change
  // ex: T..PTP.PTP.PT
  else if (pulse_start >= pulse_end) {
    if (verbose)
      shp = (char *)"GAP";
    if (elapsed >= pulse_start || elapsed <= pulse_end) {
      output = HIGH;
      if (verbose)
        nxt = (char *)"HIGH";
    }
  }

  if (ledState != output) {
    ledState = output;
    // digitalWrite(LED_BUILTIN, ledState);
    digitalWrite(LED_BUILTIN, HIGH);
  
    if (verbose) {
      Serial.print(shp);
      Serial.print(" - ");
      Serial.print("elapsed: ");
      Serial.print(elapsed);
      Serial.print(" - Next : ");
      Serial.println(nxt);
      Serial.print("pulse_start : ");
      Serial.println(pulse_start);
      Serial.print("pulse_end : ");
      Serial.println(pulse_end);
    }
  }
}

void updateAnalog(unsigned int pin, unsigned int *var, unsigned int *last) {
    // Last is to avoid pot overriding digitally set values
    if (last == NULL) { last = var; }
    int pot_read = analogRead(pin);

    // Rounding because of analogRead discrepancies
    // != for special case at zero (don't ask me why...)
    int pot_err = 8;
    if (pot_read != *last && pot_read - pot_err > *last || pot_read + pot_err < *last) {
        *var = pot_read;
        *last = *var;

        if (verbose) {
          Serial.print("pot_read - pot_err : ");
          Serial.println(pot_read - pot_err);

          Serial.print("pot_read + pot_err : ");
          Serial.println(pot_read + pot_err);

          Serial.print("Last : ");
          Serial.println(*last);
          
          Serial.print("pot_read : ");
          Serial.println(pot_read);

          Serial.println("Updating pot value");
        }
    }
}

void print_param(int var, char *name) {
  Serial.write(name);
  Serial.write(" : ");
  Serial.println(var);
}

void print_params() {
  Serial.println();
  print_param(period, (char *)"Period");
  print_param(pulse_width, (char *)"Pulse width");
  print_param(pulse_offset, (char *)"Pulse Offset");
  Serial.write('\n');
}

// TODO: Cleanup this shit
bool eval(char *input) {
  const char  separators[] = " :=";
  int         maxTokens = 2;
  char        *tokens[maxTokens];
  char        *cmd;
  int         arg;

  int token = 0;
  while (token < maxTokens) {
    tokens[token]
      = strtok(token ? NULL : input, separators);

    if (tokens[token] == NULL) { break; }
    else {
      strtrim(tokens[token]);
      // Serial.print("Strtrim result: -");
      // Serial.print(tokens[token]);
      // Serial.println("-");
      // tokens[token].toLowerCase();
      token++;
    }
  }
  if (strtok(NULL, separators)) {
    Serial.println("Invalid command (too many words)");
    return false;
  }

  switch (token) {
    case 1:
      cmd = tokens[0];
      // Just one token, test for single word commands
      if (streq(cmd, "params")) {
        print_params();
      }
      else if (streq(cmd, "verbose")) {
        verbose = !verbose;
        log_tick = verbose;
        log_cas = verbose;
      }
      else if (streq(cmd, "ticker") || streq(cmd, "tick")) {
        log_tick = !log_tick;
      }
      else if (streq(cmd, "log_cas") || streq(cmd, "cas")) {
        log_cas = !log_cas;
      }
      else if (streq(cmd, "period") || streq(cmd, "t")) {
        print_param(period, (char *)"Period");
      }
      else if (streq(cmd, "pulse_width") || streq(cmd, "width") || streq(cmd, "pulse")) {
        print_param(pulse_width, (char *)"Pulse Width");
      }
      else if (streq(cmd, "pulse_offset") || streq(cmd, "offset")) {
        print_param(pulse_offset, (char *)"Pulse Offset");
      } else {
        return false;
      }
      return true;
      break;
    case 2:
      cmd = tokens[0];
      arg = atoi(tokens[1]);

      if (streq(cmd, "period") || streq(cmd, "t")) {
        period = abs(arg);
        print_param(period, (char *)"Period");
      }
      else if (streq(cmd, "pulse_width")
        || streq(cmd, "width")
        || streq(cmd, "pulse")) {
        pulse_width = abs(arg);
        print_param(pulse_width, (char *)"Pulse Width");
      }
      else if (streq(cmd, "pulse_offset")
        || streq(cmd, "offset")) {
        pulse_offset = arg;
        print_param(pulse_offset, (char *)"Pulse Offset");
      } else {
        return false;
      }
      return true;
      break;
    default:
      return false;
  }
  return false;
}

unsigned int last_pot_period;
unsigned int last_pot_pulse;

// int lastLoop = 0;
cas_state    last_state;
void loop() {
  int lastCas = cas_val;
  cas_state new_cas_state = cas_tick(last_state, analogRead(cas_pin) - cas_error);

  cas_val = new_cas_state.sample;
  if (log_cas && lastCas != cas_val) {
    Serial.write("CAS : ");
    Serial.println(cas_val);
    print_cas_state(new_cas_state);
    // Serial.write("1 : ");
    // Serial.println(1);
  }
  // if (tick) {
  //   int time = millis();
  //   Serial.println(time - lastLoop);
  //   lastLoop = time;
  // }
  led_tick();

  updateAnalog(pot_period, &period, &last_pot_period);
  updateAnalog(pot_pulse_width, &pulse_width, &last_pot_pulse);

  if (Serial.available() > 0) {
    // TODO : static buffer to reduce local alloc cycles ?
    char                  input[BUF_RECV_SIZE];
    size_t                read_bytes = Serial.readBytes(input, BUF_RECV_SIZE);

    input[read_bytes] = '\0';

    Serial.write("> ");
    Serial.println(input);
    size_t i = 0;
    while (input[i]) {
      input[i] = tolower(input[i]);
      i++;
    }

    strtrim(input);
    if (!eval(input)) {
      Serial.println("Unrecognized input. Try \"help\"");
    }
  }
}
