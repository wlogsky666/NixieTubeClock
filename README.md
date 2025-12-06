# NixieTubeClock
The NixieTubeClock project implements hardware driver logic for a digital clock utilizing Nixie tubes IN12 as the primary display.

## Hardware Requirements
To run this code, you will need the following key components:
- Microcontroller: Arduino Uno, Nano, or equivalent MCU.
- Nixie Tubes: e.g., IN-12, IN-14, etc. (2 or 4 tubes recommended).
- Nixie Tubes Driver ICs: SN74141 or K155ID1 (1 per tube).
- Shift Registers: 74HC595 (1 per 2 tubes, due to 8-bit output).
- Power Supply: A dedicated 170V DC supply for the Nixie anodes, and a 5V DC supply for the logic circuits.

## Wiring and Setup
The project uses SPI communication to cascade data through the 74HC595 shift registers.
1. SPI Connection: Connect the SCK and LATCH pins from the MCU to all 74HC595 ICs.
2. SER Cascading: Connect the SER(MOSI) to the first 74HC595 IC, and the QH' pin output of the first 74HC595 to the SER input of the next 74HC595 in the chain.
4. BCD Decoder: The parallel outputs of each 74HC595 are split to drive two BCD decoders:
    * **First Digit (Low):** QA, QB, QC, QD outputs connect to the A, B, C, D inputs of the first Nixie Driver IC.
    * **Second Digit (High):** QE, QF, QG, QH outputs connect to the A, B, C, D inputs of the second Nixie Driver IC.
