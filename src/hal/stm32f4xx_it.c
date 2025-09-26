/**
 ******************************************************************************
 * @file    stm32f4xx_it.c
 * @brief   Interrupt Service Routines for STM32F411 USB-PS2 converter
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

/* Cortex-M4 Processor Interruption and Exception Handlers ------------------*/

/**
 * @brief  This function handles Non maskable interrupt.
 * @retval None
 */
void NMI_Handler(void)
{
    while (1) {
        /* Non-maskable interrupt handler */
    }
}

/**
 * @brief  This function handles Hard fault interrupt.
 * @retval None
 */
void HardFault_Handler(void)
{
    while (1) {
        /* Hard fault handler - should not reach here in normal operation */
        /* Could add debugging code here to capture fault information */
    }
}

/**
 * @brief  This function handles Memory management fault.
 * @retval None
 */
void MemManage_Handler(void)
{
    while (1) {
        /* Memory management fault handler */
    }
}

/**
 * @brief  This function handles Pre-fetch fault, memory access fault.
 * @retval None
 */
void BusFault_Handler(void)
{
    while (1) {
        /* Bus fault handler */
    }
}

/**
 * @brief  This function handles Undefined instruction or illegal state.
 * @retval None
 */
void UsageFault_Handler(void)
{
    while (1) {
        /* Usage fault handler */
    }
}

/**
 * @brief  This function handles System service call via SWI instruction.
 * @retval None
 */
void SVC_Handler(void)
{
    /* System service call handler */
}

/**
 * @brief  This function handles Debug monitor.
 * @retval None
 */
void DebugMon_Handler(void)
{
    /* Debug monitor handler */
}

/**
 * @brief  This function handles Pendable request for system service.
 * @retval None
 */
void PendSV_Handler(void)
{
    /* PendSV handler */
}

/**
 * @brief  This function handles System tick timer.
 * @retval None
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/* STM32F4xx Peripheral Interrupt Handlers ----------------------------------*/

/**
 * @brief  This function handles TIM2 global interrupt.
 * @retval None
 */
void TIM2_IRQHandler(void)
{
    /* TIM2 interrupt handler - used for PS/2 timing */
    extern TIM_HandleTypeDef htim2;
    HAL_TIM_IRQHandler(&htim2);
}

/**
 * @brief  This function handles USB OTG FS global interrupt.
 * @retval None
 */
void OTG_FS_IRQHandler(void)
{
    /* USB OTG FS interrupt handler */
    extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
    HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
}