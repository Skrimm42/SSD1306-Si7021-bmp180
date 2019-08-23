//sEEPROM_SPI.c
// serial EEPROM 

#include "sEEPROM_SPI.h"



void sEE_WriteEnable(void);
void sEE_WaitForWriteEnd(void);


void sEE_LowLevel_Init(void);

// Serial EEPROM initialization
// SPI2 config
// PB12 - NSS
// PB14 - MISO
// PB15 - MOSI
// PB13 - SCK
// Use as constants and calibrating coefficients storage tool
//void sEEPROM_SPI2_init(void)
//{
//  GPIO_InitTypeDef GPIO_InitStructure;
//  SPI_InitTypeDef  SPI_InitStructure;
//  
//  //NSS as simple output
//  // Enable GPIOB clock 
//  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//  GPIO_SetBits(GPIOB, GPIO_Pin_12);
//  
//  // Config SPI2
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
//  
//  // Connect GPIO pins to AF5 (SPI2) 
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2); 
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
//  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
//  
//  // SPI configuration
//  // Enable the SPI clock 
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//  SPI_I2S_DeInit(SPI2);
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//  SPI_InitStructure.SPI_CRCPolynomial = 7;
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
//  SPI_Init(SPI2, &SPI_InitStructure);
//  SPI_NSSInternalSoftwareConfig(SPI2, SPI_NSSInternalSoft_Set);
//  
//  // Enable SPI2
//  SPI_Cmd(SPI2, ENABLE);
//  
//}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_Init(void)
{ 
  SPI_InitTypeDef  SPI_InitStructure;

  sEE_LowLevel_Init();
    
  /*!< Deselect the EE: Chip Select high */
  sEE_CS_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sEE_SPI, &SPI_InitStructure);
  SPI_NSSInternalSoftwareConfig(sEE_SPI, SPI_NSSInternalSoft_Set);

  /*!< Enable the sEE_SPI  */
  SPI_Cmd(sEE_SPI, ENABLE);
}

/**
  * @brief  Initializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_LowLevel_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;

  /*!< Enable the SPI clock */
  sEE_SPI_CLK_INIT(sEE_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_APB2PeriphClockCmd(sEE_SPI_SCK_GPIO_CLK | sEE_SPI_MISO_GPIO_CLK | 
                         sEE_SPI_MOSI_GPIO_CLK | sEE_CS_GPIO_CLK, ENABLE);
  
  /*!< SPI pins configuration *************************************************/

  GPIO_InitStructure.GPIO_Pin = sEE_SPI_SCK_PIN | sEE_SPI_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sEE_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = sEE_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(sEE_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = sEE_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sEE_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
     
}

/**
  * @brief  Reads a block of data from the EEPROM.
  * @param  pBuffer: pointer to the buffer that receives the data read from the EEPROM.
  * @param  ReadAddr: EEPROM's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the EEPROM.
  * @retval None
  */
void sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead)
{  
 
  sEE_CS_LOW();
  
  /*!< Send "Read from Memory " instruction */
  sEE_SendByte(sEE_CMD_READ);
  
//  /*!< Send ReadAddr high nibble address byte to read from */
//  sEE_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*!< Send ReadAddr medium nibble address byte to read from */
  sEE_SendByte((ReadAddr& 0xFF00) >> 8);
  /*!< Send ReadAddr low nibble address byte to read from */
  sEE_SendByte(ReadAddr & 0xFF);
  
  while (NumByteToRead--) /*!< while there is data to be read */
  {
    /*!< Read a byte from the EE */
    *pBuffer = sEE_SendByte(sEE_DUMMY_BYTE);
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
  }
  
  /*!< Deselect the EE: Chip Select high */
  sEE_CS_HIGH();
}


/**
  * @brief  Writes block of data to the EE. In this function, the number of
  *         WRITE cycles are reduced, using Page WRITE sequence.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the EE.
  * @param  WriteAddr: EE's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the EE.
  * @retval None
  */
