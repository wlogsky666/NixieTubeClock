#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include <Arduino.h>

namespace ShiftRegister {

class Controller {
public:
  Controller(uint8_t latch_pin, uint8_t num_tubes);
  Controller(uint8_t latch_pin, uint8_t mosi_pin, uint8_t sck_pin,
             uint8_t num_tubes);

  /**
   * @brief Init Shift Register Controller.
   */
  void init();

  /**
   * @brief Transfer digits to the Shift Register.
   */
  void transfer(const uint8_t *digits);

private:
  const uint8_t latch_pin_;
  const uint8_t mosi_pin_;
  const uint8_t sck_pin_;
  const uint8_t num_tubes_;
  const bool use_sw_spi_;

  void _sendByte(uint8_t, uint8_t);
  uint8_t _convertToByte(uint8_t, uint8_t);
};
} // namespace ShiftRegister

#endif // SHIFT_REGISTER_DRIVER_H
