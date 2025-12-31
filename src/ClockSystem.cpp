#include <util/atomic.h>

#include "ClockSystem.h"

namespace ClockSystem {

void RtcController::init() {
  Log.info(TAG, "Searching RTC module...");
  if (!rtc_.begin()) {
    Log.error(TAG, "Could not find RTC module");
    return;
  }

  if (rtc_.lostPower()) {
    Log.warn(TAG, "RTC lost power, resetting to compile time.");
    rtc_.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(CONFIG::RTC_INTR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CONFIG::RTC_INTR_PIN),
                  RtcController::_isrWrapper, FALLING);

  rtc_.disable32K();
  rtc_.clearAlarm(1);
  rtc_.clearAlarm(2);

  // Enable 1Hz IRQ output
  rtc_.writeSqwPinMode(DS3231_SquareWave1Hz);

  update();
  Log.info(TAG, "RTC System initialized.");
}

void RtcController::setCallback(ITickCallback *cb) { cb_ = cb; }

const TimeData RtcController::getTimeData() {
  if (tick_flag_)
    update();

  return now_;
}

void RtcController::update() {
  bool should_update = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    if (tick_flag_) {
      should_update = true;
      tick_flag_ = false;
    }
  }

  if (should_update) {
    DateTime dt = rtc_.now();
    now_.hour[0] = dt.hour() / 10;
    now_.hour[1] = dt.hour() % 10;
    now_.minute[0] = dt.minute() / 10;
    now_.minute[1] = dt.minute() % 10;
    now_.second[0] = dt.second() / 10;
    now_.second[1] = dt.second() % 10;
  }
}

void RtcController::_handleInterrupt() {
  tick_flag_ = true;
  if (cb_)
    cb_->onTick();
}

void ClockSystem::RtcController::_isrWrapper() {
  getInstance()._handleInterrupt();
}

} // namespace ClockSystem