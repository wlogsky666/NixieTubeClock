#include "src/NixieTubeClock.h"

NixieTubeClock *ntc;

void setup() {
  ntc = new NixieTubeClock();
  ntc->init();
}

void loop() {}
