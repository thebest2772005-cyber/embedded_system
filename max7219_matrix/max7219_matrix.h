
#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>

void max7219matrix_send(uint8_t address, uint8_t data);
int max7219matrix_Init(void);
void max7219matrix_clear(void);
/*
 * direction:
 * 0 = Up
 * 1 = Right
 * 2 = Down
 * 3 = Left
 *
 * level:
 * 0 = Center
 * 1 = Slight tilt
 * 2 = Medium tilt
 * 3 = Severe tilt
 */
void max7219matrix_Display(uint8_t direction, uint8_t level);
void max7219matrix_test(void);
#endif
