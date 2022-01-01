#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include <stdint.h>

#define BMP280_S32_t int32_t
#define BMP280_U32_t uint32_t
#define BMP280_S64_t int64_t
#define BMP280_S32_t int32_t

#define BMP280_OSRS_T_0 (0u << 5u)
#define BMP280_OSRS_T_1 (1u << 5u)
#define BMP280_OSRS_T_2 (2u << 5u)
#define BMP280_OSRS_T_4 (3u << 5u)
#define BMP280_OSRS_T_8 (4u << 5u)
#define BMP280_OSRS_T_16 (5u << 5u)

#define BMP280_OSRS_P_0 (0u << 2u)
#define BMP280_OSRS_P_1 (1u << 2u)
#define BMP280_OSRS_P_2 (2u << 2u)
#define BMP280_OSRS_P_4 (3u << 2u)
#define BMP280_OSRS_P_8 (4u << 2u)
#define BMP280_OSRS_P_16 (5u << 2u)

#define BMP280_MODE_SLEEP 0u
#define BMP280_MODE_FORCED 1u
#define BMP280_MODE_NORMAL 3u

#define BMP280_FILTER_COEF_OFF 0
#define BMP280_FILTER_COEF_2 1
#define BMP280_FILTER_COEF_4 2
#define BMP280_FILTER_COEF_8 3
#define BMP280_FILTER_COEF_16 4

typedef struct s_bmp280_data
{
	uint32_t bmp280_pres;
	int32_t bmp280_temp;
} S_bmp280_data;

void bmp280_wait_id();
void bmp280_read_digs();
void bmp280_write(uint8_t addr, uint8_t val);
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P);
void bmp280_read_temp_pres(S_bmp280_data *data);
void bmp280_config(uint8_t val);
void bmp280_ctrl_meas(uint8_t val);
void bmp280_reset();

#endif /* INC_BMP280_H_ */
