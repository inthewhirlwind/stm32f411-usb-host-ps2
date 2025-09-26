/**
 ******************************************************************************
 * @file    usb_host_hid.h
 * @brief   Header for usb_host_hid.c - USB Host HID class
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __USB_HOST_HID_H
#define __USB_HOST_HID_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/**
 * @brief USB Host HID status enumeration
 */
typedef enum {
    USB_HOST_HID_OK = 0,        ///< HID operation successful
    USB_HOST_HID_ERROR          ///< HID operation failed
} USB_HostHIDStatus_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
USB_HostHIDStatus_t usb_host_hid_init(void);
USB_HostHIDStatus_t usb_host_hid_process(void);
USB_HostHIDStatus_t usb_host_hid_get_keyboard_report(uint8_t *report, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_HID_H */