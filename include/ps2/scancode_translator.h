/**
 ******************************************************************************
 * @file    scancode_translator.h
 * @brief   Header for scancode_translator.c - USB to PS/2 translator
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __SCANCODE_TRANSLATOR_H
#define __SCANCODE_TRANSLATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "ps2_protocol.h"
#include "keyboard_handler.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Translator status enumeration
 */
typedef enum {
    TRANSLATOR_OK = 0,          ///< Translation successful
    TRANSLATOR_ERROR,           ///< Translation failed
    TRANSLATOR_INIT,            ///< Translator initializing
    TRANSLATOR_READY            ///< Translator ready for operation
} TranslatorStatus_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
TranslatorStatus_t scancode_translator_init(void);
TranslatorStatus_t scancode_translator_usb_to_ps2(const USB_HID_KeyboardData_t *usb_data, 
                                                  PS2_ScanCode_t *ps2_scancode);
TranslatorStatus_t scancode_translator_get_status(void);
void scancode_translator_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __SCANCODE_TRANSLATOR_H */