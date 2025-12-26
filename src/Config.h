#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

namespace CONFIG {

// ====================================================================
// Settings
// ====================================================================
/**
 * @brief Number of Nixie tubes in the system.
 */
const uint8_t NUM_TUBES = 4;

/**
 * @brief Serial port baud rate
 */
const uint32_t BAUD_RATE = 115200;

/**
 * @brief Interval between each digit shuffle during anti-poisoning (ms).
 */
const uint16_t ROLLING_INTERVAL = 10;

/**
 * @brief Delay duration before locking each successive tube (ms).
 */
const uint16_t ROLLING_LOCK_INTERVAL[NUM_TUBES] = {300, 300, 500, 500};

// ====================================================================
// SPI Pins Assignment
// ====================================================================
// MOSI and SCK pins are automatically configured according to the hardware
// layout if HW SPI is being used.
const uint8_t SPI_MOSI = 11;
const uint8_t SPI_SCK = 13;
const uint8_t SPI_LATCH_PIN = 10;

// ====================================================================
// Nixie Tube Decimal Points Pins Assignment (Dots)
// ====================================================================
/**
 * @brief Array of digital pins used to control the Nixie tube decimal points.
 *
 * Each pin is connected to the BASE of an MPSA42 NPN transistor.
 */
const uint8_t DOT_PINS[NUM_TUBES] = {4, 5, 6, 7};

// ====================================================================
// RTC DS3032 PIN ASSIGNMENT
// ====================================================================
/**
 * @brief Interrupt pin to trigger time update.
 */
const uint8_t RTC_INTR_PIN = 2;

// ====================================================================
// Buzzer PIN ASSIGNMENT
// ====================================================================
/**
 * @brief PWM output pin for buzzer
 */
const uint8_t BUZZER_PIN = 9;

} // namespace CONFIG

#endif // SHIFT_REGISTER_DRIVER_H