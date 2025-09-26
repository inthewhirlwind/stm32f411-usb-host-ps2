/**
 ******************************************************************************
 * @file    usb_host_init.c
 * @brief   USB Host initialization and management for STM32F411
 * @author  STM32F411 USB-PS2 Project
 * @version 1.0.0
 * @date    2024
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usb_host_init.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define USB_HOST_MAX_RETRY_COUNT    3
#define USB_HOST_RETRY_DELAY_MS     100

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static HCD_HandleTypeDef hhcd_USB_OTG_FS;
static USB_HostStatus_t usb_host_status = USB_HOST_INIT;
static uint8_t device_connected = 0;
static uint32_t retry_count = 0;

/* Private function prototypes -----------------------------------------------*/
static void MX_USB_OTG_FS_HCD_Init(void);
static void USB_Host_Error_Handler(void);

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Initialize USB Host subsystem
 * @note   Configures USB OTG FS in Host mode for HID keyboard detection
 * @retval USB_HOST_OK if successful, USB_HOST_ERROR otherwise
 */
USB_HostStatus_t usb_host_init(void)
{
    usb_host_status = USB_HOST_INIT;
    
    /* Initialize USB OTG FS peripheral in Host mode */
    MX_USB_OTG_FS_HCD_Init();
    
    /* Start USB Host operation */
    if (HAL_HCD_Start(&hhcd_USB_OTG_FS) != HAL_OK) {
        usb_host_status = USB_HOST_ERROR;
        return USB_HOST_ERROR;
    }
    
    usb_host_status = USB_HOST_READY;
    return USB_HOST_OK;
}

/**
 * @brief  USB OTG FS HCD Initialization Function
 * @note   Configures USB OTG FS peripheral for Host mode operation
 * @retval None
 */
static void MX_USB_OTG_FS_HCD_Init(void)
{
    /* Enable USB OTG FS clock */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
    
    /* Configure USB OTG FS Host */
    hhcd_USB_OTG_FS.Instance = USB_OTG_FS;
    hhcd_USB_OTG_FS.Init.Host_channels = 8;
    hhcd_USB_OTG_FS.Init.speed = HCD_SPEED_FULL;
    hhcd_USB_OTG_FS.Init.dma_enable = DISABLE;
    hhcd_USB_OTG_FS.Init.phy_itface = HCD_PHY_EMBEDDED;
    hhcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
    hhcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
    hhcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
    hhcd_USB_OTG_FS.Init.use_external_vbus = DISABLE;
    
    if (HAL_HCD_Init(&hhcd_USB_OTG_FS) != HAL_OK) {
        USB_Host_Error_Handler();
    }
    
    /* Configure USB OTG FS interrupt */
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
}

/**
 * @brief  Process USB Host events
 * @note   Should be called regularly from main loop to handle USB events
 * @retval None
 */
void usb_host_process(void)
{
    static uint32_t last_check_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    /* Check for device connection/disconnection periodically */
    if ((current_time - last_check_time) > 100) { /* Check every 100ms */
        
        /* Check if device is connected */
        if (HAL_HCD_GetState(&hhcd_USB_OTG_FS) == HAL_HCD_STATE_READY) {
            if (!device_connected) {
                device_connected = 1;
                usb_host_status = USB_HOST_DEVICE_CONNECTED;
                
                /* Start device enumeration process */
                // This would typically involve:
                // 1. Getting device descriptor
                // 2. Setting device address
                // 3. Getting configuration descriptor
                // 4. Setting configuration
                // 5. Getting HID report descriptor (for keyboard)
            }
        } else {
            if (device_connected) {
                device_connected = 0;
                usb_host_status = USB_HOST_READY;
            }
        }
        
        last_check_time = current_time;
    }
}

/**
 * @brief  Get USB Host status
 * @retval Current USB Host status
 */
USB_HostStatus_t usb_host_get_status(void)
{
    return usb_host_status;
}

/**
 * @brief  Check if USB device is connected
 * @retval 1 if device connected, 0 otherwise
 */
uint8_t usb_host_device_connected(void)
{
    return device_connected;
}

/**
 * @brief  USB Host URB change callback
 * @note   Called when USB URB (USB Request Block) state changes
 * @param  hhcd: HCD handle
 * @param  chnum: Channel number
 * @param  urb_state: URB state
 * @retval None
 */
void usb_host_urb_change_callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state)
{
    /* Handle URB state changes */
    switch (urb_state) {
        case URB_DONE:
            /* Transfer completed successfully */
            break;
            
        case URB_ERROR:
            /* Transfer error occurred */
            if (retry_count < USB_HOST_MAX_RETRY_COUNT) {
                retry_count++;
                /* Retry the transfer */
            } else {
                usb_host_status = USB_HOST_ERROR;
                retry_count = 0;
            }
            break;
            
        case URB_STALL:
            /* Endpoint stalled */
            usb_host_status = USB_HOST_ERROR;
            break;
            
        default:
            break;
    }
}

/**
 * @brief  Read data from USB HID keyboard
 * @note   Reads HID report from connected keyboard device
 * @param  data: Pointer to buffer to store keyboard data
 * @param  length: Length of data to read
 * @retval USB_HOST_OK if successful, USB_HOST_ERROR otherwise
 */
USB_HostStatus_t usb_host_read_keyboard_data(uint8_t *data, uint16_t length)
{
    if (!device_connected || data == NULL || length == 0) {
        return USB_HOST_ERROR;
    }
    
    /* This would typically involve:
     * 1. Submitting an interrupt IN transfer request
     * 2. Waiting for transfer completion
     * 3. Processing the received HID report
     */
    
    /* Placeholder implementation - actual implementation would use HAL_HCD functions */
    (void)data;
    (void)length;
    
    return USB_HOST_OK;
}

/**
 * @brief  USB Host error handler
 * @note   Called when USB Host error occurs
 * @retval None
 */
static void USB_Host_Error_Handler(void)
{
    usb_host_status = USB_HOST_ERROR;
    
    /* Additional error handling can be added here */
    /* For example: logging, LED indication, etc. */
}

/* HAL Callback functions ----------------------------------------------------*/

/**
 * @brief  SOF callback function
 * @note   Called on Start of Frame event
 * @param  hhcd: HCD handle
 * @retval None
 */
void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd)
{
    /* SOF callback - can be used for timing purposes */
    (void)hhcd;
}

/**
 * @brief  Connect callback function
 * @note   Called when device is connected
 * @param  hhcd: HCD handle
 * @retval None
 */
void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd)
{
    device_connected = 1;
    usb_host_status = USB_HOST_DEVICE_CONNECTED;
    retry_count = 0;
}

/**
 * @brief  Disconnect callback function
 * @note   Called when device is disconnected
 * @param  hhcd: HCD handle
 * @retval None
 */
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd)
{
    device_connected = 0;
    usb_host_status = USB_HOST_READY;
    retry_count = 0;
}

/**
 * @brief  This function handles USB OTG FS global interrupt
 * @retval None
 */
void OTG_FS_IRQHandler(void)
{
    HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
}