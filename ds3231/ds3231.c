#include "ds3231.h"
#include <wiringPiI2C.h>
#include <time.h>
#include <stdio.h>

#define MODE_24H 0
#define MODE_12H 1

static int fd = -1;
static uint8_t ds3231_mode = MODE_24H; // Default mode: 0 = 24h, 1 = 12h
uint8_t is_pm = 0;                     // 0 = AM, 1 = PM (valid only when MODE_12H is active)

// Convert BCD to decimal
uint8_t bcd_to_dec(int hex) {
    return (uint8_t)(((hex >> 4) * 10) + (hex & 0x0F));
}

// Convert decimal to BCD
uint8_t dec_to_bcd(int dec) {
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}

// Configure time display mode (0: 24h, 1: 12h)
void ds3231_set_mode(uint8_t mode) {
    ds3231_mode = mode ? MODE_12H : MODE_24H;
}

// Initialize DS3231 over I2C
void ds3231_init(void) {
    fd = wiringPiI2CSetup(DS3231_ADDRESS);
    if (fd < 0) {
        printf("Failed to initialize DS3231\n");
    } else {
        printf("DS3231 initialized successfully\n");
    }
}

// Read date and time from DS3231
int ds3231_get_time(DS3231_Time *t) {
    if (fd < 0 || t == NULL) return -1;

    int raw_sec   = wiringPiI2CReadReg8(fd, SECONDS_ADDRESS);
    int raw_min   = wiringPiI2CReadReg8(fd, MINUTES_ADDRESS);
    int raw_hour  = wiringPiI2CReadReg8(fd, HOURS_ADDRESS);
    int raw_day   = wiringPiI2CReadReg8(fd, DAYS_ADDRESS);
    int raw_date  = wiringPiI2CReadReg8(fd, DATE_ADDRESS);
    int raw_month = wiringPiI2CReadReg8(fd, MONTHS_ADDRESS);
    int raw_year  = wiringPiI2CReadReg8(fd, YEARS_ADDRESS);

    // Verify all I2C read operations succeeded
    if (raw_sec < 0 || raw_min < 0 || raw_hour < 0 || 
        raw_day < 0 || raw_date < 0 || raw_month < 0 || raw_year < 0) 
    {
        return -1;
    }

    t->sec   = bcd_to_dec(raw_sec & 0x7F);
    t->min   = bcd_to_dec(raw_min & 0x7F);
    t->day   = bcd_to_dec(raw_day & 0x07);
    t->date  = bcd_to_dec(raw_date & 0x3F);
    t->month = bcd_to_dec(raw_month & 0x1F); // Bit 7 is Century flag; mask to retain 1-12
    t->year  = bcd_to_dec(raw_year & 0xFF);

    // Step 1: Decode physical register into standard 24-hour baseline
    uint8_t hour24 = 0;
    if (raw_hour & 0x40) {
        // Hardware register is currently in 12-hour format
        uint8_t hw_pm = (raw_hour & 0x20) ? 1 : 0;
        uint8_t h12   = bcd_to_dec(raw_hour & 0x1F);

        if (h12 == 12) {
            hour24 = hw_pm ? 12 : 0;
        } else {
            hour24 = h12 + (hw_pm ? 12 : 0);
        }
    } 
    else {
        // Hardware register is in 24-hour format
        hour24 = bcd_to_dec(raw_hour & 0x3F);
    }

    // Step 2: Format output into t->hour based on selected mode
    if (ds3231_mode == MODE_12H) {
        is_pm = (hour24 >= 12) ? 1 : 0;

        if (hour24 == 0) {
            t->hour = 12; // 00:00 is 12 AM
        } else if (hour24 > 12) {
            t->hour = hour24 - 12;
        } else {
            t->hour = hour24;
        }
    } else {
        // 24-hour mode (range: 0-23)
        t->hour = hour24;
    }

    return 0;
}

// Synchronize system time from host OS to DS3231 in 24-hour format
void ds3231_sync_system_time(void) {
    if (fd < 0) return;

    time_t rawtime;
    struct tm *info;
    time(&rawtime);
    info = localtime(&rawtime);

    printf("Syncing system time to DS3231: %04d-%02d-%02d %02d:%02d:%02d\n",
           info->tm_year + 1900, info->tm_mon + 1, info->tm_mday,
           info->tm_hour, info->tm_min, info->tm_sec);

    wiringPiI2CWriteReg8(fd, SECONDS_ADDRESS, dec_to_bcd(info->tm_sec)  & 0x7F);
    wiringPiI2CWriteReg8(fd, MINUTES_ADDRESS, dec_to_bcd(info->tm_min)  & 0x7F);
    wiringPiI2CWriteReg8(fd, HOURS_ADDRESS,   dec_to_bcd(info->tm_hour) & 0x3F); // Bit 6 = 0 enforces 24h mode
    wiringPiI2CWriteReg8(fd, DAYS_ADDRESS,    dec_to_bcd(info->tm_wday + 1) & 0x07); // tm_wday: 0(Sun)-6 -> DS3231: 1-7
    wiringPiI2CWriteReg8(fd, DATE_ADDRESS,    dec_to_bcd(info->tm_mday) & 0x3F);
    wiringPiI2CWriteReg8(fd, MONTHS_ADDRESS,  dec_to_bcd(info->tm_mon + 1) & 0x1F); // tm_mon: 0-11 -> DS3231: 1-12
    wiringPiI2CWriteReg8(fd, YEARS_ADDRESS,   dec_to_bcd(info->tm_year % 100));     // Store two-digit year (00-99)
}
