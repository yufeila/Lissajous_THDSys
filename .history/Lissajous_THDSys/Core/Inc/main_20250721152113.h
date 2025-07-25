/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define THD_SIG_Pin GPIO_PIN_8
#define THD_SIG_GPIO_Port GPIOF
#define AD9833_1_FSYNC_Pin GPIO_PIN_0
#define AD9833_1_FSYNC_GPIO_Port GPIOC
#define AD9833_1_SCLK_Pin GPIO_PIN_1
#define AD9833_1_SCLK_GPIO_Port GPIOC
#define AD9833_1_SDATA_Pin GPIO_PIN_2
#define AD9833_1_SDATA_GPIO_Port GPIOC
#define LF_X_Pin GPIO_PIN_2
#define LF_X_GPIO_Port GPIOA
#define LF_Y_Pin GPIO_PIN_3
#define LF_Y_GPIO_Port GPIOA
#define LM339_VREF_Pin GPIO_PIN_4
#define LM339_VREF_GPIO_Port GPIOA
#define HC_VCOIN_Pin GPIO_PIN_5
#define HC_VCOIN_GPIO_Port GPIOA
#define AD9833_2_FSYNC_Pin GPIO_PIN_0
#define AD9833_2_FSYNC_GPIO_Port GPIOB
#define AD9833_2_SCLK_Pin GPIO_PIN_1
#define AD9833_2_SCLK_GPIO_Port GPIOB
#define AD9833_2_SDATA_Pin GPIO_PIN_2
#define AD9833_2_SDATA_GPIO_Port GPIOB
#define RELAY_Pin GPIO_PIN_12
#define RELAY_GPIO_Port GPIOB
#define LCD_BK_Pin GPIO_PIN_15
#define LCD_BK_GPIO_Port GPIOB
#define SQUARE_CAP_Pin GPIO_PIN_12
#define SQUARE_CAP_GPIO_Port GPIOD
#define AD9851_RESET_Pin GPIO_PIN_4
#define AD9851_RESET_GPIO_Port GPIOB
#define AD9851_BIT_DATA_Pin GPIO_PIN_5
#define AD9851_BIT_DATA_GPIO_Port GPIOB
#define AD9851_W_CLK_Pin GPIO_PIN_6
#define AD9851_W_CLK_GPIO_Port GPIOB
#define AD9851_FQ_UP_Pin GPIO_PIN_7
#define AD9851_FQ_UP_GPIO_Port GPIOB
#define KEY0_Pin GPIO_PIN_8
#define KEY0_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_9
#define KEY1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define TIME_200MS 200
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
