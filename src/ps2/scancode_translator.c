/**
 ******************************************************************************
 * @file    scancode_translator.c
 * @brief   USB HID to PS/2 scan code translator for STM32F411
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "scancode_translator.h"
#include "ps2_protocol.h"
#include "keyboard_handler.h"

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief USB to PS/2 key mapping structure
 */
typedef struct {
    uint8_t usb_key;        ///< USB HID key code
    uint8_t ps2_key;        ///< PS/2 scan code
    uint8_t is_extended;    ///< 1 if extended key, 0 otherwise
} KeyMapping_t;

/* Private define ------------------------------------------------------------*/
#define MAX_TRANSLATION_BUFFER  8   ///< Maximum simultaneous key translations

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static TranslatorStatus_t translator_status = TRANSLATOR_INIT;
static USB_HID_KeyboardData_t last_usb_state;

/* USB HID to PS/2 scan code mapping table */
static const KeyMapping_t key_mapping_table[] = {
    /* Letters */
    {USB_HID_KEY_A, 0x1C, 0},   {USB_HID_KEY_B, 0x32, 0},   {USB_HID_KEY_C, 0x21, 0},
    {USB_HID_KEY_D, 0x23, 0},   {USB_HID_KEY_E, 0x24, 0},   {USB_HID_KEY_F, 0x2B, 0},
    {USB_HID_KEY_G, 0x34, 0},   {USB_HID_KEY_H, 0x33, 0},   {USB_HID_KEY_I, 0x43, 0},
    {USB_HID_KEY_J, 0x3B, 0},   {USB_HID_KEY_K, 0x42, 0},   {USB_HID_KEY_L, 0x4B, 0},
    {USB_HID_KEY_M, 0x3A, 0},   {USB_HID_KEY_N, 0x31, 0},   {USB_HID_KEY_O, 0x44, 0},
    {USB_HID_KEY_P, 0x4D, 0},   {USB_HID_KEY_Q, 0x15, 0},   {USB_HID_KEY_R, 0x2D, 0},
    {USB_HID_KEY_S, 0x1B, 0},   {USB_HID_KEY_T, 0x2C, 0},   {USB_HID_KEY_U, 0x3C, 0},
    {USB_HID_KEY_V, 0x2A, 0},   {USB_HID_KEY_W, 0x1D, 0},   {USB_HID_KEY_X, 0x22, 0},
    {USB_HID_KEY_Y, 0x35, 0},   {USB_HID_KEY_Z, 0x1A, 0},
    
    /* Numbers */
    {USB_HID_KEY_1, 0x16, 0},   {USB_HID_KEY_2, 0x1E, 0},   {USB_HID_KEY_3, 0x26, 0},
    {USB_HID_KEY_4, 0x25, 0},   {USB_HID_KEY_5, 0x2E, 0},   {USB_HID_KEY_6, 0x36, 0},
    {USB_HID_KEY_7, 0x3D, 0},   {USB_HID_KEY_8, 0x3E, 0},   {USB_HID_KEY_9, 0x46, 0},
    {USB_HID_KEY_0, 0x45, 0},
    
    /* Special keys */
    {USB_HID_KEY_ENTER, 0x5A, 0},       {USB_HID_KEY_ESCAPE, 0x76, 0},
    {USB_HID_KEY_BACKSPACE, 0x66, 0},   {USB_HID_KEY_TAB, 0x0D, 0},
    {USB_HID_KEY_SPACE, 0x29, 0},
    
    /* Function keys */
    {USB_HID_KEY_F1, 0x05, 0},  {USB_HID_KEY_F2, 0x06, 0},  {USB_HID_KEY_F3, 0x04, 0},
    {USB_HID_KEY_F4, 0x0C, 0},  {USB_HID_KEY_F5, 0x03, 0},  {USB_HID_KEY_F6, 0x0B, 0},
    {USB_HID_KEY_F7, 0x83, 0},  {USB_HID_KEY_F8, 0x0A, 0},  {USB_HID_KEY_F9, 0x01, 0},
    {USB_HID_KEY_F10, 0x09, 0}, {USB_HID_KEY_F11, 0x78, 0}, {USB_HID_KEY_F12, 0x07, 0},
    
    /* Extended keys */
    {USB_HID_KEY_INSERT, 0x70, 1},      {USB_HID_KEY_HOME, 0x6C, 1},
    {USB_HID_KEY_PAGE_UP, 0x7D, 1},     {USB_HID_KEY_DELETE, 0x71, 1},
    {USB_HID_KEY_END, 0x69, 1},         {USB_HID_KEY_PAGE_DOWN, 0x7A, 1},
    {USB_HID_KEY_RIGHT_ARROW, 0x74, 1}, {USB_HID_KEY_LEFT_ARROW, 0x6B, 1},
    {USB_HID_KEY_DOWN_ARROW, 0x72, 1},  {USB_HID_KEY_UP_ARROW, 0x75, 1},
    
    /* End of table marker */
    {0x00, 0x00, 0}
};

