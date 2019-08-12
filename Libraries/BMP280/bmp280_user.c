// bmp280_user.c

#include "bmp280_user.h"
#include "iic.h" //access to i2c read/write functions

void ErrorUserCallback(uint8_t ErrCode);


int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  //dev_id don't check (assume there is single device present)
    uint8_t status;
    status = i2cm_Start(BMP280_I2C, BMP280_I2C_ADDR_PRIM, 0, 1000);
    status = i2cm_WriteBuff(BMP280_I2C, &reg_addr , 1, 1000);
    status = i2cm_Start(BMP280_I2C, BMP280_I2C_ADDR_PRIM, 1, 1000);
    status = i2cm_ReadBuffAndStop(BMP280_I2C, data, len, 1000);
    return status;
}


int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr,	uint8_t *data, uint16_t len)
{
  //dev_id don't check (assume there is single device present)
  uint8_t status;
  status = i2cm_Start(BMP280_I2C, BMP280_I2C_ADDR_PRIM, 1, 1000);
  status = i2cm_WriteBuff(BMP280_I2C, data, len, 1000);
  status = i2cm_Stop(BMP280_I2C, 1000);
  return status;
}


void user_delay_ms(__IO uint32_t period)
{
  volatile uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq (&RCC_Clocks);
  
  nCount=(RCC_Clocks.HCLK_Frequency/10000)*period;
  for (; nCount!=0; nCount--);
}


void BMP280_I2C_Setup(struct bmp280_dev *bmp)
{
  int8_t rslt;
  struct bmp280_config conf;
  
  /* Map the delay function pointer with the function responsible for implementing the delay */
  bmp -> delay_ms = user_delay_ms;
  /* Assign device I2C address based on the status of SDO pin (GND for PRIMARY(0x76) & VDD for SECONDARY(0x77)) */
  bmp -> dev_id = BMP280_I2C_ADDR_PRIM;
  
  /* Select the interface mode as I2C */
  bmp -> intf = BMP280_I2C_INTF;
  
  /* Map the I2C read & write function pointer with the functions responsible for I2C bus transfer */
  bmp -> read = user_i2c_read;
  bmp -> write = user_i2c_write;
  
  rslt = bmp280_init(bmp);
  if(rslt) ErrorUserCallback(rslt);
  
  rslt = bmp280_get_config(&conf, bmp);
  if(rslt) ErrorUserCallback(rslt);
  
  /* configuring the temperature oversampling, filter coefficient and output data rate */
  /* Overwrite the desired settings */
  conf.filter = BMP280_FILTER_COEFF_2;
  
  /* Pressure oversampling set at 4x */
  conf.os_pres = BMP280_OS_4X;
  
  /* Setting the output data rate as 1HZ(1000ms) */
  conf.odr = BMP280_ODR_1000_MS;
  rslt = bmp280_set_config(&conf, bmp);
  if(rslt) ErrorUserCallback(rslt);
  
  /* Always set the power mode after setting the configuration */
  rslt = bmp280_set_power_mode(BMP280_NORMAL_MODE, bmp);
  if(rslt) ErrorUserCallback(rslt);
}


void ErrorUserCallback(uint8_t ErrCode)
{
  while(1);//trap
}