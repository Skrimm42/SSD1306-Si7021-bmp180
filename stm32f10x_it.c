/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "stm32f10x_it.h"
#include "const_var.h"
#include "user.h"
    
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

void TIM3_IRQHandler(void)
{ 
  if(TIM_GetITStatus(TIM3, TIM_IT_CC3) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
    
    if((prog_state | ~STATE_VEL) == ~STATE_VEL)//idle vel
    {
      prog_state = prog_state | STATE_VEL;
      IC3ReadValue1_ = TIM_GetCapture3(TIM3);
      Capture1 = 1;
      Count_stop_vel = 0;
      Fvel = 0;
      return;
    }
    else if((prog_state & STATE_VEL) == STATE_VEL)//work vel
    {
      IC3ReadValue1 = TIM_GetCapture3(TIM3);
      if(IC3ReadValue1_>IC3ReadValue1)Capture1 = 65535 - IC3ReadValue1_ + IC3ReadValue1;
      else Capture1 = IC3ReadValue1 - IC3ReadValue1_;
      IC3ReadValue1_ = IC3ReadValue1;   
      Impulse_wheel++;//Overall impulse amount during session
      Count_stop_vel = 0;
      standby_cntr = 0;
      Capture1_total+=Capture1;
      Fvel = 1;
    }
  }
  if(TIM_GetITStatus(TIM3, TIM_IT_CC4) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
    if((prog_state | ~STATE_CAD) == ~STATE_CAD) //idle cadence
    {
      prog_state = prog_state | STATE_CAD;
      IC3ReadValue2_ = TIM_GetCapture4(TIM3);
      Capture2 = 1;
      Count_stop_cad = 0;
      Fcad = 0;
      return;
    }
    else if((prog_state & STATE_CAD) == STATE_CAD)// work cadence
    {
      IC3ReadValue2 = TIM_GetCapture4(TIM3);
      if(IC3ReadValue2_ > IC3ReadValue2)Capture2 = 65535 - IC3ReadValue2_ + IC3ReadValue2;
      else Capture2 = IC3ReadValue2 - IC3ReadValue2_;
      IC3ReadValue2_ = IC3ReadValue2;
      Count_stop_cad = 0;
      Impulse_crank++;
      Capture2_total+=Capture2;
      Fcad = 1;
    }
  }
}


void TIM4_IRQHandler(void)
{ 
  if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) 
  {
    /* Clear TIM3 Capture compare interrupt pending bit */
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    user_program();
  }
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
