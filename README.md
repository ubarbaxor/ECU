# ECU Project

Arduino code for sampling automotive sensor signals & triggering actuators.  
Self-educational / prototype / PoC purpose.

## TODO List

- [x] Battery
- [x] Battery 12V to 5V (USB)
- [x] LEDs
- [x] CAS pickup simulator
- [x] Ignition fire
- [x] Ignition coil
- [ ] Injector rack
- [x] Injector fire
- [x] Arduino Period / Offset / pulse
- [x] Arduino check common ground w/ Battery
- [x] Arduino split stuff into files
- [x] Arduino Wrap offset / pulse around period
- [x] Arduino Serial REPL basic (verbose, tick, set params)
- [x] Arduino Serial REPL get rid of String type, use C strings
- [ ] Arduino Serial REPL cleanup (Enum etc...)
- [ ] Arduino Code Cleanup
- [x] README Project description
- [ ] README Sorting around
- [ ] README detail project process

## Hardware

### Computing / UI
Arduino Micro

### Sensors
"2 wire" CAS sensor (From 20NE / ML4.1 harness ?)

### Electronics
Bunch of resistors, nothing too fancy

### Other/Utility
- Breadboard  
    Handy for prototyping

- Bunch of wires
    To go with the breadboard

- 12V-5V DC-DC converter  
    Scrapped from a cigar-plug-to-usb adapter

## Software
This stuff.

Arduino should open serial console for live debugging / configuration.  
From there, REPL commands let one interact with existing settings.

Accepted REPL Commands :
- params - lists parameters names that may be modified.  
  TODO : Proper header and file(s) for parser / REPL
  TODO : More description here. Man ? Help ?

> `mk1.ino` contains init() and loop() declarations
  >> `init()` does whatever needs to be run once, on startup, and shouldn't happen later.  
  >> `loop()` runs as fast as possible, preferably without any interruptions.

> `cas.h` and `cas.cpp` contain the ADC and management code for CAS (Crankshaft Actuation Sensor).
  >> This is handled in a stateful way :  
  >> `cas.h` defines a struct cas_state holding necessary values.  
  >> `cas.cpp` holds a static instance of that state in `cas_tick()`  
  >> `cas_tick()` takes a sample as an argument and compares it to locally held `cas_state`, updates internal singleton and returns a new state.

> `utils.h` defines some utility stuff. Nothing too special, stuff that came up as handy on the way but isn't related to the purpose of this project so much, or stuff I found missing from default libs yet didn't care to include something for.
