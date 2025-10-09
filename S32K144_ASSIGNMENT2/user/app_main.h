/**
 * @file app_main.h
 * @author benecosta
 * @brief A library provide all function that needed to run the main application, including:
 * - Init all the related peripheral for this application, through other supported app.
 * - Processing data and main program flow using fsm.
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef APP_MAIN_H_
#define APP_MAIN_H_

#include "S32K144.h"
#include "string.h"
#include <stdio.h>
#include "app_uart.h"
#include "app_led.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Define main application status */
#define APP_INIT_FAIL   0
#define APP_INIT_OK     1

#define LED_NUM         3

/* Define command from user */
#define CMD_RED_ON          (const char *)"RED_ON"
#define CMD_RED_OFF         (const char *)"RED_OFF"
#define CMD_GREEN_ON        (const char *)"GREEN_ON"
#define CMD_GREEN_OFF       (const char *)"GREEN_OFF"
#define CMD_BLUE_ON         (const char *)"BLUE_ON"
#define CMD_BLUE_OFF        (const char *)"BLUE_OFF"
#define CMD_GET_LED_STATUS  (const char *)"LED_STATUS"
#define CMD_HELP            (const char *)"HELP"

/*******************************************************************************
 * Structures
 ******************************************************************************/
/* Manage system processing event */
typedef enum
{
    IDLE,
    GET_LED_STATUS,
    TURN_RED_ON,
    TURN_RED_OFF,
    TURN_GREEN_ON,
    TURN_GREEN_OFF,
    TURN_BLUE_ON,
    TURN_BLUE_OFF,
    SHOW_HELP_INFO,
    UNKNOWN_CMD
} system_cmd_t;

/*******************************************************************************
 * Function Prototypes
 ******************************************************************************/
uint8_t app_main_init(void);
void app_event_parser(void);
void app_run_fsm(void);

#endif /* APP_MAIN_H_ */
