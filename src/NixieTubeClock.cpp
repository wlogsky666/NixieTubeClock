#include "NixieTubeClock.h"
#include "Config.h"
#include "Logger.h"

NixieTubeClock::NixieTubeClock() : nt_ctrl_() {}

void NixieTubeClock::init() {
  // Initialize Log System
  Log.init();

  // Initialize all peripherals
  nt_ctrl_.init();
}

void NixieTubeClock::test() {
  nt_ctrl_.setDigits(random(0, 10), random(0, 10), random(0, 10),
                     random(0, 10));
  delay(2000);
  nt_ctrl_.runAntiPoisoning();
  delay(3000);
}
