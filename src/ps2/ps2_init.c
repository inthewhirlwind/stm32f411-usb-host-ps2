/**
 ******************************************************************************
 * @file    ps2_init.c
 * @brief   PS/2 interface initialization and management for STM32F411
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ps2_init.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define PS2_CLOCK_FREQ_HZ       12000   ///< PS/2 clock frequency (10-16.7 kHz range)
#define PS2_BIT_PERIOD_US       83      ///< Bit period in microseconds (1/12kHz)
#define PS2_START_BIT           0       ///< PS/2 start bit value
#define PS2_STOP_BIT            1       ///< PS/2 stop bit value

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static PS2_Status_t ps2_status = PS2_INIT;
static TIM_HandleTypeDef htim_ps2;
static volatile uint8_t ps2_timer_active = 0;

/* Private function prototypes -----------------------------------------------*/
static void PS2_GPIO_Config(void);
static void PS2_Timer_Config(void);
static void PS2_Reset_Lines(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initialize PS/2 interface
 * @note   Configures GPIO pins and timer for PS/2 protocol communication
 * @retval PS2_OK if successful, PS2_ERROR otherwise
 */
PS2_Status_t ps2_init(void)
{
    ps2_status = PS2_INIT;
    
    /* Configure PS/2 GPIO pins */
    PS2_GPIO_Config();
    
    /* Configure PS/2 timing timer */
    PS2_Timer_Config();
    
    /* Reset PS/2 lines to idle state */
    PS2_Reset_Lines();
    
    /* Small delay to ensure lines are stable */
    HAL_Delay(10);
    
    ps2_status = PS2_READY;
    return PS2_OK;
}

/**
 * @brief  Configure PS/2 GPIO pins
 * @note   Sets up clock and data pins as open-drain outputs with pull-up
 * @retval None
 */
static void PS2_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* PS/2 Clock pin configuration */
    GPIO_InitStruct.Pin = PS2_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;     /* Open-drain output */
    GPIO_InitStruct.Pull = GPIO_PULLUP;             /* Internal pull-up */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(PS2_CLK_GPIO_Port, &GPIO_InitStruct);
    
    /* PS/2 Data pin configuration */
    GPIO_InitStruct.Pin = PS2_DATA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;     /* Open-drain output */
    GPIO_InitStruct.Pull = GPIO_PULLUP;             /* Internal pull-up */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(PS2_DATA_GPIO_Port, &GPIO_InitStruct);
}

/**
 * @brief  Configure PS/2 timing timer
 * @note   Sets up TIM2 for PS/2 bit timing generation
 * @retval None
 */
static void PS2_Timer_Config(void)
{
    /* Timer is already configured in system_init.c */
    htim_ps2.Instance = PS2_TIMER;
    
    /* Timer will be started when needed for transmission */
}

/**
 * @brief  Reset PS/2 lines to idle state
 * @note   Sets both clock and data lines high (idle state)
 * @retval None
 */
