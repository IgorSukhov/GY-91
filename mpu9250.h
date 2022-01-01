/*
 * mpu9250.h
 *
 *  Created on: 30 июл. 2021 г.
 *      Author: SuhPC
 */

#ifndef INC_MPU9250_H_
#define INC_MPU9250_H_

#include <stdint.h>

#define STATUS_OK 0u
#define STATUS_ERROR 1u

#define AK8963_ST1_DRDY 0x01u
#define AK8963_ST1_DOR 0x02u

#define AK8963_WIA		0x00u

#define AK8963_INFO		0x01u

#define AK8963_ST1		0x02u
#define AK8963_ST2_DRDY	0x01u
#define AK8963_ST2_DOR	0x02u

#define AK8963_HXL                   0x03u

#define AK8963_HXH                   0x04u

#define AK8963_HYL	                 0x05u

#define AK8963_HYH	                 0x06u

#define AK8963_HZL                   0x07u

#define AK8963_HZH	                 0x08u

#define AK8963_ST2		             0x09u
#define AK8963_ST2_HOFL	             0x08u
#define AK8963_ST2_BITM	             0x10u

#define AK8963_CNTL1                 0x0Au
#define AK8963_CNTL1_MODE_POWER_DOWN 0x00u
#define AK8963_CNTL1_MODE_SINGLE     0x01u
#define AK8963_CNTL1_MODE_CONT1      0x02u
#define AK8963_CNTL1_MODE_CONT2      0x06u
#define AK8963_CNTL1_MODE_EXT_TRIG   0x04u
#define AK8963_CNTL1_MODE_SELF_TEST  0x08u
#define AK8963_CNTL1_MODE_ROM_ACCESS 0x0fu
#define AK8963_CNTL1_14BIT           0x00u
#define AK8963_CNTL1_16BIT           0x10u

#define AK8963_CNTL2		         0x0Bu
#define AK8963_CNTL2_SRST	         0x01u

#define AK8963_ASTC		             0x0Cu
#define AK8963_ASTC_SELF             0x40u

#define AK8963_TS1                   0x0Du
#define AK8963_TS2                   0x0Eu
#define AK8963_I2CDIS                0x0Fu
#define AK8963_ASAX                  0x10u
#define AK8963_ASAY                  0x11u
#define AK8963_ASAZ                  0x12u
#define AK8963_RSV                   0x13u

typedef struct s_mpu9250_data
{
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	int16_t temp;
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
} S_mpu9250_data;

typedef struct s_mpu9250_AK8963_data
{
	int16_t magn_x;
	int16_t magn_y;
	int16_t magn_z;
} S_AK8963_mpu9250_data;

void mpu9250_read_data(S_mpu9250_data *mpu9250_data);
void mpu9250_AK8963_cntl(uint8_t val);
void mpu9250_int_bypass();
uint32_t mpu9250_check_whoami();
void mpu9250_write(uint8_t addr, uint8_t val);
uint8_t mpu9250_read(uint8_t addr);
void mpu9250_AK8963_write(uint8_t addr, uint8_t val);
//uint8_t mpu9250_AK8963_read(uint8_t addr);
uint32_t mpu9250_AK8963_check_wia();
uint8_t get_mpu9250_AK8963_cntl();
void mpu9250_AK8963_read(S_AK8963_mpu9250_data *mpu9250_AK8963_data);
void mpu9250_AK8963_read_raw(uint8_t addr, uint8_t *data, uint8_t len);
void mpu9250_AK8963_read_asa(uint8_t *asax, uint8_t *asay, uint8_t *asaz);
void mpu9250_AK8963_test();
void mpu9250_AK8963_read_data(S_AK8963_mpu9250_data *mpu9250_AK8963_data);

#endif /* INC_MPU9250_H_ */
