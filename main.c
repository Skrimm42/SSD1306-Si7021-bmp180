/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "display.h"


/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
__IO void delay(__IO uint32_t nCount);
void GPIO_Setup(void);
void DisplayShowTestPage(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

GPIO_InitTypeDef GPIO_InitStructure;

int main(void)
{
  uint16_t cntr = 0;
  
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32f10x_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f10x.c file
  */     
  
  /* Initialize LEDs, Key Button, LCD and COM port(USART) available on
  STM3210X-EVAL board ******************************************************/
  
  
  /* Retarget the C library printf function to the USARTx, can be USART1 or USART2
  depending on the EVAL board you are using ********************************/
  
  /* Add your application code here
  */
  
  
  GPIO_Setup();
  InitDisplay(); //I2C1 init
  
  //DisplayShowTestPage();
  SSD1306_GotoXY(5, 3);
  SSD1306_Puts("Test counter: ", &Font_7x10, SSD1306_COLOR_WHITE);
  SSD1306_UpdateScreen();
  
  /* Infinite loop */
  while (1)
  {
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
    delay(700000);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    delay(500000);
    
    cntr++;
    SSD1306_GotoXY(20, 15);
    SSD1306_printf(&Font_16x26, "%d", cntr);
    //SSD1306_UpdateScreen();
    SSD1306_UpdateScreenDMA();
  }
}

void DisplayShowTestPage(void)
{
		PageContainer_t container;
		container.header = "TestHeader";
		container.line_one = "Line One";
		container.line_two = "Line Two";
		container.line_three = "Line Three";
		DrawPage(&container);
}


void GPIO_Setup(void)
{
    /* GPIOC Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* Configure PC13 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

__IO void delay(__IO uint32_t nCount)
{
 while(nCount--)
  {
  }
   
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
    
  }
}


#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
