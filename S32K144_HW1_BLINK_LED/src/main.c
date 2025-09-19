/**
 * @file    main.c
 * @brief   Application entry point.
 * @details Chương trình ví dụ cơ bản cho S32K144, thực hiện nhấp nháy 3 đèn LED
 * trên Port D (PTD0, PTD15, PTD16) với chu kỳ delay.
 */

#include "S32K144.h"
#include <stdio.h>

/* =================================================================================================================
 * Compiler-specific Macros
 * =================================================================================================================*/
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

/* =================================================================================================================
 * Defines and Macros
 * =================================================================================================================*/
#define CPU_CLOCK_FREQ      48000000UL
#define CYCLES_PER_MS       (CPU_CLOCK_FREQ / 1000UL)


#define GPIO_PIN_INPUT      0
#define GPIO_PIN_OUTPUT     1

#define RED_LED_PIN         15U  // Chân PTD15 điều khiển LED màu đỏ
#define GREEN_LED_PIN       16U  // Chân PTD16 điều khiển LED màu xanh lá
#define BLUE_LED_PIN        0U   // Chân PTD0 điều khiển LED màu xanh dương

/* =================================================================================================================
 * Function Prototypes
 * =================================================================================================================*/

// --- Hàm hệ thống ---
void System_DelayMs(uint32_t delay_in_ms);

// --- Các hàm liên quan đến GPIO ---
void GPIO_EnablePortClock(uint8_t port_index);
void GPIO_InitPin(PORT_Type* port, GPIO_Type* gpio_port, uint8_t pin_number, uint8_t direction);
void GPIO_TogglePin(GPIO_Type* gpio_port, uint8_t pin_number);
void GPIO_SetPin(GPIO_Type* gpio_port, uint8_t pin_number);
void GPIO_ClearPin(GPIO_Type* gpio_port, uint8_t pin_number);

// --- Hàm của ứng dụng ---
void App_Init_LEDs(void);


/* =================================================================================================================
 * Main Function
 * =================================================================================================================*/
int main(void) {
    // Khởi tạo các chân GPIO được sử dụng làm đèn LED
    App_Init_LEDs();

    // Vòng lặp vô tận của chương trình
    while(1)
    {
    	// Đảo trạng thái LED đỏ (PTD0) và chờ 3 giây
		GPIO_SetPin(IP_PTD, RED_LED_PIN);
		System_DelayMs(3000);
		GPIO_ClearPin(IP_PTD, RED_LED_PIN);

		// Đảo trạng thái LED xanh lá (PTD15) và chờ 3 giây
		GPIO_SetPin(IP_PTD, GREEN_LED_PIN);
		System_DelayMs(3000);
		GPIO_ClearPin(IP_PTD, GREEN_LED_PIN);

        // Đảo trạng thái LED xanh dương (PTD16) và chờ 3 giây
        GPIO_SetPin(IP_PTD, BLUE_LED_PIN);
        System_DelayMs(3000);
        GPIO_ClearPin(IP_PTD, BLUE_LED_PIN);
    }

    __NO_RETURN
    return 0;
}

/* =================================================================================================================
 * Application Functions
 * =================================================================================================================*/
/**
 * @brief Khởi tạo các chân GPIO dùng để điều khiển LED.
 */
void App_Init_LEDs(void)
{
    // Bật clock cho Port D để có thể sử dụng các chân trên port này
    GPIO_EnablePortClock(PCC_PORTD_INDEX);

    // Cấu hình các chân LED là Output
    GPIO_InitPin(IP_PORTD, IP_PTD, RED_LED_PIN,   GPIO_PIN_OUTPUT);
    GPIO_InitPin(IP_PORTD, IP_PTD, GREEN_LED_PIN, GPIO_PIN_OUTPUT);
    GPIO_InitPin(IP_PORTD, IP_PTD, BLUE_LED_PIN,  GPIO_PIN_OUTPUT);

    GPIO_ClearPin(IP_PTD, RED_LED_PIN);
    GPIO_ClearPin(IP_PTD, GREEN_LED_PIN);
    GPIO_ClearPin(IP_PTD, BLUE_LED_PIN);
}

/* =================================================================================================================
 * System Functions
 * =================================================================================================================*/
