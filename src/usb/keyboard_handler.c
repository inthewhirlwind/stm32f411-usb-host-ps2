/**
 ******************************************************************************
 * @file    keyboard_handler.c
 * @brief   USB HID keyboard data handler for STM32F411
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "keyboard_handler.h"
#include "usb_host_init.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define KEYBOARD_REPORT_SIZE        8       ///< Standard HID keyboard report size
#define KEYBOARD_MODIFIER_OFFSET    0       ///< Offset of modifier byte in report
#define KEYBOARD_KEY_OFFSET         2       ///< Offset of key data in report
#define KEYBOARD_MAX_KEYS           6       ///< Maximum simultaneous keys
#define KEYBOARD_BUFFER_SIZE        16      ///< Keyboard data buffer size

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static USB_HID_KeyboardData_t keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static volatile uint8_t buffer_head = 0;
static volatile uint8_t buffer_tail = 0;
static volatile uint8_t buffer_count = 0;
static USB_HID_KeyboardData_t last_keyboard_state;
static KeyboardHandlerStatus_t handler_status = KEYBOARD_HANDLER_INIT;

/* Private function prototypes -----------------------------------------------*/
static void keyboard_parse_hid_report(const uint8_t *report, USB_HID_KeyboardData_t *keyboard_data);
static uint8_t keyboard_buffer_is_full(void);
static uint8_t keyboard_buffer_is_empty(void);
static void keyboard_buffer_put(const USB_HID_KeyboardData_t *data);
static void keyboard_buffer_get(USB_HID_KeyboardData_t *data);
static void keyboard_compare_states(const USB_HID_KeyboardData_t *old_state, 
                                   const USB_HID_KeyboardData_t *new_state,
                                   USB_HID_KeyboardData_t *changes);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initialize keyboard handler
 * @note   Sets up keyboard data handling and buffering
 * @retval KEYBOARD_HANDLER_OK if successful, KEYBOARD_HANDLER_ERROR otherwise
 */
KeyboardHandlerStatus_t keyboard_handler_init(void)
{
    /* Initialize buffer pointers */
    buffer_head = 0;
    buffer_tail = 0;
    buffer_count = 0;
    
    /* Clear last keyboard state */
    memset(&last_keyboard_state, 0, sizeof(USB_HID_KeyboardData_t));
    
    handler_status = KEYBOARD_HANDLER_READY;
    return KEYBOARD_HANDLER_OK;
}

/**
 * @brief  Process USB HID keyboard report
 * @note   Parses USB HID report and stores keyboard data
 * @param  report: Pointer to USB HID report data
 * @param  report_size: Size of HID report
 * @retval KEYBOARD_HANDLER_OK if successful, KEYBOARD_HANDLER_ERROR otherwise
 */
KeyboardHandlerStatus_t keyboard_handler_process_report(const uint8_t *report, uint16_t report_size)
{
    USB_HID_KeyboardData_t keyboard_data;
    
    if (report == NULL || report_size != KEYBOARD_REPORT_SIZE) {
        return KEYBOARD_HANDLER_ERROR;
    }
    
    /* Parse HID report into keyboard data structure */
    keyboard_parse_hid_report(report, &keyboard_data);
    
    /* Check if keyboard state has changed */
    if (memcmp(&keyboard_data, &last_keyboard_state, sizeof(USB_HID_KeyboardData_t)) != 0) {
        
        /* Store new keyboard data if buffer has space */
        if (!keyboard_buffer_is_full()) {
            keyboard_buffer_put(&keyboard_data);
            
            /* Update last state */
            memcpy(&last_keyboard_state, &keyboard_data, sizeof(USB_HID_KeyboardData_t));
        } else {
            /* Buffer full - could implement overflow handling here */
            return KEYBOARD_HANDLER_BUFFER_FULL;
        }
    }
    
    return KEYBOARD_HANDLER_OK;
}

/**
 * @brief  Get keyboard data from buffer
 * @note   Retrieves next available keyboard data from internal buffer
 * @param  keyboard_data: Pointer to store keyboard data
 * @retval KEYBOARD_DATA_AVAILABLE if data available, KEYBOARD_NO_DATA otherwise
 */
KeyboardDataStatus_t keyboard_handler_get_data(USB_HID_KeyboardData_t *keyboard_data)
{
    if (keyboard_data == NULL) {
        return KEYBOARD_NO_DATA;
    }
    
    if (keyboard_buffer_is_empty()) {
        return KEYBOARD_NO_DATA;
    }
    
    keyboard_buffer_get(keyboard_data);
    return KEYBOARD_DATA_AVAILABLE;
}

/**
 * @brief  Get keyboard handler status
 * @retval Current keyboard handler status
 */
KeyboardHandlerStatus_t keyboard_handler_get_status(void)
{
    return handler_status;
}

/**
 * @brief  Keyboard handler tick function
 * @note   Called from system tick for timing operations
 * @retval None
 */
void keyboard_handler_tick(void)
{
    /* Update handler timing if needed */
    /* Could be used for timeout handling or periodic operations */
}

