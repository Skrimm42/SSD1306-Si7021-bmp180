/*
		iic.c file is a I2C Driver file.
    Copyright (C) 2018 Nima Mohammadi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "iic.h"

DMA_InitTypeDef   sEEDMA_InitStructure; 



__IO uint32_t  sEETimeout = sEE_LONG_TIMEOUT; 

//---------------Private Prototypes---------------------------------------------
void sEE_I2C_DMA_TX_IRQHandler(void);
void sEE_I2C_DMA_RX_IRQHandler(void);
uint32_t sEE_TIMEOUT_UserCallback(void);
void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction);




void i2cm_init(I2C_TypeDef* I2Cx, uint32_t i2c_clock)
{
   NVIC_InitTypeDef NVIC_InitStructure;  
  
  if (I2Cx == I2C1)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  else
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  I2C_Cmd(I2Cx, DISABLE);
  I2C_DeInit(I2Cx);
  I2C_InitTypeDef i2c_InitStruct;
  i2c_InitStruct.I2C_Mode = I2C_Mode_I2C;
  i2c_InitStruct.I2C_ClockSpeed = i2c_clock;
  i2c_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  i2c_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  i2c_InitStruct.I2C_Ack = I2C_Ack_Enable;
  i2c_InitStruct.I2C_OwnAddress1 = 0;
  I2C_Cmd(I2Cx, ENABLE);
  I2C_Init(I2Cx, &i2c_InitStruct);

   /* Enable the sEE_I2C peripheral DMA requests */
  I2C_DMACmd(sEE_I2C, ENABLE);
  
  GPIO_InitTypeDef InitStruct;
  InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
  InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

  if (I2Cx == I2C1)
    InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  else
    InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;

  GPIO_Init(GPIOB, &InitStruct);
  
    //---DMA-----------------
  /* Configure and enable I2C DMA TX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_TX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Configure and enable I2C DMA RX Channel interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = sEE_I2C_DMA_RX_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = sEE_I2C_DMA_PREPRIO;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = sEE_I2C_DMA_SUBPRIO;
  NVIC_Init(&NVIC_InitStructure);  
  
  /*!< I2C DMA TX and RX channels configuration */
  /* Enable the DMA clock */
  RCC_AHBPeriphClockCmd(sEE_I2C_DMA_CLK, ENABLE);
  
  /* I2C TX DMA Channel configuration */
  DMA_DeInit(sEE_I2C_DMA_CHANNEL_TX);
  sEEDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)sEE_I2C_DR_Address;
  sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;   /* This parameter will be configured durig communication */
  sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    /* This parameter will be configured durig communication */
  sEEDMA_InitStructure.DMA_BufferSize = 0xFFFF;            /* This parameter will be configured durig communication */
  sEEDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  sEEDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  sEEDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
  sEEDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  sEEDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  sEEDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  sEEDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(sEE_I2C_DMA_CHANNEL_TX, &sEEDMA_InitStructure);  
  
  /* I2C RX DMA Channel configuration */
  DMA_DeInit(sEE_I2C_DMA_CHANNEL_RX);
  DMA_Init(sEE_I2C_DMA_CHANNEL_RX, &sEEDMA_InitStructure);  
  
  /* Enable the DMA Channels Interrupts */
  DMA_ITConfig(sEE_I2C_DMA_CHANNEL_TX, DMA_IT_TC, ENABLE);
  DMA_ITConfig(sEE_I2C_DMA_CHANNEL_RX, DMA_IT_TC, ENABLE);    
    
}


/**
  * @brief  This function handles the DMA Tx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void sEE_I2C_DMA_TX_IRQHandler(void)
{
  /* Check if the DMA transfer is complete */ 
  if(DMA_GetFlagStatus(sEE_I2C_DMA_FLAG_TX_TC) != RESET)
  {  
    /* Disable the DMA Tx Channel and Clear all its Flags */  
    DMA_Cmd(sEE_I2C_DMA_CHANNEL_TX, DISABLE);
    DMA_ClearFlag(sEE_I2C_DMA_FLAG_TX_GL);

    /*!< Wait till all data have been physically transferred on the bus */
    sEETimeout = sEE_LONG_TIMEOUT;
    while(!I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BTF))
    {
      if((sEETimeout--) == 0) sEE_TIMEOUT_UserCallback();
    }
    
    /*!< Send STOP condition */
    I2C_GenerateSTOP(sEE_I2C, ENABLE);
    
    /* Perform a read on SR1 and SR2 register to clear eventualaly pending flags */
    (void)sEE_I2C->SR1;
    (void)sEE_I2C->SR2;
    
    /* Reset the variable holding the number of data to be written */
    //*sEEDataWritePointer = 0;  
  }
}

/**
  * @brief  This function handles the DMA Rx Channel interrupt Handler.
  * @param  None
  * @retval None
  */
void sEE_I2C_DMA_RX_IRQHandler(void)
{
  /* Check if the DMA transfer is complete */
  if(DMA_GetFlagStatus(sEE_I2C_DMA_FLAG_RX_TC) != RESET)
  {      
    /*!< Send STOP Condition */
    I2C_GenerateSTOP(sEE_I2C, ENABLE);    
    
    /* Disable the DMA Rx Channel and Clear all its Flags */  
    DMA_Cmd(sEE_I2C_DMA_CHANNEL_RX, DISABLE);
    DMA_ClearFlag(sEE_I2C_DMA_FLAG_RX_GL);
    
    /* Reset the variable holding the number of data to be read */
    //*sEEDataReadPointer = 0;
  }
}



uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}


