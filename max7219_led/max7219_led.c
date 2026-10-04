#include "max7219.h"
#include <wiringPiSPI.h>
#include <unistd.h>
#define SPI_CHANNEL 0
#define SPI_SPEED   1000000

// Send data to MAX7219
void max7219_send(uint8_t address, uint8_t data) {
    uint8_t buffer[2] = {address, data};
    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);
}

// Initialize MAX7219
void max7219_init(uint8_t decode_mode, uint8_t intensity, uint8_t scan_limit) {
    wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
    
    max7219_send(MAX7219_REG_SHUTDOWN, 0x01); // Normal operation
    max7219_send(MAX7219_REG_DISPLAYTEST, 0x00); // No display test
    max7219_send(MAX7219_REG_SCAN_LIMIT, scan_limit & 0x07); // Set scan limit (0-7)
    max7219_send(MAX7219_REG_DECODEMODE, decode_mode); // Set decode mode (0x00 for no decode, 0xFF for BCD decode)
    max7219_set_intensity(intensity); // Set intensity (brightness) (0-15)
    max7219_clear();
}

// Clear the display
void max7219_clear(void) {
    for (uint8_t i = MAX7219_REG_DIGIT0; i <= MAX7219_REG_DIGIT7; i++) {
        max7219_send(i, 0x00);
    }
}

// Set the intensity (brightness) of the display
void max7219_set_intensity(uint8_t intensity) {
    max7219_send(MAX7219_REG_INTENSITY, intensity & 0x0F);
}

// Display a number on the display
void max7219_display_number(int num) {
    for (uint8_t i = MAX7219_REG_DIGIT0; i <= MAX7219_REG_DIGIT7; i++) {
        max7219_send(i, num % 10);
        num /= 10;
        if (num == 0) 
            break;
    }
}
void max7219_test(void){
    max7219_send(MAX7219_REG_DISPLAYTEST, 0x01); // Enable display test
    sleep(2); // Wait for 2 seconds
    max7219_send(MAX7219_REG_DISPLAYTEST, 0x00); // Disable display test
}
