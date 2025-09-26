/**
 ******************************************************************************
 * @file    main.c
 * @brief   STM32F411 USB Host to PS/2 Converter - Main Application
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 * 
 * @description
 * This application configures the STM32F411CEU6 as a USB host to read data
 * from USB keyboards and translates the data to PS/2 protocol output.
 * 
 * Main features:
 * - USB OTG Host mode configuration for HID keyboard detection
 * - PS/2 protocol implementation with clock and data lines
 * - Real-time translation of USB HID scan codes to PS/2 scan codes
 * - Proper timing and signaling for PS/2 communication
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "system_init.h"
#include "usb_host_init.h"
#include "ps2_init.h"
#include "keyboard_handler.h"
#include "scancode_translator.h"

/* Private typedef -----------------------------------------------------------*/
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

/* Private define ------------------------------------------------------------*/
#define APP_VERSION_MAJOR    1
#define APP_VERSION_MINOR    0
#define APP_VERSION_PATCH    0

#define MAIN_LOOP_DELAY_MS   1      ///< Main loop delay in milliseconds
#define LED_BLINK_PERIOD_MS  1000   ///< Status LED blink period

/* Private macro -------------------------------------------------------------*/
/**
 * @brief Convert milliseconds to system ticks
 */
#define MS_TO_TICKS(ms)     ((ms) * (SystemCoreClock / 1000U))

/* Private variables ---------------------------------------------------------*/
static AppState_t app_state = APP_STATE_INIT;
static uint32_t system_tick_counter = 0;
static uint32_t led_last_toggle = 0;