uint32_t sEE_WritePage(uint8_t DevAddress, uint16_t WriteAddr, uint8_t* pBuffer, uint16_t NumByteToWrite)
{ 
  /* Set the pointer to the Number of data to be written. This pointer will be used 
      by the DMA Transfer Completer interrupt Handler in order to reset the 
      variable to 0. User should check on this variable in order to know if the 
      DMA transfer has been complete or not. */
  
uint8_t sEEAddress = DevAddress << 1;
  
  /*!< While the bus is busy */
  sEETimeout = sEE_LONG_TIMEOUT;
  while(I2C_GetFlagStatus(sEE_I2C, I2C_FLAG_BUSY))
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /*!< Send START condition */
  I2C_GenerateSTART(sEE_I2C, ENABLE);
  
  /*!< Test on EV5 and clear it */
  sEETimeout = sEE_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }
  
  /*!< Send EEPROM address for write */
  sEETimeout = sEE_FLAG_TIMEOUT;
  I2C_Send7bitAddress(sEE_I2C, sEEAddress, I2C_Direction_Transmitter);

  /*!< Test on EV6 and clear it */
  sEETimeout = sEE_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }

  
  /*!< Send the EEPROM's internal address to write to : only one byte Address */
  I2C_SendData(sEE_I2C, WriteAddr);
  
  /*!< Test on EV8 and clear it */
  sEETimeout = sEE_FLAG_TIMEOUT; 
  while(!I2C_CheckEvent(sEE_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
  {
    if((sEETimeout--) == 0) return sEE_TIMEOUT_UserCallback();
  }  
  
  /* Configure the DMA Tx Channel with the buffer address and the buffer size */
  sEE_LowLevel_DMAConfig((uint32_t)pBuffer, (uint16_t)(NumByteToWrite), sEE_DIRECTION_TX);
  
  /* Enable the DMA Tx Stream */
  DMA_Cmd(sEE_I2C_DMA_STREAM_TX, ENABLE);
  
  /* Enable the sEE_I2C peripheral DMA requests */
  I2C_DMACmd(sEE_I2C, ENABLE);
  
  /* If all operations OK, return sEE_OK (0) */
  return sEE_OK;
}


/**
  * @brief  Initializes DMA channel used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_LowLevel_DMAConfig(uint32_t pBuffer, uint32_t BufferSize, uint32_t Direction)
{ 
  /* Initialize the DMA with the new parameters */
  if (Direction == sEE_DIRECTION_TX)
  {
    /* Configure the DMA Tx Channel with the buffer address and the buffer size */
    sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    
    sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;  
    DMA_Init(sEE_I2C_DMA_CHANNEL_TX, &sEEDMA_InitStructure);  
  }
  else
  { 
    /* Configure the DMA Rx Channel with the buffer address and the buffer size */
    sEEDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pBuffer;
    sEEDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    sEEDMA_InitStructure.DMA_BufferSize = (uint32_t)BufferSize;      
    DMA_Init(sEE_I2C_DMA_CHANNEL_RX, &sEEDMA_InitStructure);    
  }
}


//-----------Enf DMA Functions--------------------------------------------------



int8_t i2cm_Start(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t IsRead, uint16_t TimeOut)
{

  uint16_t TOcntr;

  I2C_GenerateSTART(I2Cx, ENABLE);
  TOcntr = TimeOut;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)) && TOcntr) {TOcntr--;}
  if (!TOcntr)
    return I2C_ERR_HWerr;

  if (IsRead)
  {
    I2C_Send7bitAddress(I2Cx, slave_addr << 1, I2C_Direction_Receiver);
    TOcntr = TimeOut;
    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && TOcntr) {TOcntr--;}
  }
  else
  {
    I2C_Send7bitAddress(I2Cx, slave_addr << 1, I2C_Direction_Transmitter);
    TOcntr = TimeOut;
    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && TOcntr) {TOcntr--;}
  }

  if (!TOcntr)
	{
			//LOGE(" I2C Driver " , "i2c Bus is not Connected");
      return I2C_ERR_NotConnect;
	}

  return I2C_ERR_Ok;
}

int8_t i2cm_Stop(I2C_TypeDef* I2Cx, uint16_t TimeOut)
{
  I2C_GenerateSTOP(I2Cx, ENABLE);
  uint16_t TOcntr = TimeOut;
  while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF) && TOcntr);
  if (!TOcntr)
	{
    return I2C_ERR_HWerr;
	}

  return I2C_ERR_Ok;
}

int8_t i2cm_WriteBuff(I2C_TypeDef* I2Cx, uint8_t *pbuf, uint16_t len, uint16_t TimeOut)
{
  uint16_t TOcntr;

  while (len--)
  {
    I2C_SendData(I2Cx, *(pbuf++));
    TOcntr = TimeOut;
    while((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && TOcntr) {TOcntr--;}
    if (!TOcntr)
      return I2C_ERR_NotConnect;
  }

  return I2C_ERR_Ok;
}

int8_t i2cm_ReadBuffAndStop(I2C_TypeDef* I2Cx, uint8_t *pbuf, uint16_t len, uint16_t TimeOut)
{
  uint16_t TOcntr;

  I2C_AcknowledgeConfig(I2Cx, ENABLE);
    
  while (len-- != 1)
  {
    TOcntr = TimeOut;
    while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && TOcntr) {TOcntr--;}
    *pbuf++ = I2C_ReceiveData(I2Cx);
  }

  I2C_AcknowledgeConfig(I2Cx, DISABLE);
  I2C_GenerateSTOP(I2Cx,ENABLE);

  TOcntr = TimeOut;
  while ((!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) ) && TOcntr) {TOcntr--;}
  *pbuf++ = I2C_ReceiveData(I2Cx);

  return I2C_ERR_Ok;
}

