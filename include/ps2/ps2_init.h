/**
 ******************************************************************************
 * @file    ps2_init.h
 * @brief   Header for ps2_init.c - PS/2 interface initialization
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

#ifndef __PS2_INIT_H
#define __PS2_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "ps2_protocol.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief PS/2 interface status enumeration
 */
typedef enum {
    PS2_OK = 0,             ///< PS/2 operation successful
    PS2_ERROR,              ///< PS/2 operation failed
    PS2_INIT,               ///< PS/2 initialization in progress
    PS2_READY,              ///< PS/2 ready for operation
    PS2_TRANSMITTING        ///< PS/2 transmission in progress
} PS2_Status_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
PS2_Status_t ps2_init(void);
PS2_Status_t ps2_send_scancode(const PS2_ScanCode_t *scancode);
PS2_Status_t ps2_send_byte(uint8_t data);
void ps2_send_bit(uint8_t bit_value);
void ps2_delay_us(uint32_t microseconds);
PS2_Status_t ps2_get_status(void);
void ps2_tick(void);
void ps2_timer_callback(void);
void ps2_read_lines(uint8_t *clock_state, uint8_t *data_state);
void ps2_set_lines(uint8_t clock_state, uint8_t data_state);

#ifdef __cplusplus
}
#endif

#endif /* __PS2_INIT_H */