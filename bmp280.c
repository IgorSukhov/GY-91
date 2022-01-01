#include "bmp280.h"
#include "main.h"
#include <stdint.h>
#include "stm32f1xx.h"
#include "stm32f1xx_hal_i2c.h"

uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;

void bmp280_config(uint8_t val)
{
	uint8_t t_w[2u];

	t_w[0u] = 0xf5u;
	t_w[1u] = val;
	HAL_I2C_Master_Transmit(&hi2c1, 0xecu, t_w, sizeof(t_w), 1000u);
}

void bmp280_ctrl_meas(uint8_t val)
{
	uint8_t t_w[2u];

	t_w[0u] = 0xf4u;
	t_w[1u] = val;
	HAL_I2C_Master_Transmit(&hi2c1, 0xecu, t_w, sizeof(t_w), 1000u);
}

void bmp280_reset()
{
	uint8_t t_w[2u];

	t_w[0u] = 0xe0u;
	t_w[1u] = 0xb6u;
	HAL_I2C_Master_Transmit(&hi2c1, 0xecu, t_w, sizeof(t_w), 1000u);
}

void bmp280_wait_id()
{
	uint8_t t_w[1u];
	uint8_t t_r[1u];

	//HAL_Delay(500u);

	bmp280_write(0xe0u,  0xb6);

	do
	{
		t_w[0u] = 0xd0u;
		HAL_I2C_Master_Transmit(&hi2c1, 0xecu, t_w, sizeof(t_w), 1000u);
		HAL_I2C_Master_Receive(&hi2c1, 0xecu, t_r, sizeof(t_r), 1000u);
	} while (t_r[0u] != 0x58u);
}

void bmp280_read_digs()
{
	uint8_t digs[24u];
	uint8_t pData[1u];

	pData[0u] = 0x88u;
	HAL_I2C_Master_Transmit(&hi2c1, 0xecu, pData, sizeof(pData), 1000u);
	HAL_I2C_Master_Receive(&hi2c1, 0xecu, digs, sizeof(digs), 1000u);

	dig_T1 = (digs[1u] << 8u) | digs[0u];
	dig_T2 = (digs[3u] << 8u) | digs[2u];
	dig_T3 = (digs[5u] << 8u) | digs[4u];
	dig_P1 = (digs[7u] << 8u) | digs[6u];
	dig_P2 = (digs[9u] << 8u) | digs[8u];
	dig_P3 = (digs[11u] << 8u) | digs[10u];
	dig_P4 = (digs[13u] << 8u) | digs[12u];
	dig_P5 = (digs[15u] << 8u) | digs[14u];
	dig_P6 = (digs[17u] << 8u) | digs[16u];
	dig_P7 = (digs[19u] << 8u) | digs[18u];
	dig_P8 = (digs[21u] << 8u) | digs[20u];
	dig_P9 = (digs[23u] << 8u) | digs[22u];
}

void bmp280_write(uint8_t addr, uint8_t val)
{
	uint8_t pData[2u];

	pData[0u] = addr;
	pData[1u] = val;
	HAL_I2C_Master_Transmit(&hi2c1, 0xecu, pData, sizeof(pData), 1000u);
}


#define BMP280_S32_t int32_t
#define BMP280_U32_t uint32_t
#define BMP280_S64_t int64_t
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
BMP280_S32_t t_fine;



BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
	BMP280_S32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((BMP280_S32_t)dig_T1 << 1))) * ((BMP280_S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((BMP280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)dig_T1))) >> 12) * ((BMP280_S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

#define BMP280_S32_t int32_t

BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P)
{
	BMP280_S32_t var1, var2;
	BMP280_U32_t p;
	var1 = (((BMP280_S32_t)t_fine)>>1) - (BMP280_S32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BMP280_S32_t)dig_P6);
	var2 = var2 + ((var1*((BMP280_S32_t)dig_P5))<<1);
	var2 = (var2>>2)+(((BMP280_S32_t)dig_P4)<<16);
	var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BMP280_S32_t)dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((BMP280_S32_t)dig_P1))>>15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = (((BMP280_U32_t)(((BMP280_S32_t)1048576)-adc_P)-(var2>>12)))*3125;
	if (p < 0x80000000)
	{
		p = (p << 1) / ((BMP280_U32_t)var1);
	}
	else
	{
		p = (p / (BMP280_U32_t)var1) * 2;
	}
	var1 = (((BMP280_S32_t)dig_P9) * ((BMP280_S32_t)(((p>>3) * (p>>3))>>13)))>>12;
	var2 = (((BMP280_S32_t)(p>>2)) * ((BMP280_S32_t)dig_P8))>>13;
	p = (BMP280_U32_t)((BMP280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));
	return p;
}

void bmp280_read_temp_pres(S_bmp280_data *data)
{
	uint8_t pData[1u];
	uint8_t bmp280_data[6u];
	int32_t t_temp;
	uint32_t t_pres;

	pData[0u] = 0xf7u;
	HAL_I2C_Master_Transmit(&hi2c1, 0xecu, pData, sizeof(pData), 1000u);
	HAL_I2C_Master_Receive(&hi2c1, 0xecu, bmp280_data, sizeof(bmp280_data), 1000u);

	t_pres = ((bmp280_data[0u] << 16u) | (bmp280_data[1u] << 8u) | (bmp280_data[2u])) >> 4u;
	t_temp = ((bmp280_data[3u] << 16u) | (bmp280_data[4u] << 8u) | (bmp280_data[5u])) >> 4u;

	data->bmp280_temp = bmp280_compensate_T_int32(t_temp);
	data->bmp280_pres = bmp280_compensate_P_int32(t_pres);
}
