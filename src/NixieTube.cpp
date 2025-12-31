#include "NixieTube.h"
#include "Config.h"
#include "Logger.h"

namespace NixieTube {

Controller::Controller()
    : sr_ctrl_(CONFIG::SPI_LATCH_PIN, CONFIG::NUM_TUBES),
      num_tubes_(CONFIG::NUM_TUBES) {}

void Controller::init() {
  sr_ctrl_.init();
  for (int i = 0; i < num_tubes_; ++i) {
    pinMode(CONFIG::DOT_PINS[i], OUTPUT);
    digitalWrite(CONFIG::DOT_PINS[i], LOW);
  }

  Log.info(TAG, "Initialization Ready, num_tubes = %d.", num_tubes_);
}

void Controller::clear() {
  for (int i = 0; i < num_tubes_; ++i) {
    digit_state_[i] = 10;
    dot_state_[i] = 0;
    _setDot(i, dot_state_[i]);
  }

  _setDigits(digit_state_);
}

static void generateRandomDigits(uint8_t r[], int num_tubes) {
  for (int i = 0; i < num_tubes; i++) {
    r[i] = random(0, 10);
  }
}

Event Controller::run() {
  Event event = NixieTube::Event::NONE;

  if (rolling_step_ >= 0) {
    // Perform anti poisoning process
    unsigned long now = millis();
    if (now - last_rolling_time_ >= CONFIG::ROLLING_INTERVAL) {
      generateRandomDigits(rolling_digit_state_, num_tubes_);
      last_rolling_time_ = now;
      for (uint8_t i = 0; i < rolling_step_; i++) {
        rolling_digit_state_[i] = digit_state_[i];
      }
      _setDigits(rolling_digit_state_);
      for (int i = 0; i < num_tubes_; i++) {
        _setDot(i, dot_state_[i]);
      }
      event = Event::ROLLING;
    }

    if (now - last_lock_time_ >= CONFIG::ROLLING_LOCK_INTERVAL[rolling_step_]) {
      rolling_step_++;
      last_lock_time_ = now;
      is_state_changed_ = true;

      if (rolling_step_ >= num_tubes_) {
        rolling_step_ = -1;
        event = NixieTube::Event::ALL_SETTLED;
      } else {
        // Event::DIGIT_LOCK has higher priority than Event::ROLLING
        event = Event::DIGIT_LOCK;
      }
    }
  } else if (is_state_changed_) {
    _setDigits(digit_state_);
    for (int i = 0; i < num_tubes_; i++) {
      _setDot(i, dot_state_[i]);
    }
    is_state_changed_ = false;
    event = Event::NONE;
  }

  return event;
}

void Controller::runAntiPoisoning() {
  Log.debug(TAG, "Running Auto Poisoning Process");
  generateRandomDigits(rolling_digit_state_, num_tubes_);
  rolling_step_ = 0;
  last_lock_time_ = millis();
  last_rolling_time_ = millis();
}

void Controller::setDot(uint8_t index, bool en) {
  if (index >= num_tubes_) {
    return;
  }

  dot_state_[index] = en;
  is_state_changed_ = true;
}

void Controller::_setDigits(const uint8_t *ds) { sr_ctrl_.transfer(ds); }

void Controller::_setDot(int index, bool en) {
  digitalWrite(CONFIG::DOT_PINS[index], en ? HIGH : LOW);
}

} // namespace NixieTube