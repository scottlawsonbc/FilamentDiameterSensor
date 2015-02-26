/**
  ******************************************************************************
  * @file    I2C/I2C_TSENSOR/stm32f30x_it.c 
  * @author  MCD Application Team
  * @version V1.1.1
  * @date    31-October-2014
  * @brief   Main Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x_it.h"

/** @addtogroup STM32F30x_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2C_TSENSOR
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t SMbusAlertOccurred;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
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
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles I2C2 Error interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_ER_IRQHandler(void)
{
  /* Check on I2C2 Time out flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_TIMEOUT))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_TIMEOUT);
  }
  
  /* Check on I2C2 Arbitration Lost flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_ARLO))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_ARLO);
  }

  /* Check on I2C2 PEC error flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_PECERR))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_PECERR);
  }
  
  /* Check on I2C2 Overrun/Underrun error flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_OVR))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_OVR);
  }

  /* Check on I2C2 Bus error flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_BERR))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_BERR);
  }
  /* Check on I2C2 SMBALERT flag and clear it */
  if (I2C_GetITStatus(I2C2, I2C_IT_ALERT))
  {
    I2C_ClearITPendingBit(I2C2, I2C_IT_ALERT);
    SMbusAlertOccurred++;
  } 
}

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

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