static void PS2_Reset_Lines(void)
{
    /* Set both lines high (idle state) */
    HAL_GPIO_WritePin(PS2_CLK_GPIO_Port, PS2_CLK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(PS2_DATA_GPIO_Port, PS2_DATA_Pin, GPIO_PIN_SET);
}

/**
 * @brief  Send PS/2 scan code
 * @note   Transmits a scan code using PS/2 protocol timing
 * @param  scancode: Pointer to PS/2 scan code structure
 * @retval PS2_OK if successful, PS2_ERROR otherwise
 */
PS2_Status_t ps2_send_scancode(const PS2_ScanCode_t *scancode)
{
    if (scancode == NULL || ps2_status != PS2_READY) {
        return PS2_ERROR;
    }
    
    ps2_status = PS2_TRANSMITTING;
    
    /* Send each byte in the scan code */
    for (uint8_t i = 0; i < scancode->length; i++) {
        if (ps2_send_byte(scancode->data[i]) != PS2_OK) {
            ps2_status = PS2_ERROR;
            return PS2_ERROR;
        }
    }
    
    ps2_status = PS2_READY;
    return PS2_OK;
}

/**
 * @brief  Send a single byte via PS/2 protocol
 * @note   Transmits one byte with proper PS/2 framing (start, data, parity, stop)
 * @param  data: Byte to transmit
 * @retval PS2_OK if successful, PS2_ERROR otherwise
 */
PS2_Status_t ps2_send_byte(uint8_t data)
{
    uint8_t parity = 1;  /* Odd parity calculation */
    uint8_t bit_count;
    
    /* Calculate odd parity */
    for (bit_count = 0; bit_count < 8; bit_count++) {
        if (data & (1 << bit_count)) {
            parity ^= 1;
        }
    }
    
    /* Start bit (0) */
    ps2_send_bit(0);
    
    /* Data bits (LSB first) */
    for (bit_count = 0; bit_count < 8; bit_count++) {
        ps2_send_bit((data >> bit_count) & 1);
    }
    
    /* Parity bit */
    ps2_send_bit(parity);
    
    /* Stop bit (1) */
    ps2_send_bit(1);
    
    return PS2_OK;
}

/**
 * @brief  Send a single bit via PS/2 protocol
 * @note   Transmits one bit with proper PS/2 timing
 * @param  bit_value: Bit value to transmit (0 or 1)
 * @retval None
 */
void ps2_send_bit(uint8_t bit_value)
{
    /* Set data line to bit value */
    HAL_GPIO_WritePin(PS2_DATA_GPIO_Port, PS2_DATA_Pin, 
                     bit_value ? GPIO_PIN_SET : GPIO_PIN_RESET);
    
    /* Clock low for half bit period */
    HAL_GPIO_WritePin(PS2_CLK_GPIO_Port, PS2_CLK_Pin, GPIO_PIN_RESET);
    ps2_delay_us(PS2_BIT_PERIOD_US / 2);
    
    /* Clock high for half bit period */
    HAL_GPIO_WritePin(PS2_CLK_GPIO_Port, PS2_CLK_Pin, GPIO_PIN_SET);
    ps2_delay_us(PS2_BIT_PERIOD_US / 2);
}

/**
 * @brief  Microsecond delay for PS/2 timing
 * @note   Provides precise timing for PS/2 protocol
 * @param  microseconds: Delay time in microseconds
 * @retval None
 */
void ps2_delay_us(uint32_t microseconds)
{
    /* Simple delay loop - not cycle-perfect but adequate for PS/2 timing */
    /* At 84 MHz, approximately 84 cycles per microsecond */
    volatile uint32_t cycles = microseconds * 21; /* Approximate cycles for delay */
    
    while (cycles--) {
        __NOP();
    }
}

/**
 * @brief  Get PS/2 interface status
 * @retval Current PS/2 status
 */
PS2_Status_t ps2_get_status(void)
{
    return ps2_status;
}

/**
 * @brief  PS/2 tick function for timing
 * @note   Called from system tick to update PS/2 timing
 * @retval None
 */
void ps2_tick(void)
{
    /* Update PS/2 timing if needed */
    /* This can be used for timeout handling or other timing-related operations */
}

/**
 * @brief  PS/2 timer callback
 * @note   Called when PS/2 timer interrupt occurs
 * @retval None
 */
void ps2_timer_callback(void)
{
    /* Handle PS/2 timer interrupt */
    if (ps2_timer_active) {
        /* Timer callback handling for bit timing */
        ps2_timer_active = 0;
    }
}

/**
 * @brief  Check PS/2 line states
 * @note   Reads current state of PS/2 clock and data lines
 * @param  clock_state: Pointer to store clock line state
 * @param  data_state: Pointer to store data line state
 * @retval None
 */
void ps2_read_lines(uint8_t *clock_state, uint8_t *data_state)
{
    if (clock_state != NULL) {
        *clock_state = HAL_GPIO_ReadPin(PS2_CLK_GPIO_Port, PS2_CLK_Pin);
    }
    
    if (data_state != NULL) {
        *data_state = HAL_GPIO_ReadPin(PS2_DATA_GPIO_Port, PS2_DATA_Pin);
    }
}

/**
 * @brief  Set PS/2 line states
 * @note   Manually control PS/2 clock and data lines
 * @param  clock_state: Clock line state (0 or 1)
 * @param  data_state: Data line state (0 or 1)
 * @retval None
 */
void ps2_set_lines(uint8_t clock_state, uint8_t data_state)
{
    HAL_GPIO_WritePin(PS2_CLK_GPIO_Port, PS2_CLK_Pin, 
                     clock_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(PS2_DATA_GPIO_Port, PS2_DATA_Pin,
                     data_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}