void sEE_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  Addr = WriteAddr % sEE_SPI_PAGESIZE;
  count = sEE_SPI_PAGESIZE - Addr;
  NumOfPage =  NumByteToWrite / sEE_SPI_PAGESIZE;
  NumOfSingle = NumByteToWrite % sEE_SPI_PAGESIZE;

  if (Addr == 0) /*!< WriteAddr is sEE_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sEE_PAGESIZE */
    {
      sEE_WritePage(pBuffer, WriteAddr, NumByteToWrite);
    }
    else /*!< NumByteToWrite > sEE_PAGESIZE */
    {
      while (NumOfPage--)
      {
        sEE_WritePage(pBuffer, WriteAddr, sEE_SPI_PAGESIZE);
        WriteAddr +=  sEE_SPI_PAGESIZE;
        pBuffer += sEE_SPI_PAGESIZE;
      }

      sEE_WritePage(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*!< WriteAddr is not sEE_PAGESIZE aligned  */
  {
    if (NumOfPage == 0) /*!< NumByteToWrite < sEE_PAGESIZE */
    {
      if (NumOfSingle > count) /*!< (NumByteToWrite + WriteAddr) > sEE_PAGESIZE */
      {
        temp = NumOfSingle - count;

        sEE_WritePage(pBuffer, WriteAddr, count);
        WriteAddr +=  count;
        pBuffer += count;

        sEE_WritePage(pBuffer, WriteAddr, temp);
      }
      else
      {
        sEE_WritePage(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*!< NumByteToWrite > sEE_PAGESIZE */
    {
      NumByteToWrite -= count;
      NumOfPage =  NumByteToWrite / sEE_SPI_PAGESIZE;
      NumOfSingle = NumByteToWrite % sEE_SPI_PAGESIZE;

      sEE_WritePage(pBuffer, WriteAddr, count);
      WriteAddr +=  count;
      pBuffer += count;

      while (NumOfPage--)
      {
        sEE_WritePage(pBuffer, WriteAddr, sEE_SPI_PAGESIZE);
        WriteAddr +=  sEE_SPI_PAGESIZE;
        pBuffer += sEE_SPI_PAGESIZE;
      }

      if (NumOfSingle != 0)
      {
        sEE_WritePage(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}


/**
  * @brief  Writes more than one byte to the EE with a single WRITE cycle 
  *         (Page WRITE sequence).
  * @note   The number of byte can't exceed the EE page size.
  * @param  pBuffer: pointer to the buffer  containing the data to be written
  *         to the EE.
  * @param  WriteAddr: EE's internal address to write to.
  * @param  NumByteToWrite: number of bytes to write to the EE, must be equal
  *         or less than "sEE_PAGESIZE" value.
  * @retval None
  */
void sEE_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  /*!< Enable the write access to the EE */
  sEE_WriteEnable();

  /*!< Select the EE: Chip Select low */
  sEE_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  sEE_SendByte(sEE_CMD_WRITE);
//  /*!< Send WriteAddr high nibble address byte to write to */
//  sEE_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  sEE_SendByte((WriteAddr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  sEE_SendByte(WriteAddr & 0xFF);

  /*!< while there is data to be written on the EE */
  while (NumByteToWrite--)
  {
    /*!< Send the current byte */
    sEE_SendByte(*pBuffer);
    /*!< Point on the next byte to be written */
    pBuffer++;
  }

  /*!< Deselect the EE: Chip Select high */
  sEE_CS_HIGH();

  /*!< Wait the end of EE writing */
  sEE_WaitForWriteEnd();
}


/**
  * @brief  Enables the write access to the EE.
  * @param  None
  * @retval None
  */
void sEE_WriteEnable(void)
{
  /*!< Select the EE: Chip Select low */
  sEE_CS_LOW();

  /*!< Send "Write Enable" instruction */
  sEE_SendByte(sEE_CMD_WREN);

  /*!< Deselect the EE: Chip Select high */
  sEE_CS_HIGH();
}


/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the EE's
  *         status register and loop until write operation has completed.
  * @param  None
  * @retval None
  */
void sEE_WaitForWriteEnd(void)
{
  uint8_t EEstatus = 0;

  /*!< Select the EE: Chip Select low */
  sEE_CS_LOW();

  /*!< Send "Read Status Register" instruction */
  sEE_SendByte(sEE_CMD_RDSR);

  /*!< Loop as long as the memory is busy with a write cycle */
  do
  {
    /*!< Send a dummy byte to generate the clock needed by the EE
    and put the value of the status register in EE_Status variable */
    EEstatus = sEE_SendByte(sEE_DUMMY_BYTE);

  }
  while ((EEstatus & sEE_WIP_FLAG) == SET); /* Write in progress */

  /*!< Deselect the EE: Chip Select high */
  sEE_CS_HIGH();
}


/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t sEE_SendHalfWord(uint16_t HalfWord)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sEE_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send Half Word through the sEE peripheral */
  SPI_I2S_SendData(sEE_SPI, HalfWord);

  /*!< Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(sEE_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(sEE_SPI);
}
  
  
/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t sEE_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not empty */
  while (SPI_I2S_GetFlagStatus(sEE_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sEE_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sEE_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(sEE_SPI);
}

/**
  * @brief  Reads a byte from the SPI EE.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI EE.
  */
uint8_t sEE_ReadByte(void)
{
  return (sEE_SendByte(sEE_DUMMY_BYTE));
}

