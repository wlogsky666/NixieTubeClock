#include "NixieTube.h"
#include "Config.h"

NixieTube::Controller::Controller()
    : sr_ctrl_(CONFIG::SPI_LATCH_PIN, CONFIG::NUM_TUBES),
      num_tubes_(CONFIG::NUM_TUBES) {}

void NixieTube::Controller::init() {
  sr_ctrl_.init();
  for (int i = 0; i < num_tubes_; ++i) {
    pinMode(CONFIG::DOT_PINS[i], OUTPUT);
    digitalWrite(CONFIG::DOT_PINS[i], LOW);
  }
}

void NixieTube::Controller::clear() {
  const uint8_t zeros[num_tubes_] = {0};
  setDigits(zeros);
  for (int i = 0; i < num_tubes_; ++i) {
    setDot(i, 0);
  }
}

static void generateRandomDigits(uint8_t r[], int num_tubes) {
  for (int i = 0; i < num_tubes; i++) {
    r[i] = random(0, 10);
  }
}

void NixieTube::Controller::runAntiPoisoning(int cycles = 1) {
  int roll_duration_ms = 200;
  int fade_duration_ms = 1500;
  int lock_delay_duration_ms = 500;
  int total_animation_time = roll_duration_ms + fade_duration_ms;
  for (int c = 0; c < cycles; ++c) {
    long start_time = millis();
    int current_delay = 10;

    // FAST FLICKER AND DECELERATION PHASE
    uint8_t ds[num_tubes_] = {0};
    while (millis() - start_time < total_animation_time) {

      long elapsed_time = millis() - start_time;

      // Check if we are in the deceleration phase
      if (elapsed_time > roll_duration_ms) {
        long fade_elapsed = elapsed_time - roll_duration_ms;
        current_delay = map(fade_elapsed, 0, fade_duration_ms, 10, 150);
      }

      // Generate new random digits for all tubes
      generateRandomDigits(ds, num_tubes_);
      _setDigits(ds);
      for (int i = 0; i < num_tubes_; ++i) {
        _setDot(i, random(0, 2));
      }

      delay(current_delay);
    }

    // SEQUENTIAL LOCKING (Digit-by-Digit Stop)
    for (int i = 0; i < num_tubes_; i++) {
      long lock_cycle_start = millis();
      while (millis() - lock_cycle_start < lock_delay_duration_ms) {
        generateRandomDigits(ds, num_tubes_);

        // Re-copy the digits from digit states
        for (int j = 0; j <= i; j++) {
          ds[j] = digit_state_[j];
        }

        _setDigits(ds);
        _setDot(i, dot_state_[i]);
        delay(current_delay);
      }
    }

    _setDigits(digit_state_);
  }
}

void NixieTube::Controller::setDot(uint8_t index, bool en) {
  if (index >= num_tubes_) {
    Serial.print("Invalid index of tubes");
    return;
  }

  dot_state_[index] = en;
  _setDot(index, en);
}

void NixieTube::Controller::_setDigits(const uint8_t *ds) {
  sr_ctrl_.transfer(ds);
}

void NixieTube::Controller::_setDot(int index, bool en) {
  digitalWrite(CONFIG::DOT_PINS[index], en ? HIGH : LOW);
}