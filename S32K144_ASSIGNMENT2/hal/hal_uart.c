/**
 * @file hal_uart.c
 * @author benecosta2711
 * @version 0.1
 * @date 2025-10-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "hal_uart.h"
#include "my_nvic.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @brief Structure for mapping a virtual UART instance to physical resources.
 */
typedef struct
{
    LPUART_Type *const      base;               /* LPUART peripheral base pointer */
    const IRQn_Type         irqNum;             /* LPUART IRQ number */
    const uint32_t          pccIndex;           /* PCC clock gate index for LPUART */
    PORT_Type *const        txPort;             /* PORT base pointer for TX pin */
    const uint32_t          txPin;              /* Pin number for TX */
    const uint32_t          txPinMux;           /* MUX setting for TX pin */
    const uint32_t          txPccIndex;         /* PCC clock gate index for TX PORT */
    PORT_Type *const        rxPort;             /* PORT base pointer for RX pin */
    const uint32_t          rxPin;              /* Pin number for RX */
    const uint32_t          rxPinMux;           /* MUX setting for RX pin */
    const uint32_t          rxPccIndex;         /* PCC clock gate index for RX PORT */
} uart_map_t;

/**
 * @brief Assumed clock source frequency for LPUART module.
 * This example assumes SIRCDIV2 is used.
 * The default SOSC frequency is 8 MHz.
 */
#define LPUART_MODULE_CLOCK_FREQ    8000000UL

/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/

/**
 * @brief Mapping table from virtual UART instance to physical resources.
 */
static const uart_map_t s_uartMap[] = {
    /* Instance HAL_LPUART0: Maps to LPUART0, Pins PTB0 (Rx) and PTB1 (Tx) */
    {
        .base = IP_LPUART0,
        .irqNum = LPUART0_RxTx_IRQn,
        .pccIndex = PCC_LPUART0_INDEX,
        .txPort = IP_PORTB,
        .txPin = 1U,
        .txPinMux = 2U,
        .txPccIndex = PCC_PORTB_INDEX,
        .rxPort = IP_PORTB,
        .rxPin = 0,
        .rxPinMux = 2U,
        .rxPccIndex = PCC_PORTB_INDEX
    },
    /* Instance HAL_LPUART1: Maps to LPUART1, Pins PTC6 (Rx) and PTC7 (Tx) */
    {
        .base = IP_LPUART1,
        .irqNum = LPUART1_RxTx_IRQn,
        .pccIndex = PCC_LPUART1_INDEX,
        .txPort = IP_PORTC,
        .txPin = 7U,
        .txPinMux = 2U,
        .txPccIndex = PCC_PORTC_INDEX,
        .rxPort = IP_PORTC,
        .rxPin = 6U,
        .rxPinMux = 2U,
        .rxPccIndex = PCC_PORTC_INDEX
    },
    /* Instance HAL_LPUART2: Maps to LPUART2, Pins PTA8 (Rx) and PTA9 (Tx) */
    {
        .base = IP_LPUART2,
        .irqNum = LPUART2_RxTx_IRQn,
        .pccIndex = PCC_LPUART2_INDEX,
        .txPort = IP_PORTA,
        .txPin = 9U,
        .txPinMux = 2U,
        .txPccIndex = PCC_PORTA_INDEX,
        .rxPort = IP_PORTA,
        .rxPin = 8U,
        .rxPinMux = 2U,
        .rxPccIndex = PCC_PORTA_INDEX
    }
};

/**
 * @brief Array to store registered callback functions for each UART instance.
 */
static HAL_UART_Callback_t s_uartCallbacks[sizeof(s_uartMap) / sizeof(uart_map_t)];

/*******************************************************************************
 * Code
 ******************************************************************************/

