#include "max7219_matrix.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

// SPI configuration
#define SPI_CHANNEL 0
#define SPI_SPEED   1000000

// MAX7219 register addresses
#define REG_DECODE_MODE   0x09 // Decode mode: BCD code B for 7-segment, or no decode
#define REG_INTENSITY     0x0A // Display intensity / brightness
#define REG_SCAN_LIMIT    0x0B // Scan limit: number of digits/rows to scan
#define REG_SHUTDOWN      0x0C // Shutdown mode: shutdown / normal operation
#define REG_DISPLAY_TEST  0x0F // Display-test register

// Point coordinates structure
typedef struct
{
    uint8_t row;
    uint8_t col;
} Point;

// Direction and level lookup table
static Point dir_level[4][4] =
{
    /* Level 0 */
    {
        {4, 4},
        {4, 4},
        {4, 4},
        {4, 4}
    },

    /* Level 1 */
    {
        {2, 3},
        {3, 2},
        {4, 3},
        {3, 4}
    },

    /* Level 2 */
    {
        {1, 3},
        {3, 1},
        {5, 3},
        {3, 5}
    },

    /* Level 3 */
    {
        {1, 3},
        {3, 1},
        {7, 3},
        {3, 7}
    }
};

static void max7219matrix_send(uint8_t address, uint8_t data)
{
    uint8_t buffer[2];

    buffer[0] = address;
    buffer[1] = data;

    wiringPiSPIDataRW(SPI_CHANNEL, buffer, 2);
}

void max7219matrix_clear(void)
{
    uint8_t i;

    for (i = 1; i <= 8; i++)
    {
        max7219matrix_send(i, 0x00);
    }
}

void max7219matrix_Init(void)
{
    wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
    max7219matrix_send(REG_DISPLAY_TEST, 0x00); // Disable display test mode
    max7219matrix_send(REG_DECODE_MODE, 0x00);  // Disable decode mode (for 8x8 LED matrix)
    max7219matrix_send(REG_INTENSITY, 0x03);    // Set LED brightness
    max7219matrix_send(REG_SCAN_LIMIT, 0x07);   // Scan all 8 rows (digits 0-7)
    max7219matrix_send(REG_SHUTDOWN, 0x01);     // Set to normal operation mode
    
    /* Clear display */
    max7219matrix_clear();
}

void max7219matrix_Display(uint8_t direction, uint8_t level)
{
    uint8_t row;
    uint8_t col;
    uint8_t col_data;

    /* Limit input */
    if (level > 3)
        level = 3;

    if (direction > 3)
        direction = 0;

    /* Get position from lookup table */
    row = dir_level[level][direction].row;
    col = dir_level[level][direction].col;

    /* Clear previous display */
    max7219matrix_clear();

    /*
     * Create 2-column bit mask
     *
     * Example:
     * col = 3
     *
     * 1 << (8 - 3) = 00010000
     * 1 << (7 - 3) = 00001000
     *
     * Result = 00011000
     */
    col_data = (1 << (8 - col)) |
               (1 << (7 - col));

    /* Create 2x2 block */
    max7219matrix_send(row, col_data);
    max7219matrix_send(row + 1, col_data);
}
