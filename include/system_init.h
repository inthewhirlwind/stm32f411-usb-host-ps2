/**
 ******************************************************************************
 * @file    system_init.h
 * @brief   Header for system_init.c - System initialization
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __SYSTEM_INIT_H
#define __SYSTEM_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief System status enumeration
 */
typedef enum {
    SYSTEM_OK = 0,      ///< System operation successful
    SYSTEM_ERROR        ///< System operation failed
} SystemStatus_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
SystemStatus_t system_init(void);
SystemStatus_t system_get_status(void);
void Error_Handler(void);

/* HAL interrupt handlers */
void SysTick_Handler(void);
void TIM2_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_INIT_H */