/**
 * @file app_uart.c
 * @author benecosta2711
 * @brief 
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "app_uart.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define USART_BAUDRATE 9600

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Define variable manage receiving data from uart */
static volatile uint8_t receivedData = 0;
static volatile uint8_t receiveBuffer[BUFFER_SIZE];
static volatile uint8_t receiveBufferCounter = 0;

/* Define status flag for transmitter and receiver */
static volatile uint8_t sendDataCompleteFlag = APP_UART_TRANSMIT_IDLE;
static volatile uint8_t receiveDataCompleteFlag = APP_UART_RECEIVE_IDLE;

/* CMSIS Driver manager struct */
extern ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART* uart0_drv = &Driver_USART0;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
static void app_uart_callback(uint32_t event);

/*******************************************************************************
 * Function Definitions
 ******************************************************************************/

uint8_t app_uart_init(void)
{
	uint8_t retVal = APP_UART_OK;
	uint32_t mode = ARM_USART_PARITY_NONE
				| ARM_USART_DATA_BITS_8
				| ARM_USART_STOP_BITS_1
				| ARM_USART_MODE_ASYNCHRONOUS;

	if(uart0_drv->Initialize(app_uart_callback) == ARM_DRIVER_OK)
	{
		if(uart0_drv->Control(mode, USART_BAUDRATE) != ARM_DRIVER_OK)
		{
			retVal = APP_UART_ERROR;
		}
		else
		{
			/* Do nothing */
		}

	}
	else
	{
		retVal = APP_UART_ERROR;
	}

	return retVal;

}

uint8_t app_uart_send_char(char* data, uint32_t timeout)
{
	uint8_t retVal = APP_UART_OK;
	uint32_t timeoutCounter = 0;
	uint8_t length = strlen((const char*)data);

	uart0_drv->Send((uint8_t*)data, length);
	sendDataCompleteFlag = APP_UART_TRANSMIT_INPROGRESS;

	/* Define check with time get tick later... */
	while((timeoutCounter < timeout) && (APP_UART_TRANSMIT_COMPLETE == sendDataCompleteFlag))
	{
		delay(1000*1000);
		timeoutCounter++;
	}

	if(APP_UART_TRANSMIT_COMPLETE == sendDataCompleteFlag)
	{
		sendDataCompleteFlag = APP_UART_TRANSMIT_IDLE;
	}
	else
	{
		sendDataCompleteFlag = APP_UART_TRANSMIT_IDLE;
		retVal = APP_UART_SEND_FAIL;
	}

	return retVal;
}

uint8_t app_uart_receive_char(char* data, uint32_t length, uint32_t timeout)
{
	uint8_t retVal = APP_UART_OK;
	uint32_t timeoutCounter = 0;


	if(length > BUFFER_SIZE)
	{
		retVal = APP_UART_RECEIVE_FAIL;
	}
	else
	{
		uart0_drv->Receive((uint8_t*)data, length);

		/* Define check with time get tick later... */
		while((timeoutCounter < timeout) && (APP_UART_RECEIVE_DATA == receiveDataCompleteFlag))
		{
			timeoutCounter++;
		}

		if(APP_UART_RECEIVE_DATA == receiveDataCompleteFlag)
		{
			receiveDataCompleteFlag = APP_UART_RECEIVE_IDLE;
		}
		else
		{
			retVal = APP_UART_RECEIVE_FAIL;
		}

	}

	return retVal;
}

void app_uart_receive_non_blocking(void)
{
	uart0_drv->Receive((void*)&receivedData, 1);
}


uint8_t app_uart_get_incoming_data(uint8_t* data)
{
	uint8_t retVal = APP_UART_OK;
	if(NULL == data)
	{
		retVal = APP_UART_ERROR;
	}
	else
	{
		if(APP_UART_RECEIVE_DATA == receiveDataCompleteFlag)
		{
			*data = receivedData;
			receiveDataCompleteFlag = APP_UART_RECEIVE_IDLE;
		}
		else
		{
			/* Do nothing */
		}

	}

	return retVal;
}

uint8_t app_uart_get_buffer_size(void)
{
	return receiveBufferCounter;
}


void app_uart_callback(uint32_t event)
{
	if((event & ARM_USART_EVENT_RECEIVE_COMPLETE) != 0U)
	{
		receiveBuffer[receiveBufferCounter] = receivedData;
		receiveDataCompleteFlag = APP_UART_RECEIVE_DATA;
		receiveBufferCounter++;

		if(receiveBufferCounter >= BUFFER_SIZE)
		{
			receiveBufferCounter = 0;
		}

		app_uart_receive_non_blocking();
	}
	else if((event & ARM_USART_EVENT_SEND_COMPLETE) != 0U)
	{
		sendDataCompleteFlag = APP_UART_TRANSMIT_COMPLETE;

	}
	else
	{
		/* Do nothing */
	}
}

void app_uart_get_buffer_data(uint8_t* receiveData)
{
	strcpy((char*)receiveData, (const char*)receiveBuffer);
	receiveBufferCounter = 0;
}
