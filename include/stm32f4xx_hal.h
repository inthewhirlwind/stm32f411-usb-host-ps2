/**
 ******************************************************************************
 * @file    stm32f4xx_hal.h
 * @brief   This file contains all the functions prototypes for the HAL
 *          module driver.
 * @note    This is a minimal header for template purposes
 ******************************************************************************
 */

#ifndef __STM32F4XX_HAL_H
#define __STM32F4XX_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef enum {
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef enum {
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/* GPIO definitions */
typedef enum {
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
} GPIO_PinState;

#define GPIO_PIN_0                 ((uint16_t)0x0001)
#define GPIO_PIN_1                 ((uint16_t)0x0002)
#define GPIO_PIN_10                ((uint16_t)0x0400)
#define GPIO_PIN_11                ((uint16_t)0x0800)
#define GPIO_PIN_12                ((uint16_t)0x1000)
#define GPIO_PIN_13                ((uint16_t)0x2000)

typedef struct {
  uint32_t Pin;
  uint32_t Mode;
  uint32_t Pull;
  uint32_t Speed;
  uint32_t Alternate;
} GPIO_InitTypeDef;

typedef struct {
  volatile uint32_t MODER;
  volatile uint32_t OTYPER;
  volatile uint32_t OSPEEDR;
  volatile uint32_t PUPDR;
  volatile uint32_t IDR;
  volatile uint32_t ODR;
  volatile uint32_t BSRR;
  volatile uint32_t LCKR;
  volatile uint32_t AFR[2];
} GPIO_TypeDef;

/* Timer definitions */
typedef struct {
  uint32_t Prescaler;
  uint32_t CounterMode;
  uint32_t Period;
  uint32_t ClockDivision;
  uint32_t AutoReloadPreload;
} TIM_Base_InitTypeDef;

typedef struct {
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t SMCR;
  volatile uint32_t DIER;
  volatile uint32_t SR;
  volatile uint32_t EGR;
  volatile uint32_t CCMR1;
  volatile uint32_t CCMR2;
  volatile uint32_t CCER;
  volatile uint32_t CNT;
  volatile uint32_t PSC;
  volatile uint32_t ARR;
  volatile uint32_t RCR;
  volatile uint32_t CCR1;
  volatile uint32_t CCR2;
  volatile uint32_t CCR3;
  volatile uint32_t CCR4;
  volatile uint32_t BDTR;
  volatile uint32_t DCR;
  volatile uint32_t DMAR;
  volatile uint32_t OR;
} TIM_TypeDef;

typedef struct {
  TIM_TypeDef *Instance;
  TIM_Base_InitTypeDef Init;
  HAL_LockTypeDef Lock;
  uint32_t State;
} TIM_HandleTypeDef;

/* USB definitions */
typedef struct {
  uint32_t Host_channels;
  uint32_t speed;
  uint32_t dma_enable;
  uint32_t phy_itface;
  uint32_t Sof_enable;
  uint32_t low_power_enable;
  uint32_t vbus_sensing_enable;
  uint32_t use_external_vbus;
} HCD_InitTypeDef;

typedef struct {
  void *Instance;
  HCD_InitTypeDef Init;
  uint32_t State;
} HCD_HandleTypeDef;

typedef enum {
  URB_IDLE = 0,
  URB_DONE,
  URB_NOTREADY,
  URB_NYET,
  URB_ERROR,
  URB_STALL
} HCD_URBStateTypeDef;

typedef enum {
  HAL_HCD_STATE_RESET   = 0x00,
  HAL_HCD_STATE_READY   = 0x01,
  HAL_HCD_STATE_ERROR   = 0x02,
  HAL_HCD_STATE_BUSY    = 0x03,
  HAL_HCD_STATE_TIMEOUT = 0x04
} HCD_StateTypeDef;

/* Constants */
#define GPIO_MODE_OUTPUT_PP        0x00000001U
#define GPIO_MODE_OUTPUT_OD        0x00000011U
#define GPIO_MODE_AF_PP            0x00000002U
#define GPIO_NOPULL                0x00000000U
#define GPIO_PULLUP                0x00000001U
#define GPIO_SPEED_FREQ_LOW        0x00000000U
#define GPIO_SPEED_FREQ_MEDIUM     0x00000001U
#define GPIO_SPEED_FREQ_VERY_HIGH  0x00000003U

#define TIM_COUNTERMODE_UP         0x00000000U
#define TIM_CLOCKDIVISION_DIV1     0x00000000U
#define TIM_AUTORELOAD_PRELOAD_DISABLE  0x00000000U

#define HCD_SPEED_FULL             0x00000002U
#define DISABLE                    0U
#define ENABLE                     1U

#define GPIO_AF10_OTG_FS           ((uint8_t)0x0A)

/* Peripheral base addresses (dummy values) */
#define GPIOA_BASE            (0x40020000UL)
#define GPIOC_BASE            (0x40020800UL)
#define TIM2_BASE             (0x40000000UL)
#define USB_OTG_FS_BASE       (0x50000000UL)

#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOC                 ((GPIO_TypeDef *) GPIOC_BASE)
#define TIM2                  ((TIM_TypeDef *) TIM2_BASE)
#define USB_OTG_FS            ((void *) USB_OTG_FS_BASE)

/* External variables */
extern volatile uint32_t uwTick;
extern uint32_t SystemCoreClock;
extern uint32_t uwTickFreq;

/* Function prototypes */
HAL_StatusTypeDef HAL_Init(void);
void HAL_IncTick(void);
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t Delay);

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, void *sClockSourceConfig);
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *hhcd);
HCD_StateTypeDef HAL_HCD_GetState(HCD_HandleTypeDef *hhcd);
void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd);

/* Callback functions */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_HCD_HC_NotifyURBChangeCallback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state);
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd);

/* IRQ numbers (dummy values) */
#define OTG_FS_IRQn           67
#define TIM2_IRQn             28
#define PendSV_IRQn           -2
#define SysTick_IRQn          -1

/* NVIC functions */
void HAL_NVIC_SetPriority(int32_t IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(int32_t IRQn);
void HAL_NVIC_DisableIRQ(int32_t IRQn);

/* Clock control */
#define __HAL_RCC_GPIOA_CLK_ENABLE()    do { } while(0)
#define __HAL_RCC_GPIOC_CLK_ENABLE()    do { } while(0)
#define __HAL_RCC_TIM2_CLK_ENABLE()     do { } while(0)
#define __HAL_RCC_USB_OTG_FS_CLK_ENABLE() do { } while(0)
#define __HAL_RCC_USB_OTG_FS_CLK_DISABLE() do { } while(0)
#define __HAL_RCC_TIM2_CLK_DISABLE()    do { } while(0)
#define __HAL_RCC_SYSCFG_CLK_ENABLE()   do { } while(0)
#define __HAL_RCC_PWR_CLK_ENABLE()      do { } while(0)

/* Inline functions and macros */
#define __disable_irq()  do { } while(0)
#define __enable_irq()   do { } while(0)
#define __NOP()          do { } while(0)

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4XX_HAL_H */