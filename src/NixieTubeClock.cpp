#include "NixieTubeClock.h"
#include "Buzzer.h"
#include "ClockSystem.h"
#include "Config.h"
#include "Logger.h"

#define IN_RANGE(val, min, max) ((val) >= (min) && (val) <= (max))
#define IS_TIME_VALID(y, m, d, h, mm, s)                                       \
  (IN_RANGE(y, 2000, 2099) && IN_RANGE(m, 1, 12) && IN_RANGE(d, 1, 31) &&      \
   IN_RANGE(h, 0, 23) && IN_RANGE(mm, 0, 59) && IN_RANGE(s, 0, 59))

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

const Buzzer::Note TICK_DISCRETE[] = {{3000, 3}, {0, 2}, {2500, 3}};
const Buzzer::Note ROLLING[] = {{1175, 6}, {0, 22}};
const Buzzer::Note LOCKING[] = {{880, 60}};
const Buzzer::Note FINISH_CHIME[] = {{880, 50}, {0, 60}, {1175, 200}};
// const Buzzer::Note ROLLING[] = {{1318, 10}, {0, 20}};
// const Buzzer::Note LOCKING[] = {{3136, 15}, {0, 5}, {2093, 80}};
// const Buzzer::Note FINISH_CHIME[] = {{1318, 40}, {1568, 40}, {2093, 40},
// {2637, 150}};

void NixieTubeClock::onTick() { need_update_ = true; }

void NixieTubeClock::checkSerial() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.length() == 0) {
      return;
    }

    char cmd = input.charAt(0);
    cmd = toupper(cmd);

    if (cmd == 'T') {
      // DateTime format: T20260108143000
      if (input.length() >= 15) {
        int year = input.substring(1, 5).toInt();
        int mon = input.substring(5, 7).toInt();
        int day = input.substring(7, 9).toInt();
        int hour = input.substring(9, 11).toInt();
        int minute = input.substring(11, 13).toInt();
        int second = input.substring(13, 15).toInt();

        if (IS_TIME_VALID(year, mon, day, hour, minute, second)) {
          rtc_ctrl_.adjust(DateTime(year, mon, day, hour, minute, second));
        } else {
          Log.error("NIXIE", "Invalid DateTime values!");
        }
      } else {
        Log.error("NIXIE", "Format error. Expected: TYYYYMMDDHHMMSS");
      }
    } else {
      Log.error(TAG, "Error: Unsupported cmd: %c", cmd);
    }
  }
}

void NixieTubeClock::run() {
  if (need_update_) {
    auto time = rtc_ctrl_.getTimeData();
    nt_ctrl_.setDigits(time.hour[0], time.hour[1], time.minute[0],
                       time.minute[1]);

    uint8_t currentHour = time.hour[0] * 10 + time.hour[1];
    uint8_t currentMinute = time.minute[0] * 10 + time.minute[1];
    uint8_t currentSecond = time.second[0] * 10 + time.second[1];
    Log.debug(TAG, "Current hours: %2d, minutes: %2d, seconds: %2d",
              currentHour, currentMinute, currentSecond);

    // Update dots to indicate progress within the current minute.
    // Dots 0-2 light up sequentially every 15 seconds (15s, 30s, 45s).
    // Dot 3 blinks every second to provide a visual heartbeat.
    nt_ctrl_.setDot(0, currentSecond >= 15);
    nt_ctrl_.setDot(1, currentSecond >= 30);
    nt_ctrl_.setDot(2, currentSecond >= 45);
    nt_ctrl_.setDot(3, currentSecond % 2);

    buzzer_.playShort(TICK_DISCRETE);

    if (time.second[0] == 0 && time.second[1] == 0) {
      nt_ctrl_.runAntiPoisoning();
    }

    need_update_ = false;
  }

  NixieTube::Event event = nt_ctrl_.run();
  switch (event) {
  case NixieTube::Event::DIGIT_LOCK: {
    buzzer_.playShort(LOCKING, Buzzer::Priority::P_MEDIUM);
    break;
  }
  case NixieTube::Event::ROLLING: {
    buzzer_.playShort(ROLLING, Buzzer::Priority::P_LOW);
    break;
  }
  case NixieTube::Event::ALL_SETTLED: {
    buzzer_.playShort(FINISH_CHIME, Buzzer::Priority::P_HIGH);
    break;
  }
  }

  buzzer_.run();
  checkSerial();
}