/**
 * @brief Tạo một khoảng trễ theo mili giây (delay blocking).
 * @note  Đây là hàm delay bằng phần mềm, độ chính xác không cao và sẽ bị ảnh hưởng
 * bởi các ngắt (interrupts). `volatile` được dùng để ngăn trình biên dịch
 * tối ưu hóa (xóa bỏ) vòng lặp for.
 * @param[in] delay_in_ms Thời gian cần trễ tính bằng mili giây.
 */
void System_DelayMs(uint32_t delay_in_ms)
{
    volatile uint32_t loop_count;
    while(delay_in_ms--)
    {
        for(loop_count = 0; loop_count < CYCLES_PER_MS; loop_count++)
        {
            __asm("nop");
        }
    }
}

/* =================================================================================================================
 * GPIO Driver Functions
 * =================================================================================================================*/
/**
 * @brief Bật một chân GPIO lên mức cao (HIGH).
 * @details Ghi vào thanh ghi PSOR (Port Set Output Register) để set bit tương ứng.
 * Thao tác này là atomic, an toàn khi dùng trong ngắt.
 * @param[in] gpio_port Con trỏ tới base address của GPIO port (VD: PTA, PTB,...).
 * @param[in] pin_number Số thứ tự của chân cần tác động (0-31).
 */
void GPIO_SetPin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PSOR = (1UL << pin_number);
}

/**
 * @brief Tắt một chân GPIO về mức thấp (LOW).
 * @details Ghi vào thanh ghi PCOR (Port Clear Output Register) để xóa bit tương ứng.
 * Thao tác này là atomic, an toàn khi dùng trong ngắt.
 * @param[in] gpio_port Con trỏ tới base address của GPIO port (VD: PTA, PTB,...).
 * @param[in] pin_number Số thứ tự của chân cần tác động (0-31).
 */
void GPIO_ClearPin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PCOR = (1UL << pin_number);
}

/**
 * @brief Đảo trạng thái của một chân GPIO (HIGH -> LOW, LOW -> HIGH).
 * @details Ghi vào thanh ghi PTOR (Port Toggle Output Register).
 * @param[in] gpio_port Con trỏ tới base address của GPIO port (VD: PTA, PTB,...).
 * @param[in] pin_number Số thứ tự của chân cần tác động (0-31).
 */
void GPIO_TogglePin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PTOR = (1UL << pin_number);
}

/**
 * @brief Cấu hình chức năng cơ bản cho một chân (Input/Output).
 * @details
 * 1. Cấu hình MUX của chân thành GPIO (ALT1).
 * 2. Thiết lập hướng của chân (Input hoặc Output) thông qua thanh ghi PDDR.
 * @param[in] port Con trỏ tới base address của PORT (VD: PORTA, PORTB,...).
 * @param[in] gpio_port Con trỏ tới base address của GPIO port (VD: PTA, PTB,...).
 * @param[in] pin_number Số thứ tự của chân cần cấu hình (0-31).
 * @param[in] direction Hướng của chân (`GPIO_PIN_INPUT` hoặc `GPIO_PIN_OUTPUT`).
 */
void GPIO_InitPin(PORT_Type* port, GPIO_Type* gpio_port, uint8_t pin_number, uint8_t direction)
{
    // 1. Cấu hình MUX: Set chân thành chức năng GPIO (Alternative 1)
    port->PCR[pin_number] = PORT_PCR_MUX(1);

    // 2. Cấu hình hướng (Direction)
    if (direction == GPIO_PIN_OUTPUT) {
        // Set bit tương ứng trong thanh ghi PDDR (Port Data Direction Register) để cấu hình là Output
        gpio_port->PDDR |= (1UL << pin_number);
    } else {
        // Xóa bit tương ứng trong thanh ghi PDDR để cấu hình là Input
        gpio_port->PDDR &= ~(1UL << pin_number);
    }
}

/**
 * @brief Bật clock cho một GPIO Port.
 * @details Cần phải bật clock cho ngoại vi trước khi sử dụng. Thao tác này được
 * thực hiện qua thanh ghi PCC (Peripheral Clock Controller).
 * @param[in] port_index Index của port trong mảng PCCn (VD: PCC_PORTA_INDEX, PCC_PORTB_INDEX,...).
 */
void GPIO_EnablePortClock(uint8_t port_index)
{
    // Set bit CGC (Clock Gate Control) trong thanh ghi PCC tương ứng của port
    IP_PCC->PCCn[port_index] |= PCC_PCCn_CGC_MASK;
}
