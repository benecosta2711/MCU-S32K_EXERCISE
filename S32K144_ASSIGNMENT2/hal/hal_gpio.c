#include "hal_gpio.h"
#include "my_nvic.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Định nghĩa các pin ảo được sử dụng trong dự án */
#define PIN_LED_BLUE            0U
#define PIN_LED_RED             1U
#define PIN_LED_GREEN           2U

/* Tổng số pin ảo được quản lý bởi HAL */
#define HAL_VIRTUAL_PIN_COUNT   (sizeof(s_pinMap) / sizeof(pin_map_t))

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* Không có hàm static nội bộ */

/*******************************************************************************
 * Variables
 ******************************************************************************/

/**
 * @brief Bảng ánh xạ từ pin ảo sang thông tin phần cứng.
 */
static const pin_map_t s_pinMap[] = {
	[PIN_LED_BLUE]   = {IP_PORTD, IP_PTD, 0U, PCC_PORTD_INDEX, PORTD_IRQn},
    [PIN_LED_RED]   = {IP_PORTD, IP_PTD, 15U, PCC_PORTD_INDEX, PORTD_IRQn},
    [PIN_LED_GREEN] = {IP_PORTD, IP_PTD, 16U, PCC_PORTD_INDEX, PORTD_IRQn}
};

/**
 * @brief Mảng lưu trữ các con trỏ hàm callback.
 * Khai báo là 'static' để bảo vệ dữ liệu, chỉ truy cập qua API.
 */
static HAL_GPIO_Callback_t s_gpioCallbacks[sizeof(s_pinMap) / sizeof(pin_map_t)];


/*******************************************************************************
 * Code
 ******************************************************************************/

uint8_t HAL_GPIO_Init(uint32_t virtual_pin)
{
    uint8_t retVal = true;
    PORT_Type * port = NULL;
    uint32_t physical_pin = 0U;
    uint32_t pcr_val = 0U;

    if (virtual_pin >= HAL_VIRTUAL_PIN_COUNT)
    {
        retVal = false;
    }
    else
    {
        /* Bật clock cho Port */
        IP_PCC->PCCn[s_pinMap[virtual_pin].pcc_index] |= PCC_PCCn_CGC_MASK;

        /* Cấu hình MUX là GPIO (dùng Read-Modify-Write) */
        port = s_pinMap[virtual_pin].port_base;
        physical_pin = s_pinMap[virtual_pin].pin_num;
        pcr_val = port->PCR[physical_pin];
        pcr_val &= ~PORT_PCR_MUX_MASK;
        pcr_val |= PORT_PCR_MUX(1U);
        port->PCR[physical_pin] = pcr_val;
    }

    return retVal;
}

uint8_t HAL_GPIO_RegisterCallback(uint32_t virtual_pin, HAL_GPIO_Callback_t callback)
{
    uint8_t retVal = true;

    if (virtual_pin >= HAL_VIRTUAL_PIN_COUNT)
    {
        retVal = false;
    }
    else
    {
        s_gpioCallbacks[virtual_pin] = callback;
    }

    return retVal;
}

void HAL_GPIO_SetDirection(uint32_t virtual_pin, hal_gpio_direction_t dir)
{
    uint32_t physical_pin = 0U;

    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        physical_pin = s_pinMap[virtual_pin].pin_num;
        if (dir == HAL_GPIO_DIR_OUTPUT)
        {
            s_pinMap[virtual_pin].gpio_base->PDDR |= (1UL << physical_pin);
        }
        else
        {
            s_pinMap[virtual_pin].gpio_base->PDDR &= ~(1UL << physical_pin);
        }
    }
}

void HAL_GPIO_WritePin(uint32_t virtual_pin, uint8_t value)
{
    uint32_t physical_pin = 0U;

    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        physical_pin = s_pinMap[virtual_pin].pin_num;
        if (value) /* value == true */
        {
            s_pinMap[virtual_pin].gpio_base->PSOR = (1UL << physical_pin);
        }
        else
        {
            s_pinMap[virtual_pin].gpio_base->PCOR = (1UL << physical_pin);
        }
    }
}

void HAL_GPIO_TogglePin(uint32_t virtual_pin)
{
    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        s_pinMap[virtual_pin].gpio_base->PTOR = (1UL << s_pinMap[virtual_pin].pin_num);
    }
}

