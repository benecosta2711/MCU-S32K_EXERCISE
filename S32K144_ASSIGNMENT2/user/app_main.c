/**
 * @file app_main.c
 * @author benecosta2711
 * @brief 
 * @version 0.1
 * @date 2025-10-09
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "app_main.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
static system_cmd_t systemCmd = IDLE;

/*******************************************************************************
 * Function Definitions
 ******************************************************************************/
uint8_t app_main_init(void)
{
    uint8_t retVal = APP_INIT_OK;
    if (app_uart_init() != APP_UART_OK)
    {
        retVal = APP_INIT_FAIL;
    }
    else
    {
        app_led_init();
    }

    /* Start receive command */
    app_uart_receive_non_blocking();

    return retVal;
}

void app_event_parser(void)
{
    static uint8_t receiveByte = 0;
    if (APP_UART_OK == app_uart_get_incoming_data(&receiveByte))
    {
        if (('\n' == receiveByte))
        {
            uint8_t receiveLength = app_uart_get_buffer_size();
            uint8_t receiveData[receiveLength];
            app_uart_get_buffer_data(receiveData);

            uint8_t cmdLength = receiveLength - 1;
            uint8_t cmdData[cmdLength];

            strncpy((char *)cmdData, (const char *)receiveData, cmdLength);
            cmdData[cmdLength - 1] = '\0';

            if (strcmp((const char *)cmdData, CMD_GET_LED_STATUS) == 0U)
            {
                systemCmd = GET_LED_STATUS;
            }
            else if (strcmp((const char *)cmdData, CMD_HELP) == 0U)
            {
                systemCmd = SHOW_HELP_INFO;
            }
            else if (strcmp((const char *)cmdData, CMD_BLUE_ON) == 0U)
            {
                systemCmd = TURN_BLUE_ON;
            }
            else if (strcmp((const char *)cmdData, CMD_BLUE_OFF) == 0U)
            {
                systemCmd = TURN_BLUE_OFF;
            }
            else if (strcmp((const char *)cmdData, CMD_GREEN_ON) == 0U)
            {
                systemCmd = TURN_GREEN_ON;
            }
            else if (strcmp((const char *)cmdData, CMD_GREEN_OFF) == 0U)
            {
                systemCmd = TURN_GREEN_OFF;
            }
            else if (strcmp((const char *)cmdData, CMD_RED_ON) == 0U)
            {
                systemCmd = TURN_RED_ON;
            }
            else if (strcmp((const char *)cmdData, CMD_RED_OFF) == 0U)
            {
                systemCmd = TURN_RED_OFF;
            }
            else
            {
                systemCmd = UNKNOWN_CMD;
            }

            receiveByte = '\0';
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

void app_run_fsm(void)
{
    uint8_t ledStatus;
    char statusMsg[33];

    switch (systemCmd)
    {
    case IDLE:
        /* Wait for processing event */
        break;
    case GET_LED_STATUS:
        ledStatus = app_led_get_status();
        memset(statusMsg, 0, strlen(statusMsg));
        snprintf(statusMsg, sizeof(statusMsg), "STATUS: RED=%d, GREEN=%d, BLUE=%d\r\n",
                 (ledStatus & LED_RED_STATE_MSK) ? 1 : 0,
                 (ledStatus & LED_GREEN_STATE_MSK) ? 1 : 0,
                 (ledStatus & LED_BLUE_STATE_MSK) ? 1 : 0);
        app_uart_send_char(statusMsg, 100000000);

        systemCmd = IDLE;
        break;
    case SHOW_HELP_INFO:
        app_uart_send_char((char *)"--- LED Control Guidline ---\r\nLED STATUS: Get all LED states\r\nRED/GREEN/BLUE ON/OFF: Control a LED\r\n", 100000);

        systemCmd = IDLE;
        break;
    case TURN_BLUE_ON:
        app_led_control(PIN_LED_BLUE, TURN_ON);

        systemCmd = IDLE;
        break;
    case TURN_BLUE_OFF:
        app_led_control(PIN_LED_BLUE, TURN_OFF);

        systemCmd = IDLE;
        break;
    case TURN_RED_ON:
        app_led_control(PIN_LED_RED, TURN_ON);

        systemCmd = IDLE;
        break;
    case TURN_RED_OFF:
        app_led_control(PIN_LED_RED, TURN_OFF);

        systemCmd = IDLE;
        break;
    case TURN_GREEN_ON:
        app_led_control(PIN_LED_GREEN, TURN_ON);

        systemCmd = IDLE;
        break;
    case TURN_GREEN_OFF:
        app_led_control(PIN_LED_GREEN, TURN_OFF);

        systemCmd = IDLE;
        break;
    case UNKNOWN_CMD:
        app_uart_send_char((char *)"Not recognized as a command, type \"HELP\" for more information\r\n", 1000);

        systemCmd = IDLE;
        break;
    default:
        break;
    }
}
