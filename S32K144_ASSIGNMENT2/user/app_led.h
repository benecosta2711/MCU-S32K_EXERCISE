/**
 * @file app_led.h
 * @author benecosta2711
 * @brief A library define function supporting main application, including:
 * - Manage this application led status.
 * - Provide function for controlling led.
 * - Init all related peripherals for led.
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APP_LED_H_
#define APP_LED_H_
#include "S32K144.h"
#include "Driver_GPIO.h"
#include "string.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Define led application error code*/
#define APP_LED_ERROR 	0
#define APP_LED_OK 		1

/* Define led pin used in this application */
#define PIN_LED_BLUE 		0
#define PIN_LED_RED 		1
#define PIN_LED_GREEN 		2

/* Led status mask*/
#define LED_BLUE_STATE_MSK	(1 << PIN_LED_BLUE)
#define LED_RED_STATE_MSK	(1 << PIN_LED_RED)
#define LED_GREEN_STATE_MSK	(1 << PIN_LED_GREEN)

#define LED_NUM 3

/* Control led command, all the led on S32K144 EVB is active low */
#define TURN_OFF 1
#define TURN_ON 0

/* Define led state */
#define LED_ON 1
#define LED_OFF 0

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
void app_led_init(void);
void app_led_control(uint8_t led, uint8_t cmd);
uint32_t app_led_get_status(void);


#endif /* APP_LED_H_ */
