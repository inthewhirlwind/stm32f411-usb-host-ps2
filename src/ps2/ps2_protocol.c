/**
 ******************************************************************************
 * @file    ps2_protocol.c
 * @brief   PS/2 protocol implementation and scan code definitions
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "ps2_protocol.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Create PS/2 scan code structure
 * @note   Initializes a PS/2 scan code with given data
 * @param  scancode: Pointer to PS/2 scan code structure
 * @param  data: Pointer to scan code data bytes
 * @param  length: Number of bytes in scan code
 * @retval PS2_PROTOCOL_OK if successful, PS2_PROTOCOL_ERROR otherwise
 */
PS2_ProtocolStatus_t ps2_create_scancode(PS2_ScanCode_t *scancode, 
                                        const uint8_t *data, 
                                        uint8_t length)
{
    if (scancode == NULL || data == NULL || 
        length == 0 || length > PS2_MAX_SCANCODE_LENGTH) {
        return PS2_PROTOCOL_ERROR;
    }
    
    scancode->length = length;
    for (uint8_t i = 0; i < length; i++) {
        scancode->data[i] = data[i];
    }
    
    return PS2_PROTOCOL_OK;
}

/**
 * @brief  Create make code (key press) scan code
 * @note   Creates a PS/2 make code for key press events
 * @param  scancode: Pointer to PS/2 scan code structure
 * @param  key_code: PS/2 key code
 * @retval PS2_PROTOCOL_OK if successful, PS2_PROTOCOL_ERROR otherwise
 */
PS2_ProtocolStatus_t ps2_create_make_code(PS2_ScanCode_t *scancode, uint8_t key_code)
{
    if (scancode == NULL) {
        return PS2_PROTOCOL_ERROR;
    }
    
    /* Standard make code is just the key code */
    scancode->data[0] = key_code;
    scancode->length = 1;
    
    return PS2_PROTOCOL_OK;
}

/**
 * @brief  Create break code (key release) scan code
 * @note   Creates a PS/2 break code for key release events
 * @param  scancode: Pointer to PS/2 scan code structure
 * @param  key_code: PS/2 key code
 * @retval PS2_PROTOCOL_OK if successful, PS2_PROTOCOL_ERROR otherwise
 */
PS2_ProtocolStatus_t ps2_create_break_code(PS2_ScanCode_t *scancode, uint8_t key_code)
{
    if (scancode == NULL) {
        return PS2_PROTOCOL_ERROR;
    }
    
    /* Break code is 0xF0 followed by the key code */
    scancode->data[0] = PS2_BREAK_CODE_PREFIX;
    scancode->data[1] = key_code;
    scancode->length = 2;
    
    return PS2_PROTOCOL_OK;
}

/**
 * @brief  Create extended make code scan code
 * @note   Creates a PS/2 extended make code for extended keys
 * @param  scancode: Pointer to PS/2 scan code structure
 * @param  key_code: PS/2 extended key code
 * @retval PS2_PROTOCOL_OK if successful, PS2_PROTOCOL_ERROR otherwise
 */
PS2_ProtocolStatus_t ps2_create_extended_make_code(PS2_ScanCode_t *scancode, uint8_t key_code)
{
    if (scancode == NULL) {
        return PS2_PROTOCOL_ERROR;
    }
    
    /* Extended make code is 0xE0 followed by the key code */
    scancode->data[0] = PS2_EXTENDED_CODE_PREFIX;
    scancode->data[1] = key_code;
    scancode->length = 2;
    
    return PS2_PROTOCOL_OK;
}

/**
 * @brief  Create extended break code scan code
 * @note   Creates a PS/2 extended break code for extended keys
 * @param  scancode: Pointer to PS/2 scan code structure
 * @param  key_code: PS/2 extended key code
 * @retval PS2_PROTOCOL_OK if successful, PS2_PROTOCOL_ERROR otherwise  
 */
PS2_ProtocolStatus_t ps2_create_extended_break_code(PS2_ScanCode_t *scancode, uint8_t key_code)
{
    if (scancode == NULL) {
        return PS2_PROTOCOL_ERROR;
    }
    
    /* Extended break code is 0xE0, 0xF0, followed by the key code */
    scancode->data[0] = PS2_EXTENDED_CODE_PREFIX;
    scancode->data[1] = PS2_BREAK_CODE_PREFIX;
    scancode->data[2] = key_code;
    scancode->length = 3;
    
    return PS2_PROTOCOL_OK;
}

/**
 * @brief  Validate PS/2 scan code
 * @note   Checks if PS/2 scan code structure is valid
 * @param  scancode: Pointer to PS/2 scan code structure
 * @retval PS2_PROTOCOL_OK if valid, PS2_PROTOCOL_ERROR otherwise
 */
PS2_ProtocolStatus_t ps2_validate_scancode(const PS2_ScanCode_t *scancode)
{
    if (scancode == NULL || 
        scancode->length == 0 || 
        scancode->length > PS2_MAX_SCANCODE_LENGTH) {
        return PS2_PROTOCOL_ERROR;
    }
    
    return PS2_PROTOCOL_OK;
}