/* Private function prototypes -----------------------------------------------*/
static uint8_t find_ps2_scancode(uint8_t usb_key, uint8_t *ps2_key, uint8_t *is_extended);
static TranslatorStatus_t translate_modifier_keys(uint8_t old_modifier, uint8_t new_modifier, 
                                                 PS2_ScanCode_t *scancodes, uint8_t *count);
static TranslatorStatus_t translate_regular_keys(const USB_HID_KeyboardData_t *old_state,
                                                const USB_HID_KeyboardData_t *new_state,
                                                PS2_ScanCode_t *scancodes, uint8_t *count);
static uint8_t is_key_in_array(uint8_t key, const uint8_t *array, uint8_t array_size);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initialize scan code translator
 * @note   Sets up the USB HID to PS/2 translation system
 * @retval TRANSLATOR_OK if successful, TRANSLATOR_ERROR otherwise
 */
TranslatorStatus_t scancode_translator_init(void)
{
    /* Clear last USB state */
    memset(&last_usb_state, 0, sizeof(USB_HID_KeyboardData_t));
    
    translator_status = TRANSLATOR_READY;
    return TRANSLATOR_OK;
}

/**
 * @brief  Translate USB HID keyboard data to PS/2 scan code
 * @note   Converts USB keyboard state to PS/2 scan codes
 * @param  usb_data: Pointer to USB HID keyboard data
 * @param  ps2_scancode: Pointer to store PS/2 scan code
 * @retval TRANSLATOR_OK if successful, TRANSLATOR_ERROR otherwise
 */
TranslatorStatus_t scancode_translator_usb_to_ps2(const USB_HID_KeyboardData_t *usb_data, 
                                                  PS2_ScanCode_t *ps2_scancode)
{
    PS2_ScanCode_t temp_scancodes[MAX_TRANSLATION_BUFFER];
    uint8_t scancode_count = 0;
    
    if (usb_data == NULL || ps2_scancode == NULL) {
        return TRANSLATOR_ERROR;
    }
    
    if (translator_status != TRANSLATOR_READY) {
        return TRANSLATOR_ERROR;
    }
    
    /* Translate modifier key changes */
    if (translate_modifier_keys(last_usb_state.modifier, usb_data->modifier, 
                               temp_scancodes, &scancode_count) != TRANSLATOR_OK) {
        return TRANSLATOR_ERROR;
    }
    
    /* Translate regular key changes */
    if (translate_regular_keys(&last_usb_state, usb_data, 
                              &temp_scancodes[scancode_count], 
                              &scancode_count) != TRANSLATOR_OK) {
        return TRANSLATOR_ERROR;
    }
    
    /* For simplicity, return the first scan code if multiple are generated */
    /* A more sophisticated implementation would queue multiple scan codes */
    if (scancode_count > 0) {
        ps2_copy_scancode(ps2_scancode, &temp_scancodes[0]);
    } else {
        /* No changes detected */
        ps2_scancode->length = 0;
    }
    
    /* Update last USB state */
    memcpy(&last_usb_state, usb_data, sizeof(USB_HID_KeyboardData_t));
    
    return TRANSLATOR_OK;
}

/**
 * @brief  Get translator status
 * @retval Current translator status
 */
TranslatorStatus_t scancode_translator_get_status(void)
{
    return translator_status;
}

/**
 * @brief  Reset translator state
 * @note   Clears internal state and reinitializes translator
 * @retval None
 */
void scancode_translator_reset(void)
{
    memset(&last_usb_state, 0, sizeof(USB_HID_KeyboardData_t));
    translator_status = TRANSLATOR_READY;
}

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Find PS/2 scan code for USB HID key
 * @note   Searches mapping table for corresponding PS/2 scan code
 * @param  usb_key: USB HID key code
 * @param  ps2_key: Pointer to store PS/2 scan code
 * @param  is_extended: Pointer to store extended key flag
 * @retval 1 if found, 0 otherwise
 */
static uint8_t find_ps2_scancode(uint8_t usb_key, uint8_t *ps2_key, uint8_t *is_extended)
{
    for (uint16_t i = 0; key_mapping_table[i].usb_key != 0; i++) {
        if (key_mapping_table[i].usb_key == usb_key) {
            *ps2_key = key_mapping_table[i].ps2_key;
            *is_extended = key_mapping_table[i].is_extended;
            return 1;
        }
    }
    
    return 0;
}

/**
 * @brief  Translate modifier key changes
 * @note   Generates PS/2 scan codes for modifier key press/release events
 * @param  old_modifier: Previous modifier state
 * @param  new_modifier: Current modifier state
 * @param  scancodes: Array to store generated scan codes
 * @param  count: Pointer to scan code count (input/output)
 * @retval TRANSLATOR_OK if successful, TRANSLATOR_ERROR otherwise
 */