/**
 * @brief  Clear keyboard buffer
 * @note   Removes all pending keyboard data from buffer
 * @retval None
 */
void keyboard_handler_clear_buffer(void)
{
    __disable_irq();
    buffer_head = 0;
    buffer_tail = 0;
    buffer_count = 0;
    __enable_irq();
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Parse USB HID keyboard report
 * @note   Converts USB HID report format to internal keyboard data structure
 * @param  report: Pointer to USB HID report
 * @param  keyboard_data: Pointer to store parsed keyboard data
 * @retval None
 */
static void keyboard_parse_hid_report(const uint8_t *report, USB_HID_KeyboardData_t *keyboard_data)
{
    /* Clear keyboard data structure */
    memset(keyboard_data, 0, sizeof(USB_HID_KeyboardData_t));
    
    /* Extract modifier keys (byte 0) */
    keyboard_data->modifier = report[KEYBOARD_MODIFIER_OFFSET];
    
    /* Extract regular keys (bytes 2-7) */
    keyboard_data->key_count = 0;
    for (uint8_t i = 0; i < KEYBOARD_MAX_KEYS; i++) {
        uint8_t key_code = report[KEYBOARD_KEY_OFFSET + i];
        
        if (key_code != 0 && key_code != 0x01) { /* Ignore null and error codes */
            keyboard_data->keys[keyboard_data->key_count] = key_code;
            keyboard_data->key_count++;
        }
    }
}

/**
 * @brief  Check if keyboard buffer is full
 * @retval 1 if buffer is full, 0 otherwise
 */
static uint8_t keyboard_buffer_is_full(void)
{
    return (buffer_count >= KEYBOARD_BUFFER_SIZE);
}

/**
 * @brief  Check if keyboard buffer is empty
 * @retval 1 if buffer is empty, 0 otherwise
 */
static uint8_t keyboard_buffer_is_empty(void)
{
    return (buffer_count == 0);
}

/**
 * @brief  Put keyboard data into buffer
 * @note   Adds keyboard data to circular buffer
 * @param  data: Pointer to keyboard data to store
 * @retval None
 */
static void keyboard_buffer_put(const USB_HID_KeyboardData_t *data)
{
    __disable_irq();
    
    if (!keyboard_buffer_is_full()) {
        memcpy(&keyboard_buffer[buffer_head], data, sizeof(USB_HID_KeyboardData_t));
        buffer_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        buffer_count++;
    }
    
    __enable_irq();
}

/**
 * @brief  Get keyboard data from buffer
 * @note   Retrieves keyboard data from circular buffer
 * @param  data: Pointer to store retrieved keyboard data
 * @retval None
 */
static void keyboard_buffer_get(USB_HID_KeyboardData_t *data)
{
    __disable_irq();
    
    if (!keyboard_buffer_is_empty()) {
        memcpy(data, &keyboard_buffer[buffer_tail], sizeof(USB_HID_KeyboardData_t));
        buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
        buffer_count--;
    }
    
    __enable_irq();
}

/**
 * @brief  Compare keyboard states and find changes
 * @note   Identifies which keys have been pressed or released
 * @param  old_state: Previous keyboard state
 * @param  new_state: Current keyboard state
 * @param  changes: Structure to store detected changes
 * @retval None
 */
static void keyboard_compare_states(const USB_HID_KeyboardData_t *old_state, 
                                   const USB_HID_KeyboardData_t *new_state,
                                   USB_HID_KeyboardData_t *changes)
{
    /* Clear changes structure */
    memset(changes, 0, sizeof(USB_HID_KeyboardData_t));
    
    /* Check modifier changes */
    changes->modifier = old_state->modifier ^ new_state->modifier;
    
    /* Check key changes */
    /* This is a simplified implementation - a more sophisticated version
       would track individual key press/release events */
    changes->key_count = new_state->key_count;
    for (uint8_t i = 0; i < new_state->key_count; i++) {
        changes->keys[i] = new_state->keys[i];
    }
}

/**
 * @brief  Check if specific key is pressed
 * @note   Checks if a specific USB HID key code is currently pressed
 * @param  keyboard_data: Pointer to keyboard data
 * @param  key_code: USB HID key code to check
 * @retval 1 if key is pressed, 0 otherwise
 */
uint8_t keyboard_is_key_pressed(const USB_HID_KeyboardData_t *keyboard_data, uint8_t key_code)
{
    if (keyboard_data == NULL) {
        return 0;
    }
    
    for (uint8_t i = 0; i < keyboard_data->key_count; i++) {
        if (keyboard_data->keys[i] == key_code) {
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief  Check if modifier key is pressed
 * @note   Checks if a specific modifier key is currently pressed
 * @param  keyboard_data: Pointer to keyboard data
 * @param  modifier_mask: Modifier mask to check
 * @retval 1 if modifier is pressed, 0 otherwise
 */
uint8_t keyboard_is_modifier_pressed(const USB_HID_KeyboardData_t *keyboard_data, uint8_t modifier_mask)
{
    if (keyboard_data == NULL) {
        return 0;
    }
    
    return (keyboard_data->modifier & modifier_mask) ? 1 : 0;
}