//bmp280_user.h


#ifndef __BMP280_USER__
#define __BMP280_USER__

#include "stm32f10x.h"
#include "bmp280.h"

int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr,	uint8_t *data, uint16_t len);
void user_delay_ms(__IO uint32_t period);
void BMP280_I2C_Setup(struct bmp280_dev *bmp);


#define BMP280_I2C I2C1

#endif