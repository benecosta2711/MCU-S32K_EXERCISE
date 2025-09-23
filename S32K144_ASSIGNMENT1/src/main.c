/**
 * @file    main.c
 * @author  Nguyen Vuong Trung Nam
 * @brief   Application entry point.
 * @details Hiện thực driver cho gpio và ứng dụng vào chương trình nhấp nháy led dựa trên sự kiện từ nút nhấn,
 * áp dụng các kiến thức về ngắt và gpio.
 */


#include "S32K144.h"
#include <stdio.h>

#include "Driver_GPIO.h"
#include "hal_gpio.h"

#if defined (__ghs__)
    #define __INTERRUPT_SVC  __interrupt
    #define __NO_RETURN _Pragma("ghs nowarning 111")
#elif defined (__ICCARM__)
    #define __INTERRUPT_SVC  __svc
    #define __NO_RETURN _Pragma("diag_suppress=Pe111")
#elif defined (__GNUC__)
    #define __INTERRUPT_SVC  __attribute__ ((interrupt ("SVC")))
    #define __NO_RETURN
#else
    #define __INTERRUPT_SVC
    #define __NO_RETURN
#endif

void delay_ms(uint32_t duration);

volatile uint8_t g_button_event_flag = 0;

void PORTE_IRQHandler(void)
{

    if (HAL_GPIO_IsInterruptFlagSet(1))
    {
        HAL_GPIO_ClearInterruptFlag(1);

        g_button_event_flag = 1;
    }
}

int main(void) {
	HAL_GPIO_EnablePortClock(0);
	HAL_GPIO_SetAsGpio(0);
	HAL_GPIO_SetDirection(0, HAL_GPIO_DIR_OUTPUT);

	HAL_GPIO_EnablePortClock(1);
	HAL_GPIO_SetAsGpio(1);
	HAL_GPIO_SetDirection(1, HAL_GPIO_DIR_INPUT);
	HAL_GPIO_SetPullResistor(1, HAL_GPIO_PULL_NONE);
	HAL_GPIO_SetEventTrigger(1, HAL_GPIO_TRIGGER_FALLING_EDGE);


	for(;;)
	{
		if (g_button_event_flag)
		{

			g_button_event_flag = 0;


			HAL_GPIO_TogglePin(0);
		}
		delay_ms(1000);
	}


    /* to avoid the warning message for GHS and IAR: statement is unreachable*/
    __NO_RETURN
    return 0;
}

void delay_ms(uint32_t duration)
{
	while(duration--);
}