static TranslatorStatus_t translate_modifier_keys(uint8_t old_modifier, uint8_t new_modifier, 
                                                 PS2_ScanCode_t *scancodes, uint8_t *count)
{
    uint8_t modifier_changes = old_modifier ^ new_modifier;
    uint8_t initial_count = *count;
    
    /* Check each modifier bit */
    if (modifier_changes & USB_HID_MODIFIER_LEFT_CTRL) {
        if (new_modifier & USB_HID_MODIFIER_LEFT_CTRL) {
            ps2_create_make_code(&scancodes[*count], 0x14);
        } else {
            ps2_create_break_code(&scancodes[*count], 0x14);
        }
        (*count)++;
    }
    
    if (modifier_changes & USB_HID_MODIFIER_LEFT_SHIFT) {
        if (new_modifier & USB_HID_MODIFIER_LEFT_SHIFT) {
            ps2_create_make_code(&scancodes[*count], 0x12);
        } else {
            ps2_create_break_code(&scancodes[*count], 0x12);
        }
        (*count)++;
    }
    
    if (modifier_changes & USB_HID_MODIFIER_LEFT_ALT) {
        if (new_modifier & USB_HID_MODIFIER_LEFT_ALT) {
            ps2_create_make_code(&scancodes[*count], 0x11);
        } else {
            ps2_create_break_code(&scancodes[*count], 0x11);
        }
        (*count)++;
    }
    
    if (modifier_changes & USB_HID_MODIFIER_RIGHT_SHIFT) {
        if (new_modifier & USB_HID_MODIFIER_RIGHT_SHIFT) {
            ps2_create_make_code(&scancodes[*count], 0x59);
        } else {
            ps2_create_break_code(&scancodes[*count], 0x59);
        }
        (*count)++;
    }
    
    if (modifier_changes & USB_HID_MODIFIER_RIGHT_CTRL) {
        if (new_modifier & USB_HID_MODIFIER_RIGHT_CTRL) {
            ps2_create_extended_make_code(&scancodes[*count], 0x14);
        } else {
            ps2_create_extended_break_code(&scancodes[*count], 0x14);
        }
        (*count)++;
    }
    
    if (modifier_changes & USB_HID_MODIFIER_RIGHT_ALT) {
        if (new_modifier & USB_HID_MODIFIER_RIGHT_ALT) {
            ps2_create_extended_make_code(&scancodes[*count], 0x11);
        } else {
            ps2_create_extended_break_code(&scancodes[*count], 0x11);
        }
        (*count)++;
    }
    
    /* Check buffer overflow */
    if (*count > MAX_TRANSLATION_BUFFER) {
        *count = initial_count; /* Revert count */
        return TRANSLATOR_ERROR;
    }
    
    return TRANSLATOR_OK;
}

/**
 * @brief  Translate regular key changes
 * @note   Generates PS/2 scan codes for regular key press/release events
 * @param  old_state: Previous keyboard state
 * @param  new_state: Current keyboard state
 * @param  scancodes: Array to store generated scan codes
 * @param  count: Pointer to scan code count (input/output)
 * @retval TRANSLATOR_OK if successful, TRANSLATOR_ERROR otherwise
 */
static TranslatorStatus_t translate_regular_keys(const USB_HID_KeyboardData_t *old_state,
                                                const USB_HID_KeyboardData_t *new_state,
                                                PS2_ScanCode_t *scancodes, uint8_t *count)
{
    uint8_t initial_count = *count;
    uint8_t ps2_key, is_extended;
    
    /* Check for key releases (keys in old state but not in new state) */
    for (uint8_t i = 0; i < old_state->key_count; i++) {
        if (!is_key_in_array(old_state->keys[i], new_state->keys, new_state->key_count)) {
            /* Key was released */
            if (find_ps2_scancode(old_state->keys[i], &ps2_key, &is_extended)) {
                if (is_extended) {
                    ps2_create_extended_break_code(&scancodes[*count], ps2_key);
                } else {
                    ps2_create_break_code(&scancodes[*count], ps2_key);
                }
                (*count)++;
                
                if (*count >= MAX_TRANSLATION_BUFFER) {
                    *count = initial_count;
                    return TRANSLATOR_ERROR;
                }
            }
        }
    }
    
    /* Check for key presses (keys in new state but not in old state) */
    for (uint8_t i = 0; i < new_state->key_count; i++) {
        if (!is_key_in_array(new_state->keys[i], old_state->keys, old_state->key_count)) {
            /* Key was pressed */
            if (find_ps2_scancode(new_state->keys[i], &ps2_key, &is_extended)) {
                if (is_extended) {
                    ps2_create_extended_make_code(&scancodes[*count], ps2_key);
                } else {
                    ps2_create_make_code(&scancodes[*count], ps2_key);
                }
                (*count)++;
                
                if (*count >= MAX_TRANSLATION_BUFFER) {
                    *count = initial_count;
                    return TRANSLATOR_ERROR;
                }
            }
        }
    }
    
    return TRANSLATOR_OK;
}

/**
 * @brief  Check if key is in array
 * @note   Helper function to check if a key exists in an array
 * @param  key: Key to search for
 * @param  array: Array to search in
 * @param  array_size: Size of the array
 * @retval 1 if key found, 0 otherwise
 */
static uint8_t is_key_in_array(uint8_t key, const uint8_t *array, uint8_t array_size)
{
    for (uint8_t i = 0; i < array_size; i++) {
        if (array[i] == key) {
            return 1;
        }
    }
    return 0;
}