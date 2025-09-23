#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

#include "S32K144.h"
#include "stdint.h"


typedef enum {
	HAL_GPIO_DIR_INPUT,
	HAL_GPIO_DIR_OUTPUT
} hal_gpio_direction_t;

typedef enum {
	HAL_GPIO_PUSH_PULL,
	HAL_GPIO_OPEN_DRAIN
} hal_gpio_output_mode_t;

typedef enum {
	HAL_GPIO_PULL_NONE,
	HAL_GPIO_PULL_UP,
	HAL_GPIO_PULL_DOWN
} hal_gpio_pull_resistor_t;

typedef enum {
    HAL_GPIO_TRIGGER_NONE          = 0U,
    HAL_GPIO_TRIGGER_RISING_EDGE   = 9U,
    HAL_GPIO_TRIGGER_FALLING_EDGE  = 10U,
    HAL_GPIO_TRIGGER_EITHER_EDGE   = 11U
} hal_gpio_interrupt_trigger_t;

typedef struct {
	PORT_Type *port_base;
	GPIO_Type *gpio_base;
	uint32_t pin_num;
	uint32_t pcc_index;
	IRQn_Type irq_num;
} pin_map_t;

extern const pin_map_t pin_map[];

void HAL_GPIO_EnablePortClock(uint32_t virtual_pin);
void HAL_GPIO_SetAsGpio(uint32_t virtual_pin);
void HAL_GPIO_SetDirection(uint32_t virtual_pin, hal_gpio_direction_t dir);
void HAL_GPIO_WritePin(uint32_t virtual_pin, uint8_t value);
void HAL_GPIO_TogglePin(uint32_t virtual_pin);
uint8_t HAL_GPIO_ReadPin(uint32_t virtual_pin);

void HAL_GPIO_SetOutputMode(uint32_t virtual_pin, hal_gpio_output_mode_t mode);
void HAL_GPIO_SetPullResistor(uint32_t virtual_pin, hal_gpio_pull_resistor_t resistor);
void HAL_GPIO_SetEventTrigger(uint32_t virtual_pin, hal_gpio_interrupt_trigger_t trigger);
uint8_t HAL_GPIO_IsInterruptFlagSet(uint32_t virtual_pin);
void HAL_GPIO_ClearInterruptFlag(uint32_t virtual_pin);




#endif // HAL_GPIO_H
