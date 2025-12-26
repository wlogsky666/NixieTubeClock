#include "NixieTubeClock.h"
#include "Buzzer.h"
#include "ClockSystem.h"
#include "Config.h"
#include "Logger.h"

NixieTubeClock::NixieTubeClock()
    : nt_ctrl_(), rtc_ctrl_(ClockSystem::RtcController::getInstance()),
      buzzer_(CONFIG::BUZZER_PIN) {}

void NixieTubeClock::init() {
  // Initialize Log System
  Log.init();

  // Initialize all peripherals
  nt_ctrl_.init();
  rtc_ctrl_.init();
  rtc_ctrl_.setCallback(this);
  buzzer_.init();
}

void NixieTubeClock::onTick() { need_update_ = true; }

void NixieTubeClock::run() {
  if (need_update_) {
    auto time = rtc_ctrl_.getTimeData();
    nt_ctrl_.setDigits(time.hour[0], time.hour[1], time.minute[0],
                       time.minute[1]);
    nt_ctrl_.setDot(0, time.second[1] % 2 == 1);
    nt_ctrl_.setDot(1, time.second[1] % 2 == 0);
    nt_ctrl_.setDot(2, time.second[1] % 2 == 1);
    nt_ctrl_.setDot(3, time.second[1] % 2 == 0);

    uint8_t currentHour = time.hour[0] * 10 + time.hour[1];
    uint8_t currentMinute = time.minute[0] * 10 + time.minute[1];
    uint8_t currentSecond = time.second[0] * 10 + time.second[1];
    Log.debug(TAG, "Current hours: %2d, minutes: %2d, seconds: %2d",
              currentHour, currentMinute, currentSecond);
    if (currentSecond >= 56 && currentSecond <= 59) {
      Buzzer::Note note[] = {{440, 100}};
      buzzer_.playShort(note, 1);
    } else if (currentSecond == 0) {
      Buzzer::Note note[] = {{800, 500}};
      buzzer_.playShort(note, 1);
    }

    if (time.second[0] == 0 && time.second[1] == 0) {
      nt_ctrl_.runAntiPoisoning();
    }

    need_update_ = false;
  }

  nt_ctrl_.run();
  buzzer_.run();
}