uint8_t HAL_GPIO_ReadPin(uint32_t virtual_pin)
{
    uint32_t physical_pin = 0U;
    uint32_t pin_mask = 0U;
    uint32_t pin_status = 0U;
    uint8_t retVal = false;

    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        physical_pin = s_pinMap[virtual_pin].pin_num;
        pin_mask = (1UL << physical_pin);

        if ((s_pinMap[virtual_pin].gpio_base->PDDR & pin_mask) != 0U) /* Pin là output */
        {
            pin_status = (s_pinMap[virtual_pin].gpio_base->PDOR & pin_mask);
        }
        else /* Pin là input */
        {
            pin_status = (s_pinMap[virtual_pin].gpio_base->PDIR & pin_mask);
        }

        retVal = (pin_status != 0U);
    }

    return retVal;
}

void HAL_GPIO_SetOutputMode(uint32_t virtual_pin, hal_gpio_output_mode_t mode)
{
    PORT_Type * port = NULL;
    uint32_t physical_pin = 0U;

    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        port = s_pinMap[virtual_pin].port_base;
        physical_pin = s_pinMap[virtual_pin].pin_num;

        if (mode == HAL_GPIO_OPEN_DRAIN)
        {
            port->PCR[physical_pin] |= (1UL << 5U); /* Bit 5 (ODE) = 1 */
        }
        else /* HAL_GPIO_PUSH_PULL */
        {
            port->PCR[physical_pin] &= ~(1UL << 5U); /* Bit 5 (ODE) = 0 */
        }
    }
}

void HAL_GPIO_SetPullResistor(uint32_t virtual_pin, hal_gpio_pull_resistor_t resistor)
{
    PORT_Type * port = NULL;
    uint32_t physical_pin = 0U;
    uint32_t pcr_val = 0U;

    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        port = s_pinMap[virtual_pin].port_base;
        physical_pin = s_pinMap[virtual_pin].pin_num;

        pcr_val = port->PCR[physical_pin] & ~(PORT_PCR_PE_MASK | PORT_PCR_PS_MASK);

        switch (resistor)
        {
            case HAL_GPIO_PULL_UP:
                pcr_val |= PORT_PCR_PE(1U) | PORT_PCR_PS(1U);
                break;
            case HAL_GPIO_PULL_DOWN:
                pcr_val |= PORT_PCR_PE(1U) | PORT_PCR_PS(0U);
                break;
            case HAL_GPIO_PULL_NONE:
            default:
                /* No action needed */
                break;
        }

        port->PCR[physical_pin] = pcr_val;
    }
}

void HAL_GPIO_SetEventTrigger(uint32_t virtual_pin, hal_gpio_interrupt_trigger_t trigger)
{
    PORT_Type * port = NULL;
    uint32_t physical_pin = 0U;
    uint32_t irqc_value = 0U;
    uint32_t pcr_val = 0U;

    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        port = s_pinMap[virtual_pin].port_base;
        physical_pin = s_pinMap[virtual_pin].pin_num;

        switch (trigger)
        {
            case HAL_GPIO_TRIGGER_RISING_EDGE:
                irqc_value = 9U;
                break;
            case HAL_GPIO_TRIGGER_FALLING_EDGE:
                irqc_value = 10U;
                break;
            case HAL_GPIO_TRIGGER_EITHER_EDGE:
                irqc_value = 11U;
                break;
            case HAL_GPIO_TRIGGER_NONE:
            default:
                irqc_value = 0U;
                break;
        }

        pcr_val = port->PCR[physical_pin] & ~PORT_PCR_IRQC_MASK;
        pcr_val |= PORT_PCR_IRQC(irqc_value);
        port->PCR[physical_pin] = pcr_val;

        port->ISFR = (1UL << physical_pin);

        if (trigger != HAL_GPIO_TRIGGER_NONE)
        {
            NVIC->ISER[(uint32_t)s_pinMap[virtual_pin].irq_num >> 5U] = (1UL << ((uint32_t)s_pinMap[virtual_pin].irq_num & 0x1FUL));
        }
        else
        {
            NVIC->ICER[(uint32_t)s_pinMap[virtual_pin].irq_num >> 5U] = (1UL << ((uint32_t)s_pinMap[virtual_pin].irq_num & 0x1FUL));
        }
    }
}

uint8_t HAL_GPIO_IsInterruptFlagSet(uint32_t virtual_pin)
{
    uint32_t isfr_val = 0U;
    uint32_t pin_mask = 0U;
    uint8_t retVal = false;

    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        isfr_val = s_pinMap[virtual_pin].port_base->ISFR;
        pin_mask = (1UL << s_pinMap[virtual_pin].pin_num);
        retVal = ((isfr_val & pin_mask) != 0U);
    }

    return retVal;
}

void HAL_GPIO_ClearInterruptFlag(uint32_t virtual_pin)
{
    if (virtual_pin < HAL_VIRTUAL_PIN_COUNT)
    {
        s_pinMap[virtual_pin].port_base->ISFR = (1UL << s_pinMap[virtual_pin].pin_num);
    }
}

void PORTE_IRQHandler(void)
{
}
