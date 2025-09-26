/**
 ******************************************************************************
 * @file    system_init.c
 * @brief   System initialization module for STM32F411 USB-PS2 converter
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "system_init.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  System initialization function
 * @note   Initializes HAL, system clock, GPIO, and peripherals
 * @retval SYSTEM_OK if successful, SYSTEM_ERROR otherwise
 */
SystemStatus_t system_init(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick */
    if (HAL_Init() != HAL_OK) {
        return SYSTEM_ERROR;
    }

    /* Configure the system clock to 84 MHz */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM2_Init();

    return SYSTEM_OK;
}

/**
 * @brief  System Clock Configuration
 * @note   Configures system clock to 84 MHz using HSE (25 MHz crystal)
 *         SYSCLK = 84 MHz, AHB = 84 MHz, APB1 = 42 MHz, APB2 = 84 MHz
 * @retval None
 */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initializes the RCC Oscillators according to the specified parameters */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;        /* 25 MHz / 25 = 1 MHz */
    RCC_OscInitStruct.PLL.PLLN = 168;       /* 1 MHz * 168 = 168 MHz */
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  /* 168 MHz / 2 = 84 MHz */
    RCC_OscInitStruct.PLL.PLLQ = 7;         /* 168 MHz / 7 = 24 MHz (for USB) */

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;      /* 84 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;       /* 42 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;       /* 84 MHz */

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief  GPIO Initialization Function
 * @note   Initializes all GPIO pins used in the system
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Configure GPIO pin Output Level for Status LED */
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);

    /* Configure GPIO pin Output Level for PS/2 pins (initially high) */
    HAL_GPIO_WritePin(PS2_CLK_GPIO_Port, PS2_CLK_Pin|PS2_DATA_Pin, GPIO_PIN_SET);

    /* Configure Status LED Pin */
    GPIO_InitStruct.Pin = STATUS_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(STATUS_LED_GPIO_Port, &GPIO_InitStruct);

    /* Configure PS/2 Clock Pin */
    GPIO_InitStruct.Pin = PS2_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;     /* Open-drain for PS/2 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(PS2_CLK_GPIO_Port, &GPIO_InitStruct);

    /* Configure PS/2 Data Pin */
    GPIO_InitStruct.Pin = PS2_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;     /* Open-drain for PS/2 */
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(PS2_DATA_GPIO_Port, &GPIO_InitStruct);

    /* Configure USB OTG FS pins */
    GPIO_InitStruct.Pin = USB_OTG_FS_DM_Pin|USB_OTG_FS_DP_Pin|USB_OTG_FS_ID_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**
 * @brief  TIM2 Initialization Function  
 * @note   Initializes TIM2 for PS/2 timing (used for bit timing in PS/2 protocol)
 * @retval None
 */
static void MX_TIM2_Init(void)
{
    TIM_HandleTypeDef htim2;
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* Configure TIM2 for PS/2 bit timing */
    /* PS/2 clock frequency should be 10-16.7 kHz, we'll use ~12 kHz */
    /* Timer frequency = APB1_CLK * 2 / (Prescaler + 1) / (Period + 1) */
    /* 42 MHz * 2 / 3500 / 2 = 12 kHz */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 3499;                    /* Prescaler for timing */
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1;                          /* Period for PS/2 bit timing */
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

/**
 * @brief  Get system status
 * @retval Current system status
 */
SystemStatus_t system_get_status(void)
{
    /* Basic system health check */
    if (HAL_GetTick() == 0) {
        return SYSTEM_ERROR;
    }
    
    return SYSTEM_OK;
}

/**
 * @brief  Error Handler
 * @note   This function is executed in case of error occurrence
 * @retval None
 */
void Error_Handler(void)
{
    /* User can add their own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
        /* Toggle LED to indicate error */
        HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
        for (volatile uint32_t i = 0; i < 500000; i++);
    }
}

/**
 * @brief  This function handles System tick timer.
 * @retval None
 */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/**
 * @brief  This function handles TIM2 global interrupt.
 * @retval None
 */
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add their own implementation to report the file name and line number,
       example: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}