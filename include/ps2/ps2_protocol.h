/**
 ******************************************************************************
 * @file    ps2_protocol.h
 * @brief   Header for ps2_protocol.c - PS/2 protocol definitions
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __PS2_PROTOCOL_H
#define __PS2_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
 * @brief PS/2 protocol status enumeration
 */
typedef enum {
    PS2_PROTOCOL_OK = 0,        ///< Protocol operation successful
    PS2_PROTOCOL_ERROR          ///< Protocol operation failed
} PS2_ProtocolStatus_t;

/**
 * @brief PS/2 scan code structure
 */
#define PS2_MAX_SCANCODE_LENGTH 4
typedef struct {
    uint8_t data[PS2_MAX_SCANCODE_LENGTH];  ///< Scan code data bytes
    uint8_t length;                         ///< Number of bytes in scan code
} PS2_ScanCode_t;

/**
 * @brief Common PS/2 key identifiers
 */
typedef enum {
    /* Letters */
    PS2_KEY_A = 0x00, PS2_KEY_B, PS2_KEY_C, PS2_KEY_D, PS2_KEY_E,
    PS2_KEY_F, PS2_KEY_G, PS2_KEY_H, PS2_KEY_I, PS2_KEY_J,
    PS2_KEY_K, PS2_KEY_L, PS2_KEY_M, PS2_KEY_N, PS2_KEY_O,
    PS2_KEY_P, PS2_KEY_Q, PS2_KEY_R, PS2_KEY_S, PS2_KEY_T,
    PS2_KEY_U, PS2_KEY_V, PS2_KEY_W, PS2_KEY_X, PS2_KEY_Y, PS2_KEY_Z,
    
    /* Numbers */
    PS2_KEY_0, PS2_KEY_1, PS2_KEY_2, PS2_KEY_3, PS2_KEY_4,
    PS2_KEY_5, PS2_KEY_6, PS2_KEY_7, PS2_KEY_8, PS2_KEY_9,
    
    /* Common keys */
    PS2_KEY_SPACE, PS2_KEY_ENTER, PS2_KEY_BACKSPACE, PS2_KEY_TAB, PS2_KEY_ESC,
    
    /* Modifier keys */
    PS2_KEY_LSHIFT, PS2_KEY_RSHIFT, PS2_KEY_LCTRL, PS2_KEY_RCTRL,
    PS2_KEY_LALT, PS2_KEY_RALT,
    
    /* Function keys */
    PS2_KEY_F1, PS2_KEY_F2, PS2_KEY_F3, PS2_KEY_F4, PS2_KEY_F5, PS2_KEY_F6,
    PS2_KEY_F7, PS2_KEY_F8, PS2_KEY_F9, PS2_KEY_F10, PS2_KEY_F11, PS2_KEY_F12,
    
    /* Extended keys */
    PS2_KEY_INSERT, PS2_KEY_DELETE, PS2_KEY_HOME, PS2_KEY_END,
    PS2_KEY_PAGE_UP, PS2_KEY_PAGE_DOWN,
    PS2_KEY_UP_ARROW, PS2_KEY_DOWN_ARROW, PS2_KEY_LEFT_ARROW, PS2_KEY_RIGHT_ARROW
} PS2_CommonKey_t;

/* Exported constants --------------------------------------------------------*/
#define PS2_BREAK_CODE_PREFIX       0xF0   ///< Prefix for break codes (key release)
#define PS2_EXTENDED_CODE_PREFIX    0xE0   ///< Prefix for extended keys

/* Special PS/2 scan codes */
#define PS2_SCANCODE_BAT_SUCCESS    0xAA   ///< Basic Assurance Test success
#define PS2_SCANCODE_ID_KEYBOARD    0xAB   ///< Keyboard ID code
#define PS2_SCANCODE_ECHO           0xEE   ///< Echo response
#define PS2_SCANCODE_ACK            0xFA   ///< Acknowledge
#define PS2_SCANCODE_RESEND         0xFE   ///< Resend request
#define PS2_SCANCODE_ERROR          0xFF   ///< Error code

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
PS2_ProtocolStatus_t ps2_create_scancode(PS2_ScanCode_t *scancode, 
                                         const uint8_t *data, 
                                         uint8_t length);
PS2_ProtocolStatus_t ps2_create_make_code(PS2_ScanCode_t *scancode, uint8_t key_code);
PS2_ProtocolStatus_t ps2_create_break_code(PS2_ScanCode_t *scancode, uint8_t key_code);
PS2_ProtocolStatus_t ps2_create_extended_make_code(PS2_ScanCode_t *scancode, uint8_t key_code);
PS2_ProtocolStatus_t ps2_create_extended_break_code(PS2_ScanCode_t *scancode, uint8_t key_code);
PS2_ProtocolStatus_t ps2_validate_scancode(const PS2_ScanCode_t *scancode);
uint8_t ps2_get_common_key_scancode(PS2_CommonKey_t key);
uint8_t ps2_is_extended_key(PS2_CommonKey_t key);
PS2_ProtocolStatus_t ps2_copy_scancode(PS2_ScanCode_t *dest, const PS2_ScanCode_t *src);

#ifdef __cplusplus
}
#endif

#endif /* __PS2_PROTOCOL_H */