uint8_t HAL_UART_Init(uint32_t instance)
{
    uint8_t retVal = 1;
    const uart_map_t * map = NULL;
    uint32_t pcr_val = 0U;

    if (instance >= (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        retVal = 0;
    }
    else
    {
        map = &s_uartMap[instance];

        /* Enable clock for PORTs */
        IP_PCC->PCCn[map->txPccIndex] |= PCC_PCCn_CGC_MASK;

        /* Configure TX Pin MUX */
        pcr_val = map->txPort->PCR[map->txPin];
        pcr_val &= ~PORT_PCR_MUX_MASK;
        pcr_val |= PORT_PCR_MUX(map->txPinMux);
        map->txPort->PCR[map->txPin] = pcr_val;

        /* Configure RX Pin MUX */
        pcr_val = map->rxPort->PCR[map->rxPin];
        pcr_val &= ~PORT_PCR_MUX_MASK;
        pcr_val |= PORT_PCR_MUX(map->rxPinMux);
        map->rxPort->PCR[map->rxPin] = pcr_val;

        /* Select clock source for LPUART */
        IP_PCC->PCCn[map->pccIndex] &= ~PCC_PCCn_PCS_MASK;
        IP_PCC->PCCn[map->pccIndex] |= PCC_PCCn_PCS(2U);

        /* Enable clock for LPUART peripheral */
        IP_PCC->PCCn[map->pccIndex] |= PCC_PCCn_CGC_MASK;
    }

    return retVal;
}

uint8_t HAL_UART_Configure(uint32_t instance, const hal_uart_config_t *config)
{
    uint8_t retVal = 1;
    LPUART_Type * base = NULL;
    uint32_t sbr = 0U;
    uint32_t osr = 16U;

    if (((sizeof(s_uartMap) / sizeof(uart_map_t)) <= instance) || (NULL == config))
    {
        retVal = 0;
    }
    else
    {
        base = s_uartMap[instance].base;

        /* Disable transmitter and receiver before configuration */
        base->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

        /* Calculate and set Baud Rate */
        sbr = LPUART_MODULE_CLOCK_FREQ / (config->baudRate * osr);
        base->BAUD &= ~LPUART_BAUD_SBR_MASK;
        base->BAUD |= LPUART_BAUD_SBR(sbr);
        base->BAUD &= ~LPUART_BAUD_OSR_MASK;
        base->BAUD |= LPUART_BAUD_OSR(osr - 1U);

        /* Configure Stop Bits */
        base->BAUD &= ~LPUART_BAUD_SBNS_MASK;
        if (HAL_UART_STOP_BITS_2 == config->stopBits)
        {
            base->BAUD |= LPUART_BAUD_SBNS(1U);
        }
        else
        {
            /* Do nothing */
        }

        /* Configure Parity and Data Bits */
        uint32_t ctrl_val = base->CTRL;
        ctrl_val &= ~(LPUART_CTRL_PE_MASK | LPUART_CTRL_PT_MASK | LPUART_CTRL_M_MASK | LPUART_CTRL_M7_MASK);

        if (HAL_UART_PARITY_NONE != config->parity)
        {
            ctrl_val |= LPUART_CTRL_PE(1U);
            if (HAL_UART_PARITY_ODD == config->parity)
            {
                ctrl_val |= LPUART_CTRL_PT(1U);
            }
            else
            {
                /* Do nothing */
            }
        }
        else
        {
            /* Do nothing */
        }

        if (HAL_UART_DATA_BITS_9 == config->dataBits)
        {
            ctrl_val |= LPUART_CTRL_M(1U);
        }
        else
        {
            /* Do nothing */
        }

        base->CTRL = ctrl_val;
    }

    return retVal;
}

void HAL_UART_Deinit(uint32_t instance)
{
    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        /* Disable interrupts, transmitter and receiver */
        s_uartMap[instance].base->CTRL = 0U;

        /* Disable LPUART clock gate */
        IP_PCC->PCCn[s_uartMap[instance].pccIndex] &= ~PCC_PCCn_CGC_MASK;
    }
}

void HAL_UART_RegisterCallback(uint32_t instance, HAL_UART_Callback_t callback)
{
    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        s_uartCallbacks[instance] = callback;
    }
    else
    {
        /* Do nothing */
    }
}

void HAL_UART_SendByteBlocking(uint32_t instance, uint8_t data)
{
    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        /* Wait until transmit data register is empty */
        while ((s_uartMap[instance].base->STAT & LPUART_STAT_TDRE_MASK) == 0U) {}
        
        s_uartMap[instance].base->DATA = (uint8_t)data;
    }
    else
    {
        /* Do nothing */
    }

}

uint8_t HAL_UART_ReadByteBlocking(uint32_t instance)
{
    uint8_t data = 0U;

    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        /* Wait until receive data register is full */
        while ((s_uartMap[instance].base->STAT & LPUART_STAT_RDRF_MASK) == 0U) {}
        
        data = (uint8_t)s_uartMap[instance].base->DATA;
    }
    else
    {
        /* Do nothing */
    }

    return data;
}

