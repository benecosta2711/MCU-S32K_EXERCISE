/**
 * @file app_uart.h
 * @author benecosta2711
 * @brief A library provide services about send, receive and manage data through LPUART, including:
 * - Init related peripheral through CMSIS Driver.
 * - Create API for send and receive in both blocking and non-blocking style.
 * - Manage data through private buffer.
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APP_UART_H_
#define APP_UART_H_

#include "S32K144.h"
#include "string.h"
#include "Driver_USART.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Define receive buffer size */
#define BUFFER_SIZE 64

/* Define uart application error code */
#define APP_UART_ERROR                     0
#define APP_UART_OK                        1
#define APP_UART_TIMEOUT                   2
#define APP_UART_SEND_FAIL                 3
#define APP_UART_RECEIVE_FAIL              4
#define APP_UART_RECEIVE_DATA              5
#define APP_UART_RECEIVE_IDLE              6
#define APP_UART_TRANSMIT_COMPLETE         7
#define APP_UART_TRANSMIT_INPROGRESS       8
#define APP_UART_TRANSMIT_IDLE             9


/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
uint8_t app_uart_init(void);
uint8_t app_uart_send_char(char* data, uint32_t timeout);
uint8_t app_uart_receive_char(char* data, uint32_t length, uint32_t timeout);
uint8_t app_uart_get_buffer_size(void);
/* reset buffer counter, return size */
void app_uart_get_buffer_data(uint8_t* receiveData);
void app_uart_receive_non_blocking(void);
uint8_t app_uart_get_incoming_data(uint8_t* data);


#endif /* APP_UART_H_ */
