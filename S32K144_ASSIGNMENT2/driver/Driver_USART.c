#include "Driver_USART.h"
#include "hal_uart.h"

#define ARM_USART_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)  /* driver version */

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = { 
    ARM_USART_API_VERSION,
    ARM_USART_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_USART_CAPABILITIES DriverCapabilities = {
    1, /* supports UART (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports UART Single-wire mode */
    0, /* supports UART IrDA mode */
    0, /* supports UART Smart Card mode */
    0, /* Smart Card Clock generator available */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USART_EVENT_RI */
    0  /* Reserved (must be zero) */
};

//
//   Functions
//

static ARM_DRIVER_VERSION ARM_USART_GetVersion(void)
{
  return DriverVersion;
}

static ARM_USART_CAPABILITIES ARM_USART_GetCapabilities(void)
{
  return DriverCapabilities;
}

static int32_t ARM_USART_Initialize(ARM_USART_SignalEvent_t cb_event)
{
	int32_t retVal = ARM_DRIVER_OK;

	/* Enable clock for related peripheral and config alt for pin */
	if(HAL_UART_Init(HAL_LPUART1) == 1)
	{
		/* Config base on driver capacibilities */
		if(NULL != cb_event)
		{
			/* Enable all possible interrupt to create event from hardware to the application */
			HAL_UART_RegisterCallback(HAL_LPUART1, cb_event);
		}
		else
		{
			/* Do nothing */
		}

		HAL_UART_EnableTransmitter(HAL_LPUART1, 1);

	}
	else
	{
		retVal = ARM_DRIVER_ERROR;
	}

	return retVal;
}

static int32_t ARM_USART_Uninitialize(void)
{
	int32_t retVal = ARM_DRIVER_OK;

	HAL_UART_Deinit(HAL_LPUART1);

	return retVal;

}

static int32_t ARM_USART_PowerControl(ARM_POWER_STATE state)
{
    switch (state)
    {
    case ARM_POWER_OFF:
        break;

    case ARM_POWER_LOW:
        break;

    case ARM_POWER_FULL:
        break;
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_USART_Send(const void *data, uint32_t num)
{
	int32_t retVal = ARM_DRIVER_OK;

	if(NULL == data || 0 == num)
	{
		retVal = ARM_DRIVER_ERROR;
	}
	else
	{
		uint8_t* txData = (uint8_t*)data;

		g_txBuffer = txData;
		g_txBufferLength = num;

		HAL_UART_EnableInterrupts(HAL_LPUART1, HAL_UART_INT_TX_DATA_REG_EMPTY);
	}

	return retVal;

}

static int32_t ARM_USART_Receive(void *data, uint32_t num)
{
	int32_t retVal = ARM_DRIVER_OK;

	if(NULL == data || 0 == num)
	{
		retVal = ARM_DRIVER_ERROR;
	}
	else
	{
		uint8_t* rxData = (uint8_t*)data;

		g_rxBuffer = rxData;
		g_rxBufferLength = num;

		HAL_UART_EnableInterrupts(HAL_LPUART1, HAL_UART_INT_RX_DATA_REG_FULL);
	}


	return retVal;
}

static int32_t ARM_USART_Transfer(const void *data_out, void *data_in, uint32_t num)
{
	return 0;
}

static uint32_t ARM_USART_GetTxCount(void)
{
	return 0;
}

static uint32_t ARM_USART_GetRxCount(void)
{
	return 0;
}

static int32_t ARM_USART_Control(uint32_t control, uint32_t arg)
{
	hal_uart_config_t uartConfig;
	uint32_t retVal = ARM_DRIVER_OK;
	uint32_t mode = control & ARM_USART_CONTROL_Msk;

	if(ARM_USART_MODE_ASYNCHRONOUS != mode)
	{
		retVal = ARM_DRIVER_ERROR;
	}
	else
	{
		uartConfig.baudRate = arg;

		switch (control & ARM_USART_DATA_BITS_Msk)
		{
		case ARM_USART_DATA_BITS_5:
			break;
		case ARM_USART_DATA_BITS_6:
			break;
		case ARM_USART_DATA_BITS_7:
			break;
		case ARM_USART_DATA_BITS_8:
			uartConfig.dataBits = HAL_UART_DATA_BITS_8;
			break;
		case ARM_USART_DATA_BITS_9:
			uartConfig.dataBits = HAL_UART_DATA_BITS_9;
			break;
		default:
			break;
		}

		switch (control & ARM_USART_PARITY_Msk)
		{
		case ARM_USART_PARITY_EVEN:
			uartConfig.parity =  HAL_UART_PARITY_EVEN;
			break;
		case ARM_USART_PARITY_ODD:
			uartConfig.parity =  HAL_UART_PARITY_ODD;
			break;
		case ARM_USART_PARITY_NONE:
			uartConfig.parity =  HAL_UART_PARITY_NONE;
			break;
		default:
			break;
		}

		switch (control & ARM_USART_STOP_BITS_Msk)
		{
		case ARM_USART_STOP_BITS_0_5:
			break;
		case ARM_USART_STOP_BITS_1:
			uartConfig.stopBits = HAL_UART_STOP_BITS_1;
			break;
		case ARM_USART_STOP_BITS_1_5:
			break;
		case ARM_USART_STOP_BITS_2:
			uartConfig.stopBits = HAL_UART_STOP_BITS_2;
			break;
		default:
			break;
		}

		if(HAL_UART_Configure(HAL_LPUART1, &uartConfig) == 0)
		{
			retVal = ARM_DRIVER_ERROR;
		}
		else
		{
			/* Do nothing */
		}
	}

	if(retVal == ARM_DRIVER_OK)
	{
		HAL_UART_EnableTransmitter(HAL_LPUART1, 1);
		HAL_UART_EnableReceiver(HAL_LPUART1, 1);
	}
	else
	{
		/* Do nothing */
	}


	return retVal;
}

static ARM_USART_STATUS ARM_USART_GetStatus(void)
{
	ARM_USART_STATUS retVal = {
			.tx_busy = 0,
			.rx_busy = 0,
			.tx_underflow = 0,
			.rx_overflow = 0,
			.rx_break = 0,
			.rx_framing_error = 0,
			.rx_parity_error = 0,
			.reserved = 0
	};
	uint32_t stat_reg = HAL_UART_GetStatusFlags(HAL_LPUART1);


	retVal.tx_busy = (NULL == g_txBuffer) ? 0 : 1;
	retVal.rx_busy = (NULL == g_rxBuffer) ? 0 : 1;
	retVal.rx_framing_error = (stat_reg & LPUART_STAT_FE_MASK) ? 1 : 0;
	retVal.rx_overflow = (stat_reg & LPUART_STAT_OR_MASK) ? 1 : 0;
	retVal.rx_parity_error = (stat_reg & LPUART_STAT_PF_MASK) ? 1 : 0;

	return retVal;
}

static int32_t ARM_USART_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
	return 0;
}

static ARM_USART_MODEM_STATUS ARM_USART_GetModemStatus(void)
{
	/* Dummy data */
	ARM_USART_MODEM_STATUS retVal = {
			.cts = 0,
			.dcd = 0,
			.dsr = 0,
			.reserved = 0,
			.ri = 0
	};


	return retVal;
}

static void ARM_USART_SignalEvent(uint32_t event)
{
    // function body
}

// End USART Interface

extern \
ARM_DRIVER_USART Driver_USART0;
ARM_DRIVER_USART Driver_USART0 = {
    ARM_USART_GetVersion,
    ARM_USART_GetCapabilities,
    ARM_USART_Initialize,
    ARM_USART_Uninitialize,
    ARM_USART_PowerControl,
    ARM_USART_Send,
    ARM_USART_Receive,
    ARM_USART_Transfer,
    ARM_USART_GetTxCount,
    ARM_USART_GetRxCount,
    ARM_USART_Control,
    ARM_USART_GetStatus,
    ARM_USART_SetModemControl,
    ARM_USART_GetModemStatus
};