void HAL_UART_EnableInterrupts(uint32_t instance, uint32_t interruptMask)
{
    uint32_t ctrl_val = 0U;

    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        ctrl_val = s_uartMap[instance].base->CTRL;

        if ((interruptMask & HAL_UART_INT_TX_DATA_REG_EMPTY) != 0U)
        {
            ctrl_val |= LPUART_CTRL_TIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        if ((interruptMask & HAL_UART_INT_TX_COMPLETE) != 0U)
        {
            ctrl_val |= LPUART_CTRL_TCIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        if ((interruptMask & HAL_UART_INT_RX_DATA_REG_FULL) != 0U)
        {
            ctrl_val |= LPUART_CTRL_RIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        if ((interruptMask & HAL_UART_INT_RX_OVERRUN) != 0U)
        {
            ctrl_val |= LPUART_CTRL_ORIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        s_uartMap[instance].base->CTRL = ctrl_val;

        NVIC->ISER[(uint32_t)s_uartMap[instance].irqNum >> 5U] = (1UL << ((uint32_t)s_uartMap[instance].irqNum & 0x1FUL));
    }
    else
    {
        /* Do nothing */
    }
}

void HAL_UART_DisableInterrupts(uint32_t instance, uint32_t interruptMask)
{
    uint32_t ctrl_val = 0U;

    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        ctrl_val = s_uartMap[instance].base->CTRL;

        if ((interruptMask & HAL_UART_INT_TX_DATA_REG_EMPTY) != 0U)
        {
            ctrl_val &= ~LPUART_CTRL_TIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        if ((interruptMask & HAL_UART_INT_TX_COMPLETE) != 0U)
        {
            ctrl_val &= ~LPUART_CTRL_TCIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        if ((interruptMask & HAL_UART_INT_RX_DATA_REG_FULL) != 0U)
        {
            ctrl_val &= ~LPUART_CTRL_RIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        if ((interruptMask & HAL_UART_INT_RX_OVERRUN) != 0U)
        {
            ctrl_val &= ~LPUART_CTRL_ORIE_MASK;
        }
        else
        {
            /* Do nothing */
        }

        s_uartMap[instance].base->CTRL = ctrl_val;
    }
    else
    {
        /* Do nothing */
    }
}

void HAL_UART_EnableTransmitter(uint32_t instance, uint8_t enable)
{
    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        if (enable)
        {
            s_uartMap[instance].base->CTRL |= LPUART_CTRL_TE_MASK;
        }
        else
        {
            s_uartMap[instance].base->CTRL &= ~LPUART_CTRL_TE_MASK;
        }
    }
    else
    {
        /* Do nothing */
    }
}

void HAL_UART_EnableReceiver(uint32_t instance, uint8_t enable)
{
    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        if (enable)
        {
            s_uartMap[instance].base->CTRL |= LPUART_CTRL_RE_MASK;
        }
        else
        {
            s_uartMap[instance].base->CTRL &= ~LPUART_CTRL_RE_MASK;
        }
    }
    else
    {
        /* Do nothing */
    }
}

uint32_t HAL_UART_GetStatusFlags(uint32_t instance)
{
    uint32_t stat_val = 0U;

    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        stat_val = s_uartMap[instance].base->STAT;
    }
    else
    {
        /* Do nothing */
    }

    return stat_val;
}

/**
 * @brief Common IRQ Handler for LPUART instances.
 * This function should be called from the specific IRQ handlers.
 */
static void HAL_UART_IRQHandler(uint32_t instance)
{
    uint32_t stat = 0U;
    uint32_t events = 0U;

    if (instance < (sizeof(s_uartMap) / sizeof(uart_map_t)))
    {
        stat = s_uartMap[instance].base->STAT;

        if (((stat & LPUART_STAT_RDRF_MASK) != 0U) && ((s_uartMap[instance].base->CTRL & LPUART_CTRL_RIE_MASK) != 0U))
        {
            events |= ARM_USART_EVENT_RECEIVE_COMPLETE; /* (1UL << 1) */
        }
        else
        {
            /* Do nothing */
        }

        if (((stat & LPUART_STAT_TC_MASK) != 0U) && ((s_uartMap[instance].base->CTRL & LPUART_CTRL_TCIE_MASK) != 0U))
        {
            events |= ARM_USART_EVENT_TX_COMPLETE; /* (1UL << 3) */
        }
        else
        {
            /* Do nothing */
        }

        /* Check Overrun (OR) */
        if ((stat & LPUART_STAT_OR_MASK) != 0U)
        {
            events |= ARM_USART_EVENT_RX_OVERFLOW; /* (1UL << 5) */
            /* Clear Overrun flag */
            s_uartMap[instance].base->STAT |= LPUART_STAT_OR_MASK;
        }
        else
        {
            /* Do nothing */
        }

        if ((events != 0U) && (s_uartCallbacks[instance] != NULL))
        {
            s_uartCallbacks[instance](events);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        /* Do nothing */
    }
}


/* Specific IRQ Handlers for each LPUART instance */
void LPUART0_RxTx_IRQHandler(void)
{
    HAL_UART_IRQHandler(HAL_LPUART0);
}

void LPUART1_RxTx_IRQHandler(void)
{
    HAL_UART_IRQHandler(HAL_LPUART1);
}

void LPUART2_RxTx_IRQHandler(void)
{
    HAL_UART_IRQHandler(HAL_LPUART2);
}
