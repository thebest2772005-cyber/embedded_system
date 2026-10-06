#include "mpu6050.h"
#include <wiringPiI2C.h>
#include <stdio.h>
#include <math.h>

int fd;

float pitch, roll;
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;

float gyro_scale;
float accel_scale;

int MPU6050_Init(int gyro_range, int accel_range, uint8_t sample_divider)
{
    // 1. Init I2C
    fd = wiringPiI2CSetup(MPU6050_ADDR);

    if (fd < 0)
    {
        printf("I2C initialization failed!\n");
        return -1;
    }

    // 2. Check WHO_AM_I
    int data = wiringPiI2CReadReg8(fd, WHO_AM_I);

    printf("WHO_AM_I = 0x%02X\n", data);

    if (data != 0x68)
    {
        printf("MPU6050 is disconnected!\n");
        return -1;
    }

    printf("MPU6050 is connected!\n");

    // 3. Wake up MPU6050 + select PLL clock
    wiringPiI2CWriteReg8(fd, MPU6050_PWR_MGMT_1, 0x01);

    // 4. Gyroscope
    // gyro_range:
    // 0 = ±250
    // 1 = ±500
    // 2 = ±1000
    // 3 = ±2000 deg/s

    wiringPiI2CWriteReg8(fd, GYRO_CONFIG, gyro_range << 3);
    if (gyro_range == 0) gyro_scale = 131.0f;
    if (gyro_range == 1) gyro_scale = 65.5f;
    if (gyro_range == 2) gyro_scale = 32.8f;
    if (gyro_range == 3) gyro_scale = 16.4f;

    // 5. Accelerometer
    // accel_range:
    // 0 = ±2g
    // 1 = ±4g
    // 2 = ±8g
    // 3 = ±16g

    wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, accel_range << 3);

    if (accel_range == 0) accel_scale = 16384.0f;
    if (accel_range == 1) accel_scale = 8192.0f;
    if (accel_range == 2) accel_scale = 4096.0f;
    if (accel_range == 3) accel_scale = 2048.0f;

    // 6. Sample rate
    wiringPiI2CWriteReg8(fd, SMPLRT_DIV, sample_divider);

    // 7. Read back
    int gyro_config = wiringPiI2CReadReg8(fd, GYRO_CONFIG);
    int accel_config = wiringPiI2CReadReg8(fd, ACCEL_CONFIG);
    int sample_rate = wiringPiI2CReadReg8(fd, SMPLRT_DIV);

    printf("GYRO_CONFIG = 0x%02X\n", gyro_config);
    printf("ACCEL_CONFIG = 0x%02X\n", accel_config);
    printf("SMPLRT_DIV  = 0x%02X\n", sample_rate);

    return 0;
}

void MPU6050_SetDLPF(uint8_t config)
{
    // 4. Configure Digital Low Pass Filter (DLPF):
    wiringPiI2CWriteReg8(fd, CONFIG, config);
}

int MPU6050_ReadRegs(int fd, uint8_t reg, uint8_t *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        int value = wiringPiI2CReadReg8(fd, reg + i);
        if (value < 0) return -1;
        buffer[i] = (uint8_t)value;
    }
    return 0;
}

int MPU6050_ReadRaw(int fd, MPU6050_RawData *data)
{
    uint8_t buffer[14];
    if (MPU6050_ReadRegs(fd, AX_H, buffer, 14) < 0) return -1;

    data->accel_x = (buffer[0] << 8) | buffer[1];
    data->accel_y = (buffer[2] << 8) | buffer[3];
    data->accel_z = (buffer[4] << 8) | buffer[5];

    data->gyro_x = (buffer[8] << 8) | buffer[9];
    data->gyro_y = (buffer[10] << 8) | buffer[11];
    data->gyro_z = (buffer[12] << 8) | buffer[13];
    return 0;
}

void read_gyro(void)
{
    MPU6050_RawData data;
    MPU6050_ReadRaw(fd, &data);
    gyro_x = data.gyro_x / gyro_scale;
    gyro_y = data.gyro_y / gyro_scale;
    gyro_z = data.gyro_z / gyro_scale;
}

void read_accel(void)
{
    MPU6050_RawData data;
    MPU6050_ReadRaw(fd, &data);
    accel_x = data.accel_x / accel_scale;
    accel_y = data.accel_y / accel_scale;
    accel_z = data.accel_z / accel_scale;
}

void read_angle(void)
{
    read_accel();
    read_gyro();
    // Pitch: quay quanh truc Y
    pitch = atan2(-accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * RAD_TO_DEG;
    // Roll: quay quanh truc X
    roll  = atan2(accel_y, accel_z) * RAD_TO_DEG;
}
