//sEEPROM_SPI.h

#ifndef __EEPROM_SPI_H_
#define __EEPROM_SPI_H_

#include "stm32f10x.h"


/* M25P SPI EE supported commands */  
#define sEE_CMD_WRITE          0x02  /* Write to Memory instruction */
#define sEE_CMD_WRSR           0x01  /* Write Status Register instruction */
#define sEE_CMD_WREN           0x06  /* Write enable instruction */
#define sEE_CMD_READ           0x03  /* Read from Memory instruction */
#define sEE_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define sEE_CMD_RDID           0x83  /* Read identification */
#define sEE_CMD_LID            0x82  /* Locks the Identification page in read-only mode*/

#define sEE_WIP_FLAG           0x01  /* Write In Progress (WIP) flag */

#define sEE_DUMMY_BYTE         0xA5
#define sEE_SPI_PAGESIZE       0x20

#define sEE_M25P128_ID         0x202018
#define sEE_M25P64_ID          0x202017
  
/* M25P EE SPI Interface pins  */  
#define sEE_SPI                           SPI1
#define sEE_SPI_CLK                       RCC_APB2Periph_SPI1
#define sEE_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define sEE_SPI_SCK_PIN                   GPIO_Pin_5
#define sEE_SPI_SCK_GPIO_PORT             GPIOA      // SCK - GPIOA5
#define sEE_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOA
#define sEE_SPI_SCK_SOURCE                GPIO_PinSource5
#define sEE_SPI_SCK_AF                    GPIO_AF_SPI1

#define sEE_SPI_MISO_PIN                  GPIO_Pin_6
#define sEE_SPI_MISO_GPIO_PORT            GPIOA
#define sEE_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOA
#define sEE_SPI_MISO_SOURCE               GPIO_PinSource6
#define sEE_SPI_MISO_AF                   GPIO_AF_SPI1

#define sEE_SPI_MOSI_PIN                  GPIO_Pin_7
#define sEE_SPI_MOSI_GPIO_PORT            GPIOA
#define sEE_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOA
#define sEE_SPI_MOSI_SOURCE               GPIO_PinSource7
#define sEE_SPI_MOSI_AF                   GPIO_AF_SPI1

#define sEE_CS_PIN                        GPIO_Pin_4
#define sEE_CS_GPIO_PORT                  GPIOA
#define sEE_CS_GPIO_CLK                   RCC_APB2Periph_GPIOA

/* Exported macro ------------------------------------------------------------*/
/* Select sEE: Chip Select pin low */
#define sEE_CS_LOW()       GPIO_ResetBits(sEE_CS_GPIO_PORT, sEE_CS_PIN)
/* Deselect sEE: Chip Select pin high */
#define sEE_CS_HIGH()      GPIO_SetBits(sEE_CS_GPIO_PORT, sEE_CS_PIN)   



void sEE_Init(void);
void sEE_ReadBuffer(uint8_t* pBuffer, uint16_t ReadAddr, uint16_t NumByteToRead);
uint8_t sEE_SendByte(uint8_t byte);
uint16_t sEE_SendHalfWord(uint16_t HalfWord);
uint8_t sEE_ReadByte(void);
void sEE_WriteBuffer(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void sEE_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);


#endif

