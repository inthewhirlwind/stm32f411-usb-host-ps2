/**
 ******************************************************************************
 * @file    usb_host_init.h
 * @brief   Header for usb_host_init.c - USB Host initialization
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __USB_HOST_INIT_H
#define __USB_HOST_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief USB Host status enumeration
 */
typedef enum {
    USB_HOST_INIT = 0,              ///< USB Host initialization
    USB_HOST_READY,                 ///< USB Host ready for operation
    USB_HOST_DEVICE_CONNECTED,      ///< USB device connected
    USB_HOST_DEVICE_ENUMERATED,     ///< USB device enumerated
    USB_HOST_ERROR                  ///< USB Host error state
} USB_HostStatus_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
USB_HostStatus_t usb_host_init(void);
void usb_host_process(void);
USB_HostStatus_t usb_host_get_status(void);
uint8_t usb_host_device_connected(void);
USB_HostStatus_t usb_host_read_keyboard_data(uint8_t *data, uint16_t length);

/* HAL callback functions */
void usb_host_urb_change_callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state);
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd);

/* Interrupt handlers */
void OTG_FS_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST_INIT_H */