/* Private function prototypes -----------------------------------------------*/
static void main_application_loop(void);
static void status_led_update(void);
static void system_status_check(void);
static void error_handler(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Main program entry point
 * @retval Program exit status (should never return)
 */
int main(void)
{
    /* Application startup banner */
    // Note: UART printf support would be added here for debugging
    
    /* System Clock Configuration and HAL Initialization */
    if (system_init() != SYSTEM_OK) {
        app_state = APP_STATE_ERROR;
        error_handler();
    }
    
    /* Initialize USB Host subsystem */
    app_state = APP_STATE_USB_INIT;
    if (usb_host_init() != USB_HOST_OK) {
        app_state = APP_STATE_ERROR;
        error_handler();
    }
    
    /* Initialize PS/2 interface */
    app_state = APP_STATE_PS2_INIT;
    if (ps2_init() != PS2_OK) {
        app_state = APP_STATE_ERROR;
        error_handler();
    }
    
    /* Initialize keyboard translation subsystem */
    if (scancode_translator_init() != TRANSLATOR_OK) {
        app_state = APP_STATE_ERROR;
        error_handler();
    }
    
    /* System ready - start main application loop */
    app_state = APP_STATE_READY;
    
    /* Main application loop */
    main_application_loop();
    
    /* Should never reach here */
    return 0;
}

/**
 * @brief  Main application infinite loop
 * @note   This function handles the continuous operation of the USB-to-PS/2 converter
 * @retval None
 */
static void main_application_loop(void)
{
    USB_HID_KeyboardData_t usb_keyboard_data;
    PS2_ScanCode_t ps2_scancode;
    
    app_state = APP_STATE_RUNNING;
    
    while (1) {
        /* Update system status indicators */
        status_led_update();
        system_status_check();
        
        /* Process USB Host events and keyboard input */
        usb_host_process();
        
        /* Check for new keyboard data from USB */
        if (keyboard_handler_get_data(&usb_keyboard_data) == KEYBOARD_DATA_AVAILABLE) {
            
            /* Translate USB HID scan codes to PS/2 scan codes */
            if (scancode_translator_usb_to_ps2(&usb_keyboard_data, &ps2_scancode) == TRANSLATOR_OK) {
                
                /* Send PS/2 scan code via PS/2 interface */
                if (ps2_send_scancode(&ps2_scancode) != PS2_OK) {
                    /* Handle PS/2 transmission error */
                    // Error handling could include retry logic or status reporting
                }
            }
        }
        
        /* Small delay to prevent overwhelming the system */
        HAL_Delay(MAIN_LOOP_DELAY_MS);
        system_tick_counter++;
    }
}

/**
 * @brief  Update status LED to indicate system state
 * @note   Provides visual feedback about the current system status
 * @retval None
 */
static void status_led_update(void)
{
    uint32_t current_time = HAL_GetTick();
    
    /* Update LED based on application state */
    switch (app_state) {
        case APP_STATE_INIT:
        case APP_STATE_USB_INIT:
        case APP_STATE_PS2_INIT:
            /* Fast blink during initialization */
            if ((current_time - led_last_toggle) > (LED_BLINK_PERIOD_MS / 4)) {
                HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
                led_last_toggle = current_time;
            }
            break;
            
        case APP_STATE_READY:
            /* Slow blink when ready but not processing data */
            if ((current_time - led_last_toggle) > LED_BLINK_PERIOD_MS) {
                HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
                led_last_toggle = current_time;
            }
            break;
            
        case APP_STATE_RUNNING:
            /* Solid on during normal operation */
            HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
            break;
            
        case APP_STATE_ERROR:
            /* Very fast blink for error indication */
            if ((current_time - led_last_toggle) > (LED_BLINK_PERIOD_MS / 8)) {
                HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
                led_last_toggle = current_time;
            }
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Check system status and handle state transitions
 * @note   Monitors USB and PS/2 subsystems for proper operation
 * @retval None
 */
static void system_status_check(void)
{
    /* Check USB Host status */
    if (usb_host_get_status() == USB_HOST_ERROR) {
        app_state = APP_STATE_ERROR;
        return;
    }
    
    /* Check PS/2 interface status */
    if (ps2_get_status() == PS2_ERROR) {
        app_state = APP_STATE_ERROR;
        return;
    }
    
    /* Update state based on USB device connection */
    if (app_state == APP_STATE_READY && usb_host_device_connected()) {
        app_state = APP_STATE_RUNNING;
    } else if (app_state == APP_STATE_RUNNING && !usb_host_device_connected()) {
        app_state = APP_STATE_READY;
    }
}

/**
 * @brief  Error handler function
 * @note   Called when a critical error occurs, provides basic error handling
 * @retval None (infinite loop)
 */
static void error_handler(void)
{
    /* Disable interrupts to prevent further issues */
    __disable_irq();
    
    /* Try to indicate error state via LED if possible */
    while (1) {
        /* Basic error indication - toggle LED rapidly */
        for (volatile uint32_t i = 0; i < 100000; i++);
        HAL_GPIO_TogglePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin);
    }
}

/**
 * @brief  System tick interrupt callback
 * @note   Called by SysTick handler, can be used for timing operations
 * @retval None
 */
void HAL_IncTick(void)
{
    /* Update HAL tick counter */
    uwTick += (uint32_t)uwTickFreq;
    
    /* Update keyboard handler timing */
    keyboard_handler_tick();
    
    /* Update PS/2 timing */
    ps2_tick();
}

/**
 * @brief  Application version information
 * @param  major: Pointer to store major version
 * @param  minor: Pointer to store minor version  
 * @param  patch: Pointer to store patch version
 * @retval None
 */
void app_get_version(uint8_t *major, uint8_t *minor, uint8_t *patch)
{
    if (major) *major = APP_VERSION_MAJOR;
    if (minor) *minor = APP_VERSION_MINOR;
    if (patch) *patch = APP_VERSION_PATCH;
}

/**
 * @brief  Get current application state
 * @retval Current application state
 */
AppState_t app_get_state(void)
{
    return app_state;
}

/* Hardware specific weak function implementations ---------------------------*/

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called when TIM interrupt is triggered
 * @param  htim: TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /* PS/2 timing callback */
    if (htim->Instance == PS2_TIMER) {
        ps2_timer_callback();
    }
}

/**
 * @brief  USB OTG interrupt callback
 * @note   This function is called when USB OTG interrupt is triggered
 * @param  hhcd: HCD handle
 * @retval None
 */
void HAL_HCD_HC_NotifyURBChangeCallback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
    /* Forward to USB Host handler */
    usb_host_urb_change_callback(hhcd, chnum, urb_state);
}