#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>

// MAX7219 Register Addresses and Modes
#define MAX7219_REG_NOOP        0x00 // No operation
#define MAX7219_REG_DIGIT0      0x01 // Digit 0 data
#define MAX7219_REG_DIGIT1      0x02 // Digit 1 data
#define MAX7219_REG_DIGIT2      0x03 // Digit 2 data
#define MAX7219_REG_DIGIT3      0x04 // Digit 3 data
#define MAX7219_REG_DIGIT4      0x05 // Digit 4 data
#define MAX7219_REG_DIGIT5      0x06 // Digit 5 data
#define MAX7219_REG_DIGIT6      0x07 // Digit 6 data
#define MAX7219_REG_DIGIT7      0x08 // Digit 7 data
#define MAX7219_REG_DECODEMODE  0x09 // Decode mode: 0x00 = No decode, 0xFF = BCD decode for all digits
#define MAX7219_REG_INTENSITY   0x0A // Brightness control: 0x00 (min) - 0x0F (max)
#define MAX7219_REG_SCAN_LIMIT  0x0B // Display scan limit: 0x00 (Digit 0) - 0x07 (All digits)
#define MAX7219_REG_SHUTDOWN    0x0C // Shutdown mode: 0x00 = Shutdown, 0x01 = Normal operation
#define MAX7219_REG_DISPLAYTEST 0x0F // Display test: 0x00 = Normal mode, 0x01 = Display test mode

void max7219_init(uint8_t decode_mode, uint8_t intensity, uint8_t scan_limit);
void max7219_send(uint8_t address, uint8_t data);
void max7219_clear(void);
void max7219_set_intensity(uint8_t intensity);
void max7219_display_number(int num);
void max7219_test(void);

#endif // MAX7219_H
