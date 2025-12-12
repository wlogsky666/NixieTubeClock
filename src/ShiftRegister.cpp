/**
 * * CONTROL SYSTEM:
 There are 2 74HC595 shift registers cascaded and connected to the dev
 board's SPI pins. These 2 shift registers control 4 K155ID1 BCD-to-Decimal
 Decoder ICs, which drive four Nixie Tubes (Max 4 digits display).

 * * DATA ORGANIZATION (16-bit Total):
 * * 1. Data Flow:
  - The data is shifted serially into the chain (LSB or MSB first, depending on
    SPI setting).
  - The 16 bits are conceptually arranged as [Chip 1 Data (High Byte)] [Chip 2
    Data (Low Byte)].

 * * 2. Chip Mapping Logic:
  - Each 74HC595 IC manages the data transfer for two K155ID1 decoders (two
    Nixie Tubes).
  - QA - QD pins of the 74HC595 control the BCD inputs (A-D) for the **Lower
    Digit** (Units).
  - QE - QH pins of the 74HC595 control the BCD inputs (A-D) for the **Higher
    Digit** (Tens).

 * * PIN-TO-PIN MAPPING (74HC595 Output to K155ID1 Input):
   (1 and 2 are the index of 2 K155ID1 chips controlled by 1 74HC595)
   QA - A1 (Lower Digit)
   QB - B1
   QC - C1
   QD - D1
   -------------------
   QE - A2 (Higher Digit)
   QF - B2
   QG - C2
   QH - D2

 * * BCD EXAMPLE & SIGNAL CONVERSION:
    To display '71' in a group of two Nixie Tubes:
    - High Digit ('7'): BCD is 0111 (QH-QE)
    - Low Digit ('1'): BCD is 0001 (QD-QA)

 * * The 8-bit data needed for IC is 0b01110001 when sending with MSB Fisrt.
 * * Resulting 8-bit Data: 0b01110001 (0x71)
  -------------------------------------------
  74HC595  | QA QB QC QD   |   QE QF QG QH
  -------  | 1  0  0  0 (1)|   1  1  1  0 (7)
  K155ID1  | A  B  C  D    |   A  B  C  D
  -------  | (Low Digit)   |   (High Digit)
  -------------------------------------------
 * * (Note: The BCD mapping depends on how the pins A/B/C/D are wired.

 * * K155ID1 TRUTH TABLE (Active Low Output 0-9):
   | A | B | C | D | Decimal Output |
   |:-:|:-:|:-:|:-:|:--------------:|
   | L | L | L | L | 0              |
   | H | L | L | L | 1              |
   | L | H | L | L | 2              |
   | H | H | L | L | 3              |
   | L | L | H | L | 4              |
   | H | L | H | L | 5              |
   | L | H | H | L | 6              |
   | H | H | H | L | 7              |
   | L | L | L | H | 8              |
   | H | L | L | H | 9              |
   | ? | ? | ? | H | NONE/Blanking  |
*/

#include <Arduino.h>
#include <SPI.h>

#include "ShiftRegister.h"

// Several dev boards does not support hardware-accelerated bit reversal.
// This 8-bit Bit Reversal logic is CPU-intensive and might introduce
// noticeable delays on time-sensitive tasks.
// Its highly recommended to keep BIT_ORDER = MSBFIRST.
const auto BIT_ORDER = MSBFIRST;
const auto SPI_SPEED = 1000000; // 1 MHz

ShiftRegister::Controller::Controller(uint8_t latch_pin, uint8_t num_tubes)
    : latch_pin_(latch_pin), mosi_pin_(0), sck_pin_(0), num_tubes_(num_tubes),
      use_sw_spi_(false) {}

ShiftRegister::Controller::Controller(uint8_t latch_pin, uint8_t mosi_pin,
                                      uint8_t sck_pin, uint8_t num_tubes)
    : latch_pin_(latch_pin), mosi_pin_(mosi_pin), sck_pin_(sck_pin),
      num_tubes_(num_tubes), use_sw_spi_(true) {}

uint8_t ShiftRegister::Controller::_convertToByte(uint8_t high, uint8_t low) {
  // Convert 2 digits into 1 bytes for 74HC595 IC
  // If digit is over range, use 9 as default
  high = min(high, 9);
  low = min(low, 9);

  if (BIT_ORDER == MSBFIRST) {
    return (high << 4) | low;
  } else if (BIT_ORDER == LSBFIRST) {
    uint8_t lsb = (high << 4) | low;
    lsb = ((lsb & 0xf0) >> 4) | ((lsb & 0x0f) << 4);
    lsb = ((lsb & 0xcc) >> 2) | ((lsb & 0x33) << 2);
    lsb = ((lsb & 0xaa) >> 1) | ((lsb & 0x55) << 1);
    return lsb;
  }

  return 0;
}

void ShiftRegister::Controller::_sendByte(uint8_t digit1, uint8_t digit2) {
  if (use_sw_spi_) {
    shiftOut(mosi_pin_, sck_pin_, BIT_ORDER, _convertToByte(digit1, digit2));
  } else {
    SPI.transfer(_convertToByte(digit1, digit2));
  }
}

void ShiftRegister::Controller::init() {
  pinMode(latch_pin_, OUTPUT);
  digitalWrite(latch_pin_, LOW);

  if (use_sw_spi_) {
    pinMode(mosi_pin_, OUTPUT);
    pinMode(sck_pin_, OUTPUT);
    digitalWrite(mosi_pin_, LOW);
    digitalWrite(sck_pin_, LOW);
  } else {
    SPI.begin();
    SPI.beginTransaction(SPISettings(SPI_SPEED, BIT_ORDER, SPI_MODE0));
  }
}

void ShiftRegister::Controller::transfer(const uint8_t *digits) {
  uint8_t num_tubes = num_tubes_;

  digitalWrite(latch_pin_, LOW);
  if (num_tubes & 1) {
    // The system handles the odd number of tubes by pairing the last digit
    // with a zero for padding. The last digit occupies the LOW 4 bits of
    // the final byte.
    _sendByte(digits[num_tubes - 1], 0);
    num_tubes -= 1;
  }

  while (num_tubes > 0) {
    _sendByte(digits[num_tubes - 2], digits[num_tubes - 1]);
    num_tubes -= 2;
  }

  digitalWrite(latch_pin_, HIGH);
}