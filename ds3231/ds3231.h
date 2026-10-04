#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

// I2C 7-bit Device Address
#define DS3231_ADDRESS   0x68 

// Register Addresses
#define SECONDS_ADDRESS  0x00        
#define MINUTES_ADDRESS  0x01
#define HOURS_ADDRESS    0x02
#define DAYS_ADDRESS     0x03
#define DATE_ADDRESS     0x04
#define MONTHS_ADDRESS   0x05
#define YEARS_ADDRESS    0x06

// Display Mode Macros
#define DS3231_MODE_24H  0
#define DS3231_MODE_12H  1

// AM/PM Indicator (0 = AM, 1 = PM; valid when in 12h mode)
extern uint8_t is_pm;

// Time and Date Data Structure
typedef struct {
    uint8_t hour;   // 0-23 (in 24h mode) or 1-12 (in 12h mode)
    uint8_t min;    // 0-59
    uint8_t sec;    // 0-59
    uint8_t day;    // Day of week: 1-7 (1 = Sunday)
    uint8_t date;   // Day of month: 1-31
    uint8_t month;  // 1-12
    uint8_t year;   // 00-99 (representing 2000-2099)
} DS3231_Time;

// Public Function Declarations
void ds3231_init(void);
void ds3231_set_mode(uint8_t mode);
int ds3231_get_time(DS3231_Time *time_data);
void ds3231_sync_system_time(void);

#endif /* DS3231_H */
