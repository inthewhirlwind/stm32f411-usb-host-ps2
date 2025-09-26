/**
 ******************************************************************************
 * @file    main.h
 * @brief   Header for main.c file - STM32F411 USB Host to PS/2 Converter
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Application state enumeration
 */
typedef enum {
    APP_STATE_INIT = 0,         ///< Application initialization
    APP_STATE_USB_INIT,         ///< USB Host initialization
    APP_STATE_PS2_INIT,         ///< PS/2 interface initialization
    APP_STATE_READY,            ///< System ready for operation
    APP_STATE_RUNNING,          ///< Normal operation mode
    APP_STATE_ERROR             ///< Error state
} AppState_t;

/* Exported constants --------------------------------------------------------*/

/* GPIO Pin Definitions */
#define STATUS_LED_Pin          GPIO_PIN_13
#define STATUS_LED_GPIO_Port    GPIOC

/* PS/2 Interface Pins */
#define PS2_CLK_Pin             GPIO_PIN_0
#define PS2_CLK_GPIO_Port       GPIOA
#define PS2_DATA_Pin            GPIO_PIN_1  
#define PS2_DATA_GPIO_Port      GPIOA

/* USB OTG FS Pins */
#define USB_OTG_FS_DM_Pin       GPIO_PIN_11
#define USB_OTG_FS_DM_GPIO_Port GPIOA
#define USB_OTG_FS_DP_Pin       GPIO_PIN_12
#define USB_OTG_FS_DP_GPIO_Port GPIOA
#define USB_OTG_FS_ID_Pin       GPIO_PIN_10
#define USB_OTG_FS_ID_GPIO_Port GPIOA

/* Timer for PS/2 timing */
#define PS2_TIMER               TIM2

/* Crystal oscillator values */
#define HSE_VALUE               25000000U   ///< External High Speed oscillator (HSE) value
#define HSI_VALUE               16000000U   ///< Internal High Speed oscillator (HSI) value
#define LSE_VALUE               32768U      ///< External Low Speed oscillator (LSE) value
#define LSI_VALUE               32000U      ///< Internal Low Speed oscillator (LSI) value

/* System configuration */
#define SYSTEM_CLOCK_FREQ       84000000U   ///< System clock frequency in Hz (84 MHz)
#define APB1_CLOCK_FREQ         42000000U   ///< APB1 clock frequency in Hz (42 MHz) 
#define APB2_CLOCK_FREQ         84000000U   ///< APB2 clock frequency in Hz (84 MHz)

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief  Assert parameter macro
 */
#ifdef USE_FULL_ASSERT
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void app_get_version(uint8_t *major, uint8_t *minor, uint8_t *patch);
AppState_t app_get_state(void);

/* HAL callback function prototypes */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_HCD_HC_NotifyURBChangeCallback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */