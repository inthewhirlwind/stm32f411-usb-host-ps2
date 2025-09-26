/**
 ******************************************************************************
 * @file    keyboard_handler.h
 * @brief   Header for keyboard_handler.c - USB HID keyboard handler
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __KEYBOARD_HANDLER_H
#define __KEYBOARD_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Keyboard handler status enumeration
 */
typedef enum {
    KEYBOARD_HANDLER_OK = 0,        ///< Handler operation successful
    KEYBOARD_HANDLER_ERROR,         ///< Handler operation failed
    KEYBOARD_HANDLER_INIT,          ///< Handler initializing
    KEYBOARD_HANDLER_READY,         ///< Handler ready for operation
    KEYBOARD_HANDLER_BUFFER_FULL    ///< Handler buffer is full
} KeyboardHandlerStatus_t;

/**
 * @brief Keyboard data availability status
 */
typedef enum {
    KEYBOARD_NO_DATA = 0,           ///< No keyboard data available
    KEYBOARD_DATA_AVAILABLE         ///< Keyboard data is available
} KeyboardDataStatus_t;

/**
 * @brief USB HID keyboard data structure
 */
#define USB_HID_MAX_KEYS 6
typedef struct {
    uint8_t modifier;                           ///< Modifier keys bitmask
    uint8_t reserved;                           ///< Reserved byte (usually 0)
    uint8_t keys[USB_HID_MAX_KEYS];            ///< Array of pressed key codes
    uint8_t key_count;                         ///< Number of pressed keys
} USB_HID_KeyboardData_t;

/* Exported constants --------------------------------------------------------*/
/* USB HID modifier key bitmasks */
#define USB_HID_MODIFIER_LEFT_CTRL      0x01
#define USB_HID_MODIFIER_LEFT_SHIFT     0x02
#define USB_HID_MODIFIER_LEFT_ALT       0x04
#define USB_HID_MODIFIER_LEFT_GUI       0x08
#define USB_HID_MODIFIER_RIGHT_CTRL     0x10
#define USB_HID_MODIFIER_RIGHT_SHIFT    0x20
#define USB_HID_MODIFIER_RIGHT_ALT      0x40
#define USB_HID_MODIFIER_RIGHT_GUI      0x80

/* Common USB HID key codes */
#define USB_HID_KEY_A                   0x04
#define USB_HID_KEY_B                   0x05
#define USB_HID_KEY_C                   0x06
#define USB_HID_KEY_D                   0x07
#define USB_HID_KEY_E                   0x08
#define USB_HID_KEY_F                   0x09
#define USB_HID_KEY_G                   0x0A
#define USB_HID_KEY_H                   0x0B
#define USB_HID_KEY_I                   0x0C
#define USB_HID_KEY_J                   0x0D
#define USB_HID_KEY_K                   0x0E
#define USB_HID_KEY_L                   0x0F
#define USB_HID_KEY_M                   0x10
#define USB_HID_KEY_N                   0x11
#define USB_HID_KEY_O                   0x12
#define USB_HID_KEY_P                   0x13
#define USB_HID_KEY_Q                   0x14
#define USB_HID_KEY_R                   0x15
#define USB_HID_KEY_S                   0x16
#define USB_HID_KEY_T                   0x17
#define USB_HID_KEY_U                   0x18
#define USB_HID_KEY_V                   0x19
#define USB_HID_KEY_W                   0x1A
#define USB_HID_KEY_X                   0x1B
#define USB_HID_KEY_Y                   0x1C
#define USB_HID_KEY_Z                   0x1D

#define USB_HID_KEY_1                   0x1E
#define USB_HID_KEY_2                   0x1F
#define USB_HID_KEY_3                   0x20
#define USB_HID_KEY_4                   0x21
#define USB_HID_KEY_5                   0x22
#define USB_HID_KEY_6                   0x23
#define USB_HID_KEY_7                   0x24
#define USB_HID_KEY_8                   0x25
#define USB_HID_KEY_9                   0x26
#define USB_HID_KEY_0                   0x27

#define USB_HID_KEY_ENTER               0x28
#define USB_HID_KEY_ESCAPE              0x29
#define USB_HID_KEY_BACKSPACE           0x2A
#define USB_HID_KEY_TAB                 0x2B
#define USB_HID_KEY_SPACE               0x2C

#define USB_HID_KEY_F1                  0x3A
#define USB_HID_KEY_F2                  0x3B
#define USB_HID_KEY_F3                  0x3C
#define USB_HID_KEY_F4                  0x3D
#define USB_HID_KEY_F5                  0x3E
#define USB_HID_KEY_F6                  0x3F
#define USB_HID_KEY_F7                  0x40
#define USB_HID_KEY_F8                  0x41
#define USB_HID_KEY_F9                  0x42
#define USB_HID_KEY_F10                 0x43
#define USB_HID_KEY_F11                 0x44
#define USB_HID_KEY_F12                 0x45

#define USB_HID_KEY_INSERT              0x49
#define USB_HID_KEY_HOME                0x4A
#define USB_HID_KEY_PAGE_UP             0x4B
#define USB_HID_KEY_DELETE              0x4C
#define USB_HID_KEY_END                 0x4D
#define USB_HID_KEY_PAGE_DOWN           0x4E
#define USB_HID_KEY_RIGHT_ARROW         0x4F
#define USB_HID_KEY_LEFT_ARROW          0x50
#define USB_HID_KEY_DOWN_ARROW          0x51
#define USB_HID_KEY_UP_ARROW            0x52

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
KeyboardHandlerStatus_t keyboard_handler_init(void);
KeyboardHandlerStatus_t keyboard_handler_process_report(const uint8_t *report, uint16_t report_size);
KeyboardDataStatus_t keyboard_handler_get_data(USB_HID_KeyboardData_t *keyboard_data);
KeyboardHandlerStatus_t keyboard_handler_get_status(void);
void keyboard_handler_tick(void);
void keyboard_handler_clear_buffer(void);

/* Utility functions */
uint8_t keyboard_is_key_pressed(const USB_HID_KeyboardData_t *keyboard_data, uint8_t key_code);
uint8_t keyboard_is_modifier_pressed(const USB_HID_KeyboardData_t *keyboard_data, uint8_t modifier_mask);

#ifdef __cplusplus
}
#endif

#endif /* __KEYBOARD_HANDLER_H */