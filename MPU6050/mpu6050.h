#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
//address
#define MPU6050_ADDR        0x68
#define MPU6050_PWR_MGMT_1  0x6B
#define WHO_AM_I    0x75
#define SMPLRT_DIV  0x19

// để cài đặt bộ lọc nhiễu số
#define CONFIG      0x1A

#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C

#define AX_H 0x3B
#define AX_L 0x3C
#define AY_H 0x3D
#define AY_L 0x3E
#define AZ_H 0x3F
#define AZ_L 0x40

#define GX_H 0x43
#define GX_L 0x44
#define GY_H 0x45
#define GY_L 0x46
#define GZ_H 0x47
#define GZ_L 0x48


#define RAD_TO_DEG (180.0f / 3.14159265f)

extern int fd;

extern float pitch, roll;
extern float accel_x, accel_y, accel_z;
extern float gyro_x, gyro_y, gyro_z;


typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

} MPU6050_RawData;

typedef struct
{
    float accel_x;
    float accel_y;
    float accel_z;

    float gyro_x;
    float gyro_y;
    float gyro_z;

} MPU6050_Data;

/* khởi tạo */
int MPU6050_Init(void);

/* Thanh ghi */
int MPU6050_ReadRegs(int fd, uint8_t reg, uint8_t *buffer, int length);


/* đọc cảm biến */
int MPU6050_ReadRaw(int fd, MPU6050_RawData *data);

/* khởi tạo cảm biến */
void read_gyro(void);
void read_accel(void);
void read_angle(void);

/* hiệu chuẩn con quay hồi chuyển */
int MPU6050_CalibrateGyro(int fd, int samples, float *offset_x, float *offset_y, float *offset_z);

#endif
