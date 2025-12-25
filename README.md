# NixieTubeClock
The NixieTubeClock project implements hardware driver logic for a digital clock utilizing Nixie tubes IN12 as the primary display.

## Hardware Requirements
To run this code, you will need the following key components:
- Microcontroller: Arduino Uno, Nano, or equivalent MCU.
- Nixie Tubes: e.g., IN-12, IN-14, etc. (4 or 6 tubes recommended).
- Nixie Tubes Driver ICs: SN74141 or K155ID1 (1 per tube).
- Shift Registers: 74HC595 (1 per 2 tubes, due to 8-bit output).
- Power Supply: A dedicated 170V DC supply for the Nixie anodes, 8V DC supply for micro controller, and a 5V DC supply for the logic circuits.
- RTC: DS3032 IC and its battery supply.
- Others: MPSA42 ICs (1 per tube), Buzzer, 20K/10K resistors and 100uF/100nf capacitors.

## Wiring and Setup
The project uses SPI communication to cascade data through the 74HC595 shift registers.
1. SPI Connection: Connect the SCK and LATCH pins to all 74HC595 ICs.
2. SER Cascading: Connect the SER(MOSI) to the first 74HC595 IC, and the QH' pin output of the first 74HC595 to the SER input of the next 74HC595 in the chain.
4. BCD Decoder: The parallel outputs of each 74HC595 are split to drive two BCD decoders:
    * **First Digit (Low):** QA, QB, QC, QD outputs connect to the A, B, C, D inputs of the first Nixie Driver IC.
    * **Second Digit (High):** QE, QF, QG, QH outputs connect to the A, B, C, D inputs of the second Nixie Driver IC.
    * More Digit: ...
5. I2C Connection: Connect RTC DS3231 SDA to A4 and SCL to A5. The SQW (IRQ) is connected to D2 (Arduino IRQ pin).
6. Buzzer: Connect to D9


<img width="1190" height="845" alt="SCH_Schematic1_1-P1_2025-12-25 (1)" src="https://github.com/user-attachments/assets/b46e6157-7f21-436c-b92d-39e7639a39b7" />
