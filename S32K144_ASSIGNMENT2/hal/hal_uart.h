/**
 * @file hal_uart.h
 * @author benecosta2711
 * @brief A library contain function that configure the LPUART peripheral register for initializing the
 * peripheral, using the peripheral and processing data of that peripheral at hardware level.
 * Current version of this library support:
 * - Basic config function for LPUART0, LPUART1 and LPUART2.
 * - Basic function to processing data, including: send and receive blocking.
 * - Support configure interrupt, including: overun detect, full receiver data register detect, full and empty transmitter data register detect.
 * @version 0.1
 * @date 2025-10-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef HAL_UART_H_
#define HAL_UART_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "S32K144.h"
#include "stddef.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief Defines the virtual UART instances available.
 * Used as an index for the mapping and state tables.
 */
#define HAL_LPUART0             0U
#define HAL_LPUART1             1U
#define HAL_LPUART2             2U

/* Dummy define for testing the hal layer */
#define ARM_USART_EVENT_SEND_COMPLETE       (1UL << 0)  ///< Send completed; however USART may still transmit data
#define ARM_USART_EVENT_RECEIVE_COMPLETE    (1UL << 1)  ///< Receive completed
#define ARM_USART_EVENT_TRANSFER_COMPLETE   (1UL << 2)  ///< Transfer completed
#define ARM_USART_EVENT_TX_COMPLETE         (1UL << 3)  ///< Transmit completed (optional)
#define ARM_USART_EVENT_TX_UNDERFLOW        (1UL << 4)  ///< Transmit data not available (Synchronous Slave)
#define ARM_USART_EVENT_RX_OVERFLOW         (1UL << 5)  ///< Receive data overflow
#define ARM_USART_EVENT_RX_TIMEOUT          (1UL << 6)  ///< Receive character timeout (optional)
#define ARM_USART_EVENT_RX_BREAK            (1UL << 7)  ///< Break detected on receive
#define ARM_USART_EVENT_RX_FRAMING_ERROR    (1UL << 8)  ///< Framing error detected on receive
#define ARM_USART_EVENT_RX_PARITY_ERROR     (1UL << 9)  ///< Parity error detected on receive
#define ARM_USART_EVENT_CTS                 (1UL << 10) ///< CTS state changed (optional)
#define ARM_USART_EVENT_DSR                 (1UL << 11) ///< DSR state changed (optional)
#define ARM_USART_EVENT_DCD                 (1UL << 12) ///< DCD state changed (optional)
#define ARM_USART_EVENT_RI                  (1UL << 13) ///< RI  state changed (optional)


/**
 * @brief Defines HAL-specific data bit configurations.
 */
typedef enum
{
    HAL_UART_DATA_BITS_8,
    HAL_UART_DATA_BITS_9
} hal_uart_databits_t;

/**
 * @brief Defines HAL-specific parity configurations.
 */
typedef enum
{
    HAL_UART_PARITY_NONE,
    HAL_UART_PARITY_EVEN,
    HAL_UART_PARITY_ODD
} hal_uart_parity_t;

/**
 * @brief Defines HAL-specific stop bit configurations.
 */
typedef enum
{
    HAL_UART_STOP_BITS_1,
    HAL_UART_STOP_BITS_2
} hal_uart_stopbits_t;

/**
 * @brief Defines HAL-specific interrupt sources.
 * These are bitmasks to allow enabling multiple interrupts at once.
 */
typedef enum
{
    HAL_UART_INT_TX_DATA_REG_EMPTY = (1U << 0U), /* Corresponds to STAT[TDRE] */
    HAL_UART_INT_TX_COMPLETE       = (1U << 1U), /* Corresponds to STAT[TC]   */
    HAL_UART_INT_RX_DATA_REG_FULL  = (1U << 2U), /* Corresponds to STAT[RDRF] */
    HAL_UART_INT_RX_OVERRUN        = (1U << 3U)  /* Corresponds to STAT[OR]   */
} hal_uart_interrupt_t;

