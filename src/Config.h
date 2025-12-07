// ====================================================================
// HARDWARE CONFIGURATION
// ====================================================================

// Uncomment the definition for the board you are currently using.
// This will automatically set the default Hardware SPI MOSI and CLK pins.
// #define BOARD_LINKIT_7688
// #define BOARD_LINKIT_7697

// ====================================================================
// Settings
// ====================================================================
const int NUM_TUBES = 2;

// ====================================================================
// SPI PINS ASSIGNMENT
// ====================================================================
// If a board is defined above, MOSI and SCK pins are automatically configured
// according to the hardware layout.
#if defined(__AVR_ATmega328P__)
const int SPI_MOSI = 11;
const int SPI_SCK = 13;
const int SPI_LATCH_PIN = 10;
#elif defined(__LINKIT_7688__)
const int SPI_MOSI = 21;
const int SPI_SCK = 23;
const int SPI_LATCH_PIN = 24;
#elif defined(__LINKIT_7697__)
const int SPI_MOSI = 11;
const int SPI_SCK = 13;
const int SPI_LATCH_PIN = 10;
#else
#error "The dev board is unspecified or not supported！"
#endif
