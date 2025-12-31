#pragma once
#include "Logger.h"
#include <RTClib.h>

namespace ClockSystem {

struct TimeData {
  uint8_t hour[2];
  uint8_t minute[2];
  uint8_t second[2];
};

class ITickCallback {
  friend class RtcController;

private:
  /**
   * @brief Callback function type for RTC tick events.
   * @note Since this is called from an ISR, the function MUST NOT use
   * blocking code, I2C (Wire), or Serial.print.
   */
  virtual void onTick() = 0;
};

class RtcController {
public:
  static RtcController &getInstance() {
    static RtcController instance;
    return instance;
  }

  RtcController(const RtcController &) = delete;
  void operator=(const RtcController &) = delete;

  /**
   * @brief Initialize DS3231 and attach hardware interrupt for 1Hz pulse.
   */
  void init();

  /**
   * @brief Register a listener for the 1Hz hardware pulse.
   * @param cb Function pointer to be executed within ISR context.
   */
  void setCallback(ITickCallback *cb);

  /**
   * @brief Returns a reference to the cached time data.
   * @details If a new tick has occurred, it automatically triggers update()
   * to sync with RTC IC before returning data.
   */
  const TimeData getTimeData();

  /**
   * @brief Synchronizes internal TimeData with the RTC IC via I2C.
   * @note Should be called in the main loop context, not inside an ISR.
   */
  void update();

private:
  RtcController() = default;

  const char *TAG = "RtcController";
  RTC_DS3231 rtc_;
  TimeData now_;
  ITickCallback *cb_ = nullptr;

  volatile bool tick_flag_;
  void _handleInterrupt();
  static void _isrWrapper();
};

} // namespace ClockSystem