#ifndef HAL_GPIO_H_
#define HAL_GPIO_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "S32K144.h"
#include "stddef.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define true 	1
#define false 	0

/**
 * @brief Định nghĩa hướng của chân GPIO (Input/Output)
 */
typedef enum
{
    HAL_GPIO_DIR_INPUT,
    HAL_GPIO_DIR_OUTPUT
} hal_gpio_direction_t;

/**
 * @brief Định nghĩa chế độ Output của chân GPIO (Push-Pull/Open-Drain)
 */
typedef enum
{
    HAL_GPIO_PUSH_PULL,
    HAL_GPIO_OPEN_DRAIN
} hal_gpio_output_mode_t;

/**
 * @brief Định nghĩa loại điện trở kéo
 */
typedef enum
{
    HAL_GPIO_PULL_NONE,
    HAL_GPIO_PULL_UP,
    HAL_GPIO_PULL_DOWN
} hal_gpio_pull_resistor_t;

/**
 * @brief Định nghĩa loại trigger cho ngắt ngoài
 */
typedef enum
{
    HAL_GPIO_TRIGGER_NONE,
    HAL_GPIO_TRIGGER_RISING_EDGE,
    HAL_GPIO_TRIGGER_FALLING_EDGE,
    HAL_GPIO_TRIGGER_EITHER_EDGE
} hal_gpio_interrupt_trigger_t;

/**
 * @brief Cấu trúc ánh xạ từ pin ảo sang thông tin phần cứng
 */
typedef struct
{
    PORT_Type *port_base;
    GPIO_Type *gpio_base;
    uint32_t pin_num;
    uint32_t pcc_index;
    IRQn_Type irq_num;
} pin_map_t;

/**
 * @brief Định nghĩa kiểu con trỏ hàm callback
 */
typedef void (*HAL_GPIO_Callback_t)(uint32_t virtual_pin, uint32_t event);

/*******************************************************************************
 * API
 ******************************************************************************/

/**
 * @brief Khởi tạo một chân GPIO (bật clock và set MUX).
 *
 * @param virtual_pin Pin ảo cần khởi tạo.
 * @return true nếu thành công, false nếu pin không hợp lệ.
 */
uint8_t HAL_GPIO_Init(uint32_t virtual_pin);

/**
 * @brief Đăng ký một hàm callback cho một pin ảo.
 *
 * @param virtual_pin Pin ảo cần đăng ký.
 * @param callback Con trỏ tới hàm callback.
 * @return true nếu đăng ký thành công, false nếu pin không hợp lệ.
 */
uint8_t HAL_GPIO_RegisterCallback(uint32_t virtual_pin, HAL_GPIO_Callback_t callback);

/**
 * @brief Cấu hình hướng (Input/Output) cho một chân GPIO.
 *
 * @param virtual_pin Pin ảo cần cấu hình.
 * @param dir Hướng của chân (HAL_GPIO_DIR_INPUT hoặc HAL_GPIO_DIR_OUTPUT).
 */
void HAL_GPIO_SetDirection(uint32_t virtual_pin, hal_gpio_direction_t dir);

/**
 * @brief Ghi một giá trị logic ra chân Output.
 *
 * @param virtual_pin Pin ảo cần ghi.
 * @param value Giá trị cần ghi (true cho HIGH, false cho LOW).
 */
void HAL_GPIO_WritePin(uint32_t virtual_pin, uint8_t value);

/**
 * @brief Đảo trạng thái của một chân Output.
 *
 * @param virtual_pin Pin ảo cần đảo trạng thái.
 */
void HAL_GPIO_TogglePin(uint32_t virtual_pin);

/**
 * @brief Đọc trạng thái logic của một chân (Input hoặc Output).
 *
 * @param virtual_pin Pin ảo cần đọc.
 * @return Trạng thái của chân (true cho HIGH, false cho LOW).
 */
uint8_t HAL_GPIO_ReadPin(uint32_t virtual_pin);

/**
 * @brief Cấu hình chế độ Output (Push-Pull/Open-Drain).
 *
 * @param virtual_pin Pin ảo cần cấu hình.
 * @param mode Chế độ Output (HAL_GPIO_PUSH_PULL hoặc HAL_GPIO_OPEN_DRAIN).
 */
void HAL_GPIO_SetOutputMode(uint32_t virtual_pin, hal_gpio_output_mode_t mode);

/**
 * @brief Cấu hình điện trở kéo cho một chân.
 *
 * @param virtual_pin Pin ảo cần cấu hình.
 * @param resistor Loại điện trở kéo.
 */
void HAL_GPIO_SetPullResistor(uint32_t virtual_pin, hal_gpio_pull_resistor_t resistor);

/**
 * @brief Cấu hình ngắt ngoài cho một chân.
 *
 * @param virtual_pin Pin ảo cần cấu hình.
 * @param trigger Loại trigger cho ngắt.
 */
void HAL_GPIO_SetEventTrigger(uint32_t virtual_pin, hal_gpio_interrupt_trigger_t trigger);

/**
 * @brief Kiểm tra cờ ngắt của một chân.
 *
 * @param virtual_pin Pin ảo cần kiểm tra.
 * @return true nếu cờ ngắt được set, false nếu không.
 */
uint8_t HAL_GPIO_IsInterruptFlagSet(uint32_t virtual_pin);

/**
 * @brief Xóa cờ ngắt của một chân.
 *
 * @param virtual_pin Pin ảo cần xóa cờ ngắt.
 */
void HAL_GPIO_ClearInterruptFlag(uint32_t virtual_pin);

#endif /* HAL_GPIO_H_ */
