#include "hal_gpio.h"
#include "my_nvic.h"

#define MAX_CALLBACK_FUNC 10

#define PIN_BUTTON1	 			0
#define PIN_BUTTON2	 			1
#define PIN_LED_RED				2
#define PIN_LED_GREEN			3

const pin_map_t pin_map[] = {
	[PIN_BUTTON1] = {IP_PORTE, IP_PTE, 4, PCC_PORTE_INDEX, PORTE_IRQn},
	[PIN_BUTTON2] = {IP_PORTE, IP_PTE, 5, PCC_PORTE_INDEX, PORTE_IRQn},
	[PIN_LED_RED] = {IP_PORTD, IP_PTD, 15, PCC_PORTD_INDEX, PORTD_IRQn},
	[PIN_LED_GREEN] = {IP_PORTD, IP_PTD, 16, PCC_PORTD_INDEX, PORTD_IRQn}
};

static HAL_GPIO_CallBack_t g_hal_callbacks[MAX_CALLBACK_FUNC];


void HAL_GPIO_EnablePortClock(uint32_t virtual_pin)
{
	IP_PCC->PCCn[pin_map[virtual_pin].pcc_index] |= PCC_PCCn_CGC_MASK;
}

void HAL_GPIO_SetAsGpio(uint32_t virtual_pin)
{
	PORT_Type *config_port = pin_map[virtual_pin].port_base;
	uint32_t physical_pin = pin_map[virtual_pin].pin_num;

	uint32_t pcr_val = config_port->PCR[physical_pin];
	pcr_val &= ~PORT_PCR_MUX_MASK;
	pcr_val |= PORT_PCR_MUX(1);
	config_port->PCR[physical_pin] = pcr_val;
}

void HAL_GPIO_SetDirection(uint32_t virtual_pin, hal_gpio_direction_t dir)
{
	uint8_t physical_pin = pin_map[virtual_pin].pin_num;
	switch (dir)
	{
	case HAL_GPIO_DIR_OUTPUT:
	{
		pin_map[virtual_pin].gpio_base->PDDR |= (1UL << physical_pin);
		break;
	}
	case HAL_GPIO_DIR_INPUT:
	{
		pin_map[virtual_pin].gpio_base->PDDR &= ~(1UL << physical_pin);
		break;
	}
	default:
		break;
	}

}

void HAL_GPIO_WritePin(uint32_t virtual_pin, uint8_t value)
{
	uint8_t physical_pin = pin_map[virtual_pin].pin_num;
	if(1 == value)
	{
		pin_map[virtual_pin].gpio_base->PSOR = (1UL << physical_pin);
	}
	else
	{
		pin_map[virtual_pin].gpio_base->PCOR = (1UL << physical_pin);
	}

}

void HAL_GPIO_TogglePin(uint32_t virtual_pin)
{
	uint8_t physical_pin = pin_map[virtual_pin].pin_num;
	pin_map[virtual_pin].gpio_base->PTOR = (1UL << physical_pin);

}

uint8_t HAL_GPIO_ReadPin(uint32_t virtual_pin)
{
	uint8_t physical_pin = pin_map[virtual_pin].pin_num;
	uint8_t pinMode = (pin_map[virtual_pin].gpio_base->PDDR & (1UL << physical_pin));
	uint8_t pinStatus;
	if(1 == pinMode)
	{
		pinStatus = (pin_map[virtual_pin].gpio_base->PDOR & (1UL << physical_pin));
	}
	else
	{
		pinStatus = (pin_map[virtual_pin].gpio_base->PDIR & (1UL << physical_pin));
	}

	return pinStatus;

}

void HAL_GPIO_SetOutputMode(uint32_t virtual_pin, hal_gpio_output_mode_t mode)
{
	switch (mode)
	{
	case HAL_GPIO_OPEN_DRAIN:
	{
		break;
	}
	case HAL_GPIO_PUSH_PULL:
	{
		break;
	}
	default:
		break;
	}


}

