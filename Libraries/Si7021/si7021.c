#include "stm32f10x.h"
#include "iic.h"
#include "si7021.h"


void I2C2_Setup(void); // Prototype
uint16_t SI75_ReadReg(uint8_t reg);



void I2C2_Setup(void)
{

}

// Read 16-bit LM75 register
uint16_t SI75_ReadReg(uint8_t reg) 
{
  uint16_t value;
  
  I2C_AcknowledgeConfig(I2C_PORT, ENABLE); // Enable I2C acknowledgment
  I2C_GenerateSTART(I2C_PORT, ENABLE);
  while (!I2C_CheckEvent(I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5
  I2C_Send7bitAddress(I2C_PORT, SI7021_ADDR, I2C_Direction_Transmitter); // Send slave address
  while (!I2C_CheckEvent(I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)); // Wait for EV6
  I2C_SendData(I2C_PORT, reg); // Send register address
  while (!I2C_CheckEvent(I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED)); // Wait for EV8
  I2C_GenerateSTART(I2C_PORT, ENABLE); // Send repeated START condition (aka Re-START)
  while (!I2C_CheckEvent(I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT)); // Wait for EV5
  I2C_Send7bitAddress(I2C_PORT, SI7021_ADDR, I2C_Direction_Receiver); // Send slave address for READ
  while (!I2C_CheckEvent(I2C_PORT, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); // Wait for EV6
  while (!I2C_CheckEvent(I2C_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED)); // Wait for EV7 (Byte received from slave)
  value = (I2C_ReceiveData(I2C_PORT) << 8); // Receive high byte
  I2C_AcknowledgeConfig(I2C_PORT, DISABLE); // Disable I2C acknowledgment
  I2C_GenerateSTOP(I2C_PORT, ENABLE); // Send STOP condition
  while (!I2C_CheckEvent(I2C_PORT, I2C_EVENT_MASTER_BYTE_RECEIVED)); // Wait for EV7 (Byte received from slave)
  value |= I2C_ReceiveData(I2C_PORT); // Receive low byte
  
  return value;
}

  


void Si7021_Setup(void)
{
  //I2C2_Setup();
}


float Si7021_ReadTemp(void)
{
  uint16_t tmp;
  float res;
  
  tmp = SI75_ReadReg(READ_T_HOLD_MASTER_MODE);
  res = (175.72 * tmp)/65536 - 46.85;
  
  return res;
}

uint8_t Si7021_ReadRH(void)
{
  uint16_t tmp;
  uint8_t res;
  
  tmp = SI75_ReadReg(READ_RH_HOLD_MASTER_MODE);
  if (tmp < 3146) tmp = 3146; // Limit to 0 %
  if (tmp > 55575) tmp = 55575; // Limit to 100%
  res = (125 * tmp)/65536 - 6;
  return res;
}

/*
Each time a relative humidity measurement is made a temperature measurement is also made for the purposes of
temperature compensation of the relative humidity measurement. If the temperature value is required, it can be
read using command 0xE0; this avoids having to perform a second temperature measurement.
*/
float Si7021_ReadTempFormer(void) 
{
  uint16_t tmp;
  float res;
  
  tmp = SI75_ReadReg(READ_T_PREVIOUS_MEASUREMENT);
  res = (175.72 * tmp)/65536 - 46.85;
  return res;
}

 void Si7021_Read_RH_Temp(Si7021_measurments* Si7021_res)
{
 
  Si7021_res -> RH = Si7021_ReadRH();
  Si7021_res -> Temperature_f = Si7021_ReadTempFormer();
   
}
