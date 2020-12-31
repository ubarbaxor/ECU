// memo : depeche pfitzer
# define SEC * 1000
// # define MS * 1
# define pot_period 0
# define pot_pulse_width 1

bool verbose = false;
bool tick = false;
// TODO : Add trim. Get rid of String's
// void toLowerCase(char *str) {
//   int i = 0;
//   while (str[i]) {
//     str[i] = tolower(str[i]);
//   }
// }

unsigned int    pulse_width = 512;
unsigned int    period = 1 SEC;
int             pulse_offset = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

int             ledState = LOW;
unsigned long   ms_last = 0;

void led_tick() {
  // total elapsed time
  unsigned long ms_current = millis();
  // position in current period
  const int elapsed = (ms_current - ms_last) % period;
  if (ms_current - ms_last >= period) {
    ms_last = ms_current - elapsed;
    if (verbose || tick)
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
  char *shp; // Shape, for lack of better name
  char *nxt; // Next, high or low
  shp = "N/A"; // will set when calculating shit
  // Default to low, set to high if needed
  nxt = "LOW";

  // Pulse overlaps with period
  // ex: TPPPTPPPTPPPT
  if (pulse_width >= period) {
    // Serial.println("DAFUQ AM I DOING HERE");
    output = HIGH;
    if (verbose) {
      shp = "MAX";
      nxt = "HIGH";
    }
  }
  // Pulse width included in period
  // ex: T.P.T.P.T.P.T
  else if (pulse_end >= pulse_start) {
    if (verbose)
      shp = "INC";
    // We're in da pulse
    if (elapsed >= pulse_start && elapsed <= pulse_end) {
      output = HIGH;
      if (verbose)
        nxt = "HIGH";
    }
  }
  // Split / pulse overlaps period change
  // ex: T..PTP.PTP.PT
  else if (pulse_start >= pulse_end) {
    if (verbose)
      shp = "GAP";
    if (elapsed >= pulse_start || elapsed <= pulse_end) {
      output = HIGH;
      if (verbose)
        nxt = "HIGH";
    }
  }

  if (ledState != output) {
    ledState = output;
    digitalWrite(LED_BUILTIN, ledState);
  
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

void updateAnalog(int pin, int *var, int *last) {
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
  Serial.write('\n');
  print_param(period, "Period");
  print_param(pulse_width, "Pulse width");
  print_param(pulse_offset, "Pulse Offset");
  Serial.write('\n');
}

// TODO: Cleanup this shit
bool eval(String input) {
  char buffer[64]; // fuck my life
  input.getBytes(buffer, 64);

  const char separators[] = " :=";
  int maxTokens = 2;
  String tokens[maxTokens]; // current max tokens = 2

  tokens[0] = strtok(buffer, separators);
  if (tokens[0] == NULL) {
    Serial.println("Invalid command (no words)");
    return false;
  }
  tokens[0] = String(tokens[0]);
  tokens[0].trim();
  tokens[0].toLowerCase();

  int token = 1;
  while (token < maxTokens) {
    tokens[token] = strtok(NULL, separators);
    if (tokens[token] == NULL) { break; }
    else {
      tokens[token] = String(tokens[token]);
      tokens[token].trim();
      tokens[token].toLowerCase();
      token++;
    }
  }
  if (strtok(NULL, separators)) {
    Serial.println("Invalid command (too many words)");
    return false;
  }

  switch (token) {
    case 1:
      // Just one token, test for single word commands
      if (tokens[0] == "params") {
        print_params();
      }
      else if (tokens[0] == "verbose") {
        verbose = !verbose;
      }
      else if (tokens[0] == "ticker" || tokens[0] == "tick") {
        tick = !tick;
      }
      else if (tokens[0] == "period" || tokens[0] == "t") {
        print_param(period, "Period");
      }
      else if (tokens[0] == "pulse_width" || tokens[0] == "width" || tokens[0] == "pulse") {
        print_param(pulse_width, "Pulse Width");
      }
      else if (tokens[0] == "pulse_offset" || tokens[0] == "offset") {
        print_param(pulse_offset, "Pulse Offset");
      }
      return true;
      break;
    case 2:
      if (tokens[0] == "period" || tokens[0] == "t") {
        period = abs(tokens[1].toInt());
        print_param(period, "Period");
      }
    else if (tokens[0] == "pulse_width" || tokens[0] == "width" || tokens[0] == "pulse") {
      pulse_width = abs(tokens[1].toInt());
      print_param(pulse_width, "Pulse Width");
    }
    else if (tokens[0] == "pulse_offset" || tokens[0] == "offset") {
      pulse_offset = tokens[1].toInt();
      print_param(pulse_offset, "Pulse Offset");
    }
    return true;
    // break;
  default:
    return false;
    // break;
  }
  return false;
}

int last_pot_period;
int last_pot_pulse;
int lastLoop = 0;
void loop() {
  // if (verbose || tick) {
  //   int time = millis();
  //   Serial.println(time - lastLoop);
  //   lastLoop = time;
  // }
  led_tick();
  updateAnalog(pot_period, &period, &last_pot_period);
  updateAnalog(pot_pulse_width, &pulse_width, &last_pot_pulse);
  if (Serial.available() > 0) {
    String input = Serial.readString();
    input.trim();
    input.toLowerCase();
    if (!eval(input)) {
      Serial.println("Unrecognized input.");
    }
  }
}
