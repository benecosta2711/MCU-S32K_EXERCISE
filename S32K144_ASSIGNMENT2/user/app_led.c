/**
 * @file app_led.c
 * @author benecosta2711
 * @brief 
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "app_led.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* CMSIS Driver manager struct */
extern ARM_DRIVER_GPIO Driver_GPIO0;
ARM_DRIVER_GPIO *gpio_drv = &Driver_GPIO0;

/* Manage led status */
static uint32_t ledStatus = 0;

/*******************************************************************************
 * Function Definitions
 ******************************************************************************/
void app_led_init(void)
{
    gpio_drv->Setup(PIN_LED_RED, NULL);
    gpio_drv->SetDirection(PIN_LED_RED, ARM_GPIO_OUTPUT);
    gpio_drv->SetOutput(PIN_LED_RED, 1);

    gpio_drv->Setup(PIN_LED_GREEN, NULL);
    gpio_drv->SetDirection(PIN_LED_GREEN, ARM_GPIO_OUTPUT);
    gpio_drv->SetOutput(PIN_LED_GREEN, 1);

    gpio_drv->Setup(PIN_LED_BLUE, NULL);
    gpio_drv->SetDirection(PIN_LED_BLUE, ARM_GPIO_OUTPUT);
    gpio_drv->SetOutput(PIN_LED_BLUE, 1);
}

void app_led_control(uint8_t led, uint8_t cmd)
{
    gpio_drv->SetOutput(led, cmd);
    if (LED_ON == cmd)
    {
        ledStatus |= (1 << led);
    }
    else
    {
        ledStatus &= ~(1 << led);
    }
}

uint32_t app_led_get_status(void)
{
    return ledStatus;
}