void HAL_GPIO_SetPullResistor(uint32_t virtual_pin, hal_gpio_pull_resistor_t resistor)
{
	PORT_Type* config_port = pin_map[virtual_pin].port_base;
	uint8_t physical_pin = pin_map[virtual_pin].pin_num;

	/* Read & clear current pull mode config */
	uint32_t pcr_val = config_port->PCR[physical_pin] & ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);


	switch (resistor)
	{
	case HAL_GPIO_PULL_NONE:
	{
		break;
	}
	case HAL_GPIO_PULL_UP:
	{
		pcr_val |= PORT_PCR_PE(1) | PORT_PCR_PS(1);

		break;
	}
	case HAL_GPIO_PULL_DOWN:
	{
		pcr_val |= PORT_PCR_PE(1) | PORT_PCR_PS(0);

		break;
	}
	default:
		break;
	}

	config_port->PCR[physical_pin] = pcr_val;


}

void HAL_GPIO_SetEventTrigger(uint32_t virtual_pin, hal_gpio_interrupt_trigger_t trigger)
{
	PORT_Type *config_port = pin_map[virtual_pin].port_base;
	uint32_t physical_pin = pin_map[virtual_pin].pin_num;
	uint32_t irqc_value = 0;

	switch (trigger)
	{
		case HAL_GPIO_TRIGGER_RISING_EDGE:
			irqc_value = 9;
			break;
		case HAL_GPIO_TRIGGER_FALLING_EDGE:
			irqc_value = 10;
			break;
		case HAL_GPIO_TRIGGER_EITHER_EDGE:
			irqc_value = 11;
			break;
		case HAL_GPIO_TRIGGER_NONE:
		default:
			irqc_value = 0;
			break;
	}

	/* Read and clear the previous interrupt mode */
	uint32_t pcr_val = config_port->PCR[physical_pin] & ~PORT_PCR_IRQC_MASK;

	pcr_val |= PORT_PCR_IRQC(irqc_value);
	config_port->PCR[physical_pin] = pcr_val;

	/* Activate the NVIC */
	if (HAL_GPIO_TRIGGER_NONE != trigger)
	{
		NVIC->ISER[(uint32_t)pin_map[virtual_pin].irq_num >> 5] = (1UL << ((uint32_t)pin_map[virtual_pin].irq_num & 0x1FUL));
	}
	else
	{
		NVIC->ICER[(uint32_t)pin_map[virtual_pin].irq_num >> 5] = (1UL << ((uint32_t)pin_map[virtual_pin].irq_num & 0x1FUL));
	}

}

uint8_t HAL_GPIO_IsInterruptFlagSet(uint32_t virtual_pin)
{
	uint32_t isfr_val = pin_map[virtual_pin].port_base->ISFR;
	uint32_t pin_mask = (1UL << pin_map[virtual_pin].pin_num);
	return ((isfr_val & pin_mask) != 0);
}

void HAL_GPIO_ClearInterruptFlag(uint32_t virtual_pin)
{
	pin_map[virtual_pin].port_base->ISFR = (1UL << pin_map[virtual_pin].pin_num);
}

uint8_t HAL_GPIO_RegisterCallback(uint32_t virtual_pin, HAL_GPIO_CallBack_t callback)
{
	uint8_t result = 1;

	if(virtual_pin < MAX_CALLBACK_FUNC)
	{
		g_hal_callbacks[virtual_pin] = callback;
	}
	else
	{
		result = 0;
	}

	return result;

}

void PORTE_IRQHandler(void)
{
    if (HAL_GPIO_IsInterruptFlagSet(PIN_BUTTON1))
    {
        HAL_GPIO_ClearInterruptFlag(PIN_BUTTON1);

        if (g_hal_callbacks[PIN_BUTTON1] != NULL)
        {
            g_hal_callbacks[PIN_BUTTON1](PIN_BUTTON1, HAL_GPIO_TRIGGER_FALLING_EDGE);
        }
    }
    else if (HAL_GPIO_IsInterruptFlagSet(PIN_BUTTON2))
	{
		HAL_GPIO_ClearInterruptFlag(PIN_BUTTON2);

		if (g_hal_callbacks[PIN_BUTTON2] != NULL)
		{
			g_hal_callbacks[PIN_BUTTON2](PIN_BUTTON2, HAL_GPIO_TRIGGER_FALLING_EDGE);
		}
	}
}
