#include "Driver_GPIO.h"
#include "hal_gpio.h"

// Pin mapping
#define GPIO_MAX_PINS           (256)
#define PIN_IS_AVAILABLE(n)     ((n) < GPIO_MAX_PINS)


// Setup GPIO Interface
static int32_t GPIO_Setup (ARM_GPIO_Pin_t pin, ARM_GPIO_SignalEvent_t cb_event) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin))
  {
	  HAL_GPIO_EnablePortClock(pin);
	  HAL_GPIO_SetAsGpio(pin);

	  uint8_t register_result = HAL_GPIO_RegisterCallback(pin, cb_event);

	  if(!register_result)
	  {
		  result = ARM_GPIO_ERROR_PIN;
	  }

  }
  else
  {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;

}

// Set GPIO Direction
static int32_t GPIO_SetDirection (ARM_GPIO_Pin_t pin, ARM_GPIO_DIRECTION direction) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin))
  {
    switch (direction)
    {
      case ARM_GPIO_INPUT:
    	  HAL_GPIO_SetDirection(pin, HAL_GPIO_DIR_INPUT);

        break;
      case ARM_GPIO_OUTPUT:
    	  HAL_GPIO_SetDirection(pin, HAL_GPIO_DIR_OUTPUT);
        break;
      default:
        result = ARM_DRIVER_ERROR_PARAMETER;
        break;
    }
  }
  else
  {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
};

// Set GPIO Output Mode
static int32_t GPIO_SetOutputMode (ARM_GPIO_Pin_t pin, ARM_GPIO_OUTPUT_MODE mode) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin))
  {
    switch (mode)
    {
      case ARM_GPIO_PUSH_PULL:
        break;
      case ARM_GPIO_OPEN_DRAIN:
        break;
      default:
        result = ARM_DRIVER_ERROR_PARAMETER;
        break;
    }
  }
  else
  {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Pull Resistor
static int32_t GPIO_SetPullResistor (ARM_GPIO_Pin_t pin, ARM_GPIO_PULL_RESISTOR resistor)
{
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin))
  {
    switch (resistor)
    {
      case ARM_GPIO_PULL_NONE:
    	  HAL_GPIO_SetPullResistor(pin, HAL_GPIO_PULL_NONE);
        break;
      case ARM_GPIO_PULL_UP:
    	  HAL_GPIO_SetPullResistor(pin, HAL_GPIO_PULL_UP);
        break;
      case ARM_GPIO_PULL_DOWN:
    	  HAL_GPIO_SetPullResistor(pin, HAL_GPIO_PULL_DOWN);
        break;
      default:
        result = ARM_DRIVER_ERROR_PARAMETER;
        break;
    }
  }
  else
  {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Event Trigger
static int32_t GPIO_SetEventTrigger (ARM_GPIO_Pin_t pin, ARM_GPIO_EVENT_TRIGGER trigger) {
  int32_t result = ARM_DRIVER_OK;

  if (PIN_IS_AVAILABLE(pin)) {
    switch (trigger) {
      case ARM_GPIO_TRIGGER_NONE:
    	  HAL_GPIO_SetEventTrigger(pin, HAL_GPIO_TRIGGER_NONE);
        break;
      case ARM_GPIO_TRIGGER_RISING_EDGE:
    	  HAL_GPIO_SetEventTrigger(pin, HAL_GPIO_TRIGGER_RISING_EDGE);
        break;
      case ARM_GPIO_TRIGGER_FALLING_EDGE:
    	  HAL_GPIO_SetEventTrigger(pin, HAL_GPIO_TRIGGER_FALLING_EDGE);
        break;
      case ARM_GPIO_TRIGGER_EITHER_EDGE:
    	  HAL_GPIO_SetEventTrigger(pin, HAL_GPIO_TRIGGER_EITHER_EDGE);
        break;
      default:
        result = ARM_DRIVER_ERROR_PARAMETER;
        break;
    }
  } else {
    result = ARM_GPIO_ERROR_PIN;
  }

  return result;
}

// Set GPIO Output Level
static void GPIO_SetOutput (ARM_GPIO_Pin_t pin, uint32_t val)
{

  if (PIN_IS_AVAILABLE(pin))
  {
	  HAL_GPIO_WritePin(pin, val);
  }
}

// Get GPIO Input Level
static uint32_t GPIO_GetInput (ARM_GPIO_Pin_t pin) {
  uint32_t val = 0U;

  if (PIN_IS_AVAILABLE(pin))
  {
	  val = HAL_GPIO_ReadPin(pin);
  }
  return val;
}


// GPIO Driver access structure
ARM_DRIVER_GPIO Driver_GPIO0 = {
  GPIO_Setup,
  GPIO_SetDirection,
  GPIO_SetOutputMode,
  GPIO_SetPullResistor,
  GPIO_SetEventTrigger,
  GPIO_SetOutput,
  GPIO_GetInput
};

