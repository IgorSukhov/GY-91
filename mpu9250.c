#include "mpu9250.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_i2c.h"
#include "main.h"
#include "string.h"

void mpu9250_read_data(S_mpu9250_data *mpu9250_data)
{
	uint8_t t_val[14u];

	memset(t_val, 0u, sizeof(t_val));
	t_val[0u] = 0x3bu;
	HAL_I2C_Master_Transmit(&hi2c1, 0xd0u, t_val, 1u, 1000u);
	HAL_I2C_Master_Receive(&hi2c1, 0xd0u, t_val, sizeof(t_val), 1000u);

	mpu9250_data->accel_x = (t_val[0u] << 8u) | t_val[1u];
	mpu9250_data->accel_y = (t_val[2u] << 8u) | t_val[3u];
	mpu9250_data->accel_z = (t_val[4u] << 8u) | t_val[5u];
	mpu9250_data->temp = (t_val[6u] << 8u) | t_val[7u];
	mpu9250_data->gyro_x = (t_val[8u] << 8u) | t_val[9u];
	mpu9250_data->gyro_y = (t_val[10u] << 8u) | t_val[11u];
	mpu9250_data->gyro_z = (t_val[12u] << 8u) | t_val[13u];
}

uint32_t ehc_magnetic_sensor_overflow = 0u;

void mpu9250_AK8963_read_data(S_AK8963_mpu9250_data *mpu9250_AK8963_data)
{
	uint8_t t_val[14u];
	uint8_t drdy;
	uint8_t st2;
	//uint8_t dor;

	memset(t_val, 0u, sizeof(t_val));
	t_val[0u] = 0x02u;
	HAL_I2C_Master_Transmit(&hi2c1, 0x18u, t_val, 1u, 1000u);
	HAL_I2C_Master_Receive(&hi2c1, 0x18u, t_val, 1u, 1000u);

	drdy = t_val[0u] & 1u;
	//dor = t_val[0u] & 2u;

	if (drdy > 0u)
	{
		t_val[0u] = 0x03u;
		HAL_I2C_Master_Transmit(&hi2c1, 0x18u, t_val, 1u, 1000u);
		HAL_I2C_Master_Receive(&hi2c1, 0x18u, t_val, 7u, 1000u);
		mpu9250_AK8963_data->magn_x = (t_val[1u] << 8u) | t_val[0u];
		mpu9250_AK8963_data->magn_y = (t_val[3u] << 8u) | t_val[2u];
		mpu9250_AK8963_data->magn_z = (t_val[5u] << 8u) | t_val[4u];
		st2 = t_val[0u];
	}

	if (st2 & AK8963_ST2_HOFL)
	{
		ehc_magnetic_sensor_overflow++;
	}

//	if (dor > 0u)
//	{
//		int a = 0u;
//		a++;
//	}
}

uint32_t mpu9250_AK8963_check_wia()
{
	uint8_t t_val = 0x00u;

	mpu9250_AK8963_read_raw(AK8963_WIA, &t_val, 1u);

	if (t_val == 0x48)
	{
		return STATUS_OK;
	}
	else
	{
		return STATUS_ERROR;
	}
}

void mpu9250_AK8963_test()
{
	uint8_t st1;
	uint8_t data[7u];

//	(1) Set Power-down mode. (MODE[3:0]=“0000”)
	mpu9250_AK8963_write(AK8963_CNTL1, AK8963_CNTL1_MODE_POWER_DOWN | AK8963_CNTL1_16BIT);
//	(2) Write “1” to SELF bit of ASTC register (other bits in this register should be kept “0”)
	mpu9250_AK8963_write(AK8963_ASTC, AK8963_ASTC_SELF);
//	(3) Set Self-test Mode. (MODE[3:0]=“1000”)
	mpu9250_AK8963_write(AK8963_CNTL1, AK8963_CNTL1_MODE_SELF_TEST);
//	(4) Check Data Ready or not by any of the following method.
//	- Polling DRDY bit of ST1 register
//	- Monitor DRDY pin
//	When Data Ready, proceed to the next step.
	do
	{
		mpu9250_AK8963_read_raw(AK8963_ST1, &st1, 1u);
	} while (!(st1 & AK8963_ST1_DRDY));
//	(5) Read measurement data (HXL to HZH)
	mpu9250_AK8963_read_raw(AK8963_HXL, data, sizeof(data));
//	(6) Write “0” to SELF bit of ASTC register
	mpu9250_AK8963_write(AK8963_ASTC, 0u);
//	(7) Set Power-down mode. (MODE[3:0]=“0000”)
	mpu9250_AK8963_write(AK8963_CNTL1, AK8963_CNTL1_MODE_POWER_DOWN | AK8963_CNTL1_16BIT);
}

void mpu9250_AK8963_cntl(uint8_t val)
{
	mpu9250_AK8963_write(AK8963_CNTL1, val);
}

void mpu9250_int_bypass()
{
	mpu9250_write(0x37u, 0x02u);
}

uint32_t mpu9250_check_whoami()
{
	if (mpu9250_read(0x75u) == 0x71)
	{
		return STATUS_OK;
	}
	else
	{
		return STATUS_ERROR;
	}
}


void mpu9250_AK8963_read_asa(uint8_t *asax, uint8_t *asay, uint8_t *asaz)
{
	uint8_t arr[3u];

	mpu9250_AK8963_read_raw(AK8963_ASAX, arr, sizeof(arr));
	*asax = arr[0u];
	*asay = arr[1u];
	*asaz = arr[2u];
}

void mpu9250_write(uint8_t addr, uint8_t val)
{
	uint8_t t_w[2u];

	t_w[0u] = addr;
	t_w[1u] = val;
	HAL_I2C_Master_Transmit(&hi2c1, 0xd0u, t_w, sizeof(t_w), 1000u);
}

uint8_t mpu9250_read(uint8_t addr)
{
	uint8_t t_val[1u];

	t_val[0u] = addr;
	HAL_I2C_Master_Transmit(&hi2c1, 0xd0u, t_val, sizeof(t_val), 1000u);
	t_val[0u] = 0u;
	HAL_I2C_Master_Receive(&hi2c1, 0xd0u, t_val, sizeof(t_val), 1000u);

	return t_val[0u];
}

void mpu9250_AK8963_write(uint8_t addr, uint8_t val)
{
	uint8_t t_w[2u];

	t_w[0u] = addr;
	t_w[1u] = val;
	HAL_I2C_Master_Transmit(&hi2c1, 0x18u, t_w, sizeof(t_w), 1000u);
}

void mpu9250_AK8963_read_raw(uint8_t addr, uint8_t *data, uint8_t len)
{
	uint8_t t_val[1u];

	t_val[0u] = addr;

	HAL_I2C_Master_Transmit(&hi2c1, 0x18u, t_val, sizeof(t_val), 1000u);
	HAL_I2C_Master_Receive(&hi2c1, 0x18u, data, len, 1000u);
}
