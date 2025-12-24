#ifndef NIXIE_TUBE_H
#define NIXIE_TUBE_H

#include <Arduino.h>

#include "Config.h"
#include "Logger.h"
#include "ShiftRegister.h"

namespace NixieTube {

class Controller {
public:
  Controller();

  /**
   * @brief Init Shift Register and Dot Controller.
   */
  void init();

  /**
   * @brief Reset all digits and dots to the invisible state.
   */
  void clear();

  /**
   * @brief Performs the Cathode Anti-Poisoning routine.
   */
  void runAntiPoisoning(int cycles = 1);

  /**
   * @brief Set dot state of tube.
   */
  void setDot(uint8_t index, bool en);

  /**
   * @brief Set digit state of tube.
   */
  template <typename... Args> void setDigits(Args... digits) {
    if (sizeof...(Args) != num_tubes_) {
      Log.error(TAG, "Num of tubes %d not matched!", num_tubes_);
      return;
    }

    const uint8_t ds[] = {(uint8_t)digits...};
    _setDigits(ds);
    for (int i = 0; i < num_tubes_; ++i) {
      digit_state_[i] = ds[i];
    }
  }

private:
  const char *TAG = "NixieTube";
  ShiftRegister::Controller sr_ctrl_;
  const uint8_t num_tubes_;
  bool dot_state_[CONFIG::NUM_TUBES] = {0};
  uint8_t digit_state_[CONFIG::NUM_TUBES] = {0};

  void _setDigits(const uint8_t *ds);
  void _setDot(int index, bool en);
};
} // namespace NixieTube

#endif // NIXIE_TUBE_H
