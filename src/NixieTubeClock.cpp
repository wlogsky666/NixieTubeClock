#include "NixieTubeClock.h"
#include "Config.h"


NixieTubeClock::NixieTubeClock() : sr_ctrl_(SPI_LATCH_PIN, NUM_TUBES) {}

void NixieTubeClock::init() {
  // Initialize all peripherals
  sr_ctrl_.init();
}