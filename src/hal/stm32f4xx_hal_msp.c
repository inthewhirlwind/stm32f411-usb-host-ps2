/**
 ******************************************************************************
 * @file    stm32f4xx_hal_msp.c
 * @brief   HAL MSP module for STM32F411 USB-PS2 converter
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initializes the Global MSP.
 * @retval None
 */
void HAL_MspInit(void)
{
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();
    
    /* System interrupt init*/
    /* PendSV_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
    
    /* SysTick_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/**
 * @brief  DeInitializes the Global MSP.
 * @retval None
 */
void HAL_MspDeInit(void)
{
    /* Reset peripherals */
    __HAL_RCC_APB1_FORCE_RESET();
    __HAL_RCC_APB1_RELEASE_RESET();
    __HAL_RCC_APB2_FORCE_RESET();
    __HAL_RCC_APB2_RELEASE_RESET();
    __HAL_RCC_AHB1_FORCE_RESET();
    __HAL_RCC_AHB1_RELEASE_RESET();
}

/**
 * @brief  Initialize HCD MSP.
 * @param  hhcd: HCD handle
 * @retval None
 */
void HAL_HCD_MspInit(HCD_HandleTypeDef* hhcd)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if(hhcd->Instance == USB_OTG_FS)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        /* Configure USB OTG FS pins */
        GPIO_InitStruct.Pin = USB_OTG_FS_DM_Pin|USB_OTG_FS_DP_Pin|USB_OTG_FS_ID_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
        /* Peripheral clock enable */
        __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
        
        /* Enable USB OTG FS interrupt */
        HAL_NVIC_SetPriority(OTG_FS_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
    }
}

/**
 * @brief  DeInitialize HCD MSP.
 * @param  hhcd: HCD handle
 * @retval None
 */
void HAL_HCD_MspDeInit(HCD_HandleTypeDef* hhcd)
{
    if(hhcd->Instance == USB_OTG_FS)
    {
        /* Peripheral clock disable */
        __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
        
        /* Configure USB OTG FS pins */
        HAL_GPIO_DeInit(GPIOA, USB_OTG_FS_DM_Pin|USB_OTG_FS_DP_Pin|USB_OTG_FS_ID_Pin);
        
        /* Disable USB OTG FS interrupt */
        HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
    }
}

/**
 * @brief  Initialize TIM MSP.
 * @param  htim: TIM handle
 * @retval None
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == TIM2)
    {
        /* Peripheral clock enable */
        __HAL_RCC_TIM2_CLK_ENABLE();
        
        /* TIM2 interrupt Init */
        HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
    }
}

/**
 * @brief  DeInitialize TIM MSP.
 * @param  htim: TIM handle
 * @retval None
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim)
{
    if(htim->Instance == TIM2)
    {
        /* Peripheral clock disable */
        __HAL_RCC_TIM2_CLK_DISABLE();
        
        /* TIM2 interrupt DeInit */
        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
}