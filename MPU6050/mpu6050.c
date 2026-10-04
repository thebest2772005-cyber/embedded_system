#include "mpu6050.h"
#include <wiringPiI2C.h>
#include <stdio.h>
#include <math.h>

int fd;

float pitch, roll;
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;

int MPU6050_Init(void)
{
	int gyro_config;
	int accel_config;

	fd = wiringPiI2CSetup(MPU6050_ADDR);
	if (fd < 0){
   		printf("I2C error!\n");
   		return -1;
	}

	wiringPiI2CWriteReg8(fd, MPU6050_PWR_MGMT_1, 0x00);
	wiringPiI2CWriteReg8(fd, SMPLRT_DIV, 0x07);


	int data = wiringPiI2CReadReg8(fd, WHO_AM_I);
	printf("WHO_AM_I = 0x%02X\n", data);

	wiringPiI2CWriteReg8(fd, GYRO_CONFIG, 0x00);
	wiringPiI2CWriteReg8(fd, ACCEL_CONFIG, 0x00);
	gyro_config = wiringPiI2CReadReg8(fd, GYRO_CONFIG);
	accel_config = wiringPiI2CReadReg8(fd, ACCEL_CONFIG);

	if (data  == 0x68){
		printf("MPU is connected!");

	}
	else {
		printf("MPU is disconnected!");
		return -1;
	}

	printf("GYRO_CONFIG = 0x%02X\n", gyro_config);
	printf("ACCEL_CONFIG = 0x%02X\n", accel_config);
    	return 0;
}

int MPU6050_ReadRegs(int fd, uint8_t reg, uint8_t *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        int value = wiringPiI2CReadReg8(fd, reg+i);
        if (value < 0) return -1;
        buffer[i] = (uint8_t)value;
    }
    return 0;
}

int MPU6050_ReadRaw(int fd, MPU6050_RawData *data)
{
    uint8_t buffer[14];
    if (MPU6050_ReadRegs(fd, AX_H, buffer, 14) < 0)   return -1;

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
    	gyro_x = data.gyro_x / 131.0f;
   	gyro_y = data.gyro_y / 131.0f;
    	gyro_z = data.gyro_z / 131.0f;
}

void read_accel(void)
{
    	MPU6050_RawData data;
   	MPU6050_ReadRaw(fd, &data);
    	accel_x = data.accel_x / 16384.0f;
   	accel_y = data.accel_y / 16384.0f;
    	accel_z = data.accel_z / 16384.0f;
}
void read_angle(void) {
    read_accel();
    read_gyro();
    // Pitch: quay quanh truc Y
    pitch = atan2(-accel_x, sqrt(accel_y * accel_y + accel_z * accel_z)) * RAD_TO_DEG;
    // Roll: quay quanh truc X
    roll  = atan2(accel_y, accel_z) * RAD_TO_DEG;
}