/**
 * @brief  Get PS/2 scan code for common keys
 * @note   Returns the PS/2 scan code for frequently used keys
 * @param  key: Common key identifier
 * @retval PS/2 scan code, 0x00 if not found
 */
uint8_t ps2_get_common_key_scancode(PS2_CommonKey_t key)
{
    switch (key) {
        case PS2_KEY_A:         return 0x1C;
        case PS2_KEY_B:         return 0x32;
        case PS2_KEY_C:         return 0x21;
        case PS2_KEY_D:         return 0x23;
        case PS2_KEY_E:         return 0x24;
        case PS2_KEY_F:         return 0x2B;
        case PS2_KEY_G:         return 0x34;
        case PS2_KEY_H:         return 0x33;
        case PS2_KEY_I:         return 0x43;
        case PS2_KEY_J:         return 0x3B;
        case PS2_KEY_K:         return 0x42;
        case PS2_KEY_L:         return 0x4B;
        case PS2_KEY_M:         return 0x3A;
        case PS2_KEY_N:         return 0x31;
        case PS2_KEY_O:         return 0x44;
        case PS2_KEY_P:         return 0x4D;
        case PS2_KEY_Q:         return 0x15;
        case PS2_KEY_R:         return 0x2D;
        case PS2_KEY_S:         return 0x1B;
        case PS2_KEY_T:         return 0x2C;
        case PS2_KEY_U:         return 0x3C;
        case PS2_KEY_V:         return 0x2A;
        case PS2_KEY_W:         return 0x1D;
        case PS2_KEY_X:         return 0x22;
        case PS2_KEY_Y:         return 0x35;
        case PS2_KEY_Z:         return 0x1A;
        
        case PS2_KEY_0:         return 0x45;
        case PS2_KEY_1:         return 0x16;
        case PS2_KEY_2:         return 0x1E;
        case PS2_KEY_3:         return 0x26;
        case PS2_KEY_4:         return 0x25;
        case PS2_KEY_5:         return 0x2E;
        case PS2_KEY_6:         return 0x36;
        case PS2_KEY_7:         return 0x3D;
        case PS2_KEY_8:         return 0x3E;
        case PS2_KEY_9:         return 0x46;
        
        case PS2_KEY_SPACE:     return 0x29;
        case PS2_KEY_ENTER:     return 0x5A;
        case PS2_KEY_BACKSPACE: return 0x66;
        case PS2_KEY_TAB:       return 0x0D;
        case PS2_KEY_ESC:       return 0x76;
        
        case PS2_KEY_LSHIFT:    return 0x12;
        case PS2_KEY_RSHIFT:    return 0x59;
        case PS2_KEY_LCTRL:     return 0x14;
        case PS2_KEY_LALT:      return 0x11;
        
        case PS2_KEY_F1:        return 0x05;
        case PS2_KEY_F2:        return 0x06;
        case PS2_KEY_F3:        return 0x04;
        case PS2_KEY_F4:        return 0x0C;
        case PS2_KEY_F5:        return 0x03;
        case PS2_KEY_F6:        return 0x0B;
        case PS2_KEY_F7:        return 0x83;
        case PS2_KEY_F8:        return 0x0A;
        case PS2_KEY_F9:        return 0x01;
        case PS2_KEY_F10:       return 0x09;
        case PS2_KEY_F11:       return 0x78;
        case PS2_KEY_F12:       return 0x07;
        
        default:                return 0x00;
    }
}

/**
 * @brief  Check if key is an extended key
 * @note   Determines if a key requires extended scan code prefix
 * @param  key: Common key identifier
 * @retval 1 if extended key, 0 otherwise
 */
uint8_t ps2_is_extended_key(PS2_CommonKey_t key)
{
    switch (key) {
        case PS2_KEY_RCTRL:
        case PS2_KEY_RALT:
        case PS2_KEY_INSERT:
        case PS2_KEY_DELETE:
        case PS2_KEY_HOME:
        case PS2_KEY_END:
        case PS2_KEY_PAGE_UP:
        case PS2_KEY_PAGE_DOWN:
        case PS2_KEY_UP_ARROW:
        case PS2_KEY_DOWN_ARROW:
        case PS2_KEY_LEFT_ARROW:
        case PS2_KEY_RIGHT_ARROW:
            return 1;
        default:
            return 0;
    }
}

/**
 * @brief  Copy PS/2 scan code
 * @note   Copies one PS/2 scan code structure to another
 * @param  dest: Destination scan code structure
 * @param  src: Source scan code structure
 * @retval PS2_PROTOCOL_OK if successful, PS2_PROTOCOL_ERROR otherwise
 */
PS2_ProtocolStatus_t ps2_copy_scancode(PS2_ScanCode_t *dest, const PS2_ScanCode_t *src)
{
    if (dest == NULL || src == NULL) {
        return PS2_PROTOCOL_ERROR;
    }
    
    if (ps2_validate_scancode(src) != PS2_PROTOCOL_OK) {
        return PS2_PROTOCOL_ERROR;
    }
    
    dest->length = src->length;
    for (uint8_t i = 0; i < src->length; i++) {
        dest->data[i] = src->data[i];
    }
    
    return PS2_PROTOCOL_OK;
}