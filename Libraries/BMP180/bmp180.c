#include "stm32f10x.h"
#include "bmp180.h"


void I2C_Setup(void);
void I2C_write(uint8_t data, uint8_t address);
uint8_t I2C_read(uint8_t address);

int16_t AC1 = 0, AC2 = 0, AC3 = 0; 
uint16_t AC4 = 0, AC5 = 0, AC6 = 0;
int16_t B1 = 0, B2 = 0, MB = 0, MC = 0, MD = 0;

uint32_t UT, UP;
int32_t X1; 
int32_t X2;
int32_t X3;
int32_t B5;
int16_t T; 
int32_t B6;
int32_t B3; 
uint32_t B4;
uint32_t B7; 
int32_t P;

void I2C_Setup(void)
{
  
}


void BMP180_Setup(void)
{
  I2C_Setup();
  AC1 = I2C_read(0xAA)*256 + I2C_read(0xAB);
  AC2 = I2C_read(0xAC)*256 + I2C_read(0xAD);
  AC3 = I2C_read(0xAE)*256 + I2C_read(0xAF);
  AC4 = I2C_read(0xB0)*256 + I2C_read(0xB1);
  AC5 = I2C_read(0xB2)*256 + I2C_read(0xB3);
  AC6 = I2C_read(0xB4)*256 + I2C_read(0xB5);
  B1 = I2C_read(0xB6)*256 + I2C_read(0xB7);
  B2 = I2C_read(0xB8)*256 + I2C_read(0xB9);
  MB = I2C_read(0xBA)*256 + I2C_read(0xBB);
  MC = I2C_read(0xBC)*256 + I2C_read(0xBD);
  MD = I2C_read(0xBE)*256 + I2C_read(0xBF);
}

void I2C_write(uint8_t data, uint8_t address)
{
	I2C_GenerateSTART(BMP180_I2C_PORT, ENABLE);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(BMP180_I2C_PORT, 0xEF, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(BMP180_I2C_PORT, address);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(BMP180_I2C_PORT, data);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTOP(BMP180_I2C_PORT, ENABLE);
}

uint8_t I2C_read(uint8_t address)
{
	uint8_t data;
	while(I2C_GetFlagStatus(BMP180_I2C_PORT, I2C_FLAG_BUSY));
	I2C_GenerateSTART(BMP180_I2C_PORT, ENABLE);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(BMP180_I2C_PORT, 0xEF, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_SendData(BMP180_I2C_PORT, address);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_GenerateSTART(BMP180_I2C_PORT, ENABLE);
	while(!I2C_CheckEvent(BMP180_I2C_PORT, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(BMP180_I2C_PORT, 0xEF, I2C_Direction_Receiver);
	while(!I2C_CheckEvent(BMP180_I2C_PORT,I2C_EVENT_MASTER_BYTE_RECEIVED));
	data = I2C_ReceiveData(BMP180_I2C_PORT);
	I2C_AcknowledgeConfig(BMP180_I2C_PORT, DISABLE);
	I2C_GenerateSTOP(BMP180_I2C_PORT, ENABLE);
	return data;
}

void BMP180_get_T_P(BMP180_measurements* BMP180_res)
{
  //Start temperature measurement
  I2C_write(0x2E, 0xF4);
  //Wait to measurement
  //GPIO_SetBits(GPIOB, GPIO_Pin_12);
  for(volatile uint32_t del = 0; del<18500; del++);
  //GPIO_ResetBits(GPIOB, GPIO_Pin_12);
  //Read uncompensated temperature value
  UT = I2C_read(0xF6) * 256 + I2C_read(0xF7);
  
  //Start pressure measurement
  I2C_write(0x34 + (OSS<<6), 0xF4);
  //Wait to measurement
  for(volatile uint32_t del = 0; del<100000; del++);
  //Read uncompensated pressure value
  UP = I2C_read(0xF6) * 256 + I2C_read(0xF7);
  
  //temparature compensation
  X1 = (UT - AC6) * AC5 / 32768;
  X2 = MC * 2048 / (X1 + MD);
  B5 = X1 + X2;
  T = (B5 + 8) / 16;
  
  //pressure compensation
  B6 = B5 - 4000;
  X1 = (B2 * (B6 * B6/4096))/2048;
  X2 = AC2 * B6 / 2048;
  X3 = X1 + X2;
  B3 = (((AC1 * 4 + X3)) + 2)/4;
  X1 = AC3  * B6 / 8192;
  X2 = (B1 * (B6 * B6 / 4096))/ 65536;
  X3 = ((X1 + X2) + 2) / 4;
  B4 = AC4 * (unsigned long)(X3 + 32768)/32768;
  B7 = ((unsigned long)UP - B3) * 50000;
  if(B7 < 0x80000000)P = (B7 * 2) / B4;
  else P = (B7/B4) * 2;
  X1 = (P/256) * (P/256);
  X1 = (X1 * 3038)/65536;
  X2 = (-7357 * P)/65536;
  P = P + (X1 + X2 + 3791)/16;
  
  BMP180_res -> T  = (float)T/10;
  BMP180_res -> P = (float)P/1000;
}