/**
 * @brief Structure for UART configuration parameters.
 */
typedef struct
{
    uint32_t baudRate;
    hal_uart_databits_t dataBits;
    hal_uart_parity_t parity;
    hal_uart_stopbits_t stopBits;
} hal_uart_config_t;

/**
 * @brief Defines the callback function pointer type for UART events.
 * The 'event' parameter will be a bitmask of CMSIS USART events.
 */
typedef void (*HAL_UART_Callback_t)(uint32_t event);

extern volatile uint8_t* g_txBuffer;
extern volatile uint32_t g_txBufferLength;
extern volatile uint32_t g_txBufferCount;

extern volatile uint8_t* g_rxBuffer;
extern volatile uint32_t g_rxBufferCount;
extern volatile uint32_t g_rxBufferLength;

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Initializes a LPUART instance.
 * Enables clocks for LPUART and PORT modules, and configures pins.
 *
 * @param instance The virtual UART instance (e.g., HAL_LPUART0).
 * @return true if initialization is successful, false otherwise.
 */
uint8_t HAL_UART_Init(uint32_t instance);

/**
 * @brief Configures a LPUART instance with specified parameters.
 *
 * @param instance The virtual UART instance.
 * @param config Pointer to the configuration structure.
 * @return true if configuration is successful, false otherwise.
 */
uint8_t HAL_UART_Configure(uint32_t instance, const hal_uart_config_t *config);

/**
 * @brief De-initializes a LPUART instance.
 * Disables clocks and resets peripheral.
 *
 * @param instance The virtual UART instance.
 */
void HAL_UART_Deinit(uint32_t instance);

/**
 * @brief Registers a callback function for a UART instance.
 *
 * @param instance The virtual UART instance.
 * @param callback The callback function to be registered.
 */
void HAL_UART_RegisterCallback(uint32_t instance, HAL_UART_Callback_t callback);

/**
 * @brief Sends a single byte of data.
 * This is a blocking function that waits until the transmit buffer is empty.
 *
 * @param instance The virtual UART instance.
 * @param data The byte of data to send.
 */
void HAL_UART_SendByteBlocking(uint32_t instance, uint8_t data);

/**
 * @brief Reads a single byte of data.
 * This is a blocking function that waits until a byte is received.
 *
 * @param instance The virtual UART instance.
 * @return The byte of data received.
 */
uint8_t HAL_UART_ReadByteBlocking(uint32_t instance);

/**
 * @brief Enables one or more UART interrupts.
 *
 * @param instance The virtual UART instance.
 * @param interruptMask A bitmask of interrupts to enable (from hal_uart_interrupt_t).
 */
void HAL_UART_EnableInterrupts(uint32_t instance, uint32_t interruptMask);

/**
 * @brief Disables one or more UART interrupts.
 *
 * @param instance The virtual UART instance.
 * @param interruptMask A bitmask of interrupts to disable (from hal_uart_interrupt_t).
 */
void HAL_UART_DisableInterrupts(uint32_t instance, uint32_t interruptMask);

/**
 * @brief Enables or disables the transmitter.
 *
 * @param instance The virtual UART instance.
 * @param enable Set to true to enable, false to disable.
 */
void HAL_UART_EnableTransmitter(uint32_t instance, uint8_t enable);

/**
 * @brief Enables or disables the receiver.
 *
 * @param instance The virtual UART instance.
 * @param enable Set to true to enable, false to disable.
 */
void HAL_UART_EnableReceiver(uint32_t instance, uint8_t enable);

/**
 * @brief Gets the current status flags from the LPUART peripheral.
 *
 * @param instance The virtual UART instance.
 * @return The 32-bit value of the STAT register.
 */
uint32_t HAL_UART_GetStatusFlags(uint32_t instance);

#endif /* HAL_UART_H_ */
