/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define oSol_Valve_b_Heade_Pin GPIO_PIN_1
#define oSol_Valve_b_Heade_GPIO_Port GPIOC
#define oSol_Valve_a_Heade_Pin GPIO_PIN_2
#define oSol_Valve_a_Heade_GPIO_Port GPIOC
#define iPedal_L_Pin GPIO_PIN_0
#define iPedal_L_GPIO_Port GPIOA
#define iPedal_R_Pin GPIO_PIN_2
#define iPedal_R_GPIO_Port GPIOA
#define oBuzer_Pin GPIO_PIN_4
#define oBuzer_GPIO_Port GPIOA
#define oSTLED316S_STB5_Pin GPIO_PIN_6
#define oSTLED316S_STB5_GPIO_Port GPIOA
#define oSTLED316S_STB1_Pin GPIO_PIN_4
#define oSTLED316S_STB1_GPIO_Port GPIOC
#define oSTLED316S_STB2_Pin GPIO_PIN_5
#define oSTLED316S_STB2_GPIO_Port GPIOC
#define oSTLED316S_STB3_Pin GPIO_PIN_0
#define oSTLED316S_STB3_GPIO_Port GPIOB
#define oSTLED316S_STB4_Pin GPIO_PIN_1
#define oSTLED316S_STB4_GPIO_Port GPIOB
#define iSTLED316S_IRQN5_Pin GPIO_PIN_11
#define iSTLED316S_IRQN5_GPIO_Port GPIOB
#define iSTLED316S_IRQN5_EXTI_IRQn EXTI15_10_IRQn
#define iBackDoor_Pin GPIO_PIN_12
#define iBackDoor_GPIO_Port GPIOB
#define oPWM_STEP1_Pin GPIO_PIN_6
#define oPWM_STEP1_GPIO_Port GPIOC
#define oDIR1_Pin GPIO_PIN_8
#define oDIR1_GPIO_Port GPIOC
#define oSyncArc_Pin GPIO_PIN_8
#define oSyncArc_GPIO_Port GPIOA
#define oSMD_EN1_Pin GPIO_PIN_11
#define oSMD_EN1_GPIO_Port GPIOA
#define iFB_STEP1_Pin GPIO_PIN_12
#define iFB_STEP1_GPIO_Port GPIOC
#define iFB_STEP1_EXTI_IRQn EXTI15_10_IRQn
#define iSTLED316S_IRQN3_Pin GPIO_PIN_2
#define iSTLED316S_IRQN3_GPIO_Port GPIOD
#define iSTLED316S_IRQN3_EXTI_IRQn EXTI2_IRQn
#define oSolenoidValve2_Pin GPIO_PIN_6
#define oSolenoidValve2_GPIO_Port GPIOB
#define oSolenoidValve1_Pin GPIO_PIN_7
#define oSolenoidValve1_GPIO_Port GPIOB
#define iCarriageStop_Pin GPIO_PIN_9
#define iCarriageStop_GPIO_Port GPIOB
#define iCarriageStop_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
