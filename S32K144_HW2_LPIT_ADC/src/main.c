/**
 * @file      main.c
 * @author    Nguyen Vuong Trung Nam
 * @brief     Điểm khởi đầu của ứng dụng (Application entry point).
 * @details   Chương trình ví dụ cơ bản cho S32K144, thực hiện nhấp nháy đèn LED
 * xanh mỗi 1 giây sử dụng thư viện software timer và LPIT.
 */

#include "S32K144.h"
#include "system_S32K144.h"
#include "software_timer.h"

/*==================================================================================================
* MACROS AND DEFINES
==================================================================================================*/

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

/**
 * @brief Định nghĩa cho hướng của chân GPIO.
 */
#define GPIO_PIN_INPUT      0
#define GPIO_PIN_OUTPUT     1

/**
 * @brief Định nghĩa các thông số cho đèn LED xanh.
 */
#define BLUE_LED_PIN        0U
#define BLUE_LED_PORT       IP_PORTD
#define BLUE_LED_GPIO       IP_PTD

/*==================================================================================================
* FUNCTION PROTOTYPES
==================================================================================================*/
/**
 * @brief Khởi tạo và chuyển đổi clock hệ thống sang SPLL.
 */
void Clock_Init_System_SPLL(void);

/**
 * @brief Cấp clock cho một Port GPIO.
 */
void GPIO_EnablePortClock(uint8_t port_index);

/**
 * @brief Cấu hình một chân GPIO là Input hoặc Output.
 */
void GPIO_InitPin(PORT_Type* port, GPIO_Type* gpio_port, uint8_t pin_number, uint8_t direction);

/**
 * @brief Đảo trạng thái một chân GPIO.
 */
void GPIO_TogglePin(GPIO_Type* gpio_port, uint8_t pin_number);

/**
 * @brief Bật một chân GPIO lên mức cao.
 */
void GPIO_SetPin(GPIO_Type* gpio_port, uint8_t pin_number);

/**
 * @brief Tắt một chân GPIO về mức thấp.
 */
void GPIO_ClearPin(GPIO_Type* gpio_port, uint8_t pin_number);


int main(void)
{
    Clock_Init_System_SPLL();
    /* Kiểm tra giá trị clock được cập nhật */
    SystemCoreClockUpdate();

    GPIO_EnablePortClock(PCC_PORTD_INDEX);
    GPIO_InitPin(BLUE_LED_PORT, BLUE_LED_GPIO, BLUE_LED_PIN,  GPIO_PIN_OUTPUT);
    GPIO_ClearPin(BLUE_LED_GPIO, BLUE_LED_PIN);

    TIM_Init();
    TIM_SetTime(0, 1000);

    while(1)
    {
        if(TIM_IsFlag(0))
        {
            GPIO_TogglePin(BLUE_LED_GPIO, BLUE_LED_PIN);
            TIM_SetTime(0, 1000);
        }
    }

    __NO_RETURN
    return 0;
}

/*==================================================================================================
* GPIO DRIVER FUNCTIONS
==================================================================================================*/
/**
 * @copydoc GPIO_SetPin
 */
void GPIO_SetPin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PSOR = (1UL << pin_number);
}

/**
 * @copydoc GPIO_ClearPin
 */
void GPIO_ClearPin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PCOR = (1UL << pin_number);
}

/**
 * @copydoc GPIO_TogglePin
 */
void GPIO_TogglePin(GPIO_Type* gpio_port, uint8_t pin_number)
{
    gpio_port->PTOR = (1UL << pin_number);
}

/**
 * @copydoc GPIO_InitPin
 */
void GPIO_InitPin(PORT_Type* port, GPIO_Type* gpio_port, uint8_t pin_number, uint8_t direction)
{
    /**
     * Cấu hình MUX: Set chân thành chức năng GPIO (Alternative 1)
     */
    port->PCR[pin_number] = PORT_PCR_MUX(1);

    /**
     * Cấu hình hướng (Direction)
     */
    if (direction == GPIO_PIN_OUTPUT) {
        /**
         * Set bit tương ứng trong thanh ghi PDDR (Port Data Direction Register)
         * để cấu hình là Output.
         */
        gpio_port->PDDR |= (1UL << pin_number);
    } else {
        /**
         * Xóa bit tương ứng trong thanh ghi PDDR để cấu hình là Input.
         */
        gpio_port->PDDR &= ~(1UL << pin_number);
    }
}

/**
 * @copydoc GPIO_EnablePortClock
 */
void GPIO_EnablePortClock(uint8_t port_index)
{
    /**
     * Set bit CGC (Clock Gate Control) trong thanh ghi PCC tương ứng của port.
     */
    IP_PCC->PCCn[port_index] |= PCC_PCCn_CGC_MASK;
}

/*==================================================================================================
* SYSTEM CLOCK FUNCTIONS
==================================================================================================*/
/**
 * @brief Cấu hình clock hệ thống để chạy từ SPLL với thạch anh 8MHz.
 * @details Hàm này tuân thủ các hướng dẫn an toàn từ nhà sản xuất,
 * bao gồm việc bật các bộ giám sát clock.
 *
 * @note Các tần số clock cuối cùng:
 * - Core Clock: 80 MHz
 * - Bus Clock: 40 MHz
 * - Slow (Flash) Clock: 20 MHz
 * - SPLLDIV2 Output (cho ngoại vi): 20 MHz
 */
void Clock_Init_System_SPLL(void)
{
    /**
     * BƯỚC 1: Cấu hình và bật nguồn clock cho SPLL (SOSC - Thạch anh 8MHz)
     */
    IP_SCG->SOSCDIV = SCG_SOSCDIV_SOSCDIV1(1) | SCG_SOSCDIV_SOSCDIV2(1);
    IP_SCG->SOSCCFG = SCG_SOSCCFG_RANGE(3) | SCG_SOSCCFG_EREFS_MASK;
    IP_SCG->SOSCCSR = SCG_SOSCCSR_SOSCEN_MASK;
    while ((IP_SCG->SOSCCSR & SCG_SOSCCSR_SOSCVLD_MASK) == 0);
    IP_SCG->SOSCCSR |= SCG_SOSCCSR_SOSCCMRE_MASK;

    /**
     * BƯỚC 2: Cấu hình bộ nhân tần SPLL
     */
    IP_SCG->SPLLCSR &= ~SCG_SPLLCSR_SPLLEN_MASK;
    /**
     * Nguồn SOSC=8MHz / PREDIV(/1) * MULT(x20) / 2  => VCO = 80MHz
     */
    IP_SCG->SPLLCFG = SCG_SPLLCFG_PREDIV(0) | SCG_SPLLCFG_MULT(4);
    /**
     * SPLLDIV1 (hệ thống): 80MHz / 2 = 40MHz
     * SPLLDIV2 (ngoại vi): 80MHz / 4 = 20MHz
     */
    IP_SCG->SPLLDIV = SCG_SPLLDIV_SPLLDIV1(1) | SCG_SPLLDIV_SPLLDIV2(3);
    IP_SCG->SPLLCSR |= SCG_SPLLCSR_SPLLEN_MASK;
    while ((IP_SCG->SPLLCSR & SCG_SPLLCSR_SPLLVLD_MASK) == 0);
    IP_SCG->SPLLCSR |= SCG_SPLLCSR_SPLLCMRE_MASK;

    /**
     * BƯỚC 3: Chuyển clock hệ thống sang SPLL và cấu hình các bộ chia
     */
    IP_SCG->RCCR = SCG_RCCR_SCS(6)
                 | SCG_RCCR_DIVCORE(0)
                 | SCG_RCCR_DIVBUS(1)
                 | SCG_RCCR_DIVSLOW(3);
    while (((IP_SCG->CSR & SCG_CSR_SCS_MASK) >> SCG_CSR_SCS_SHIFT) != 6);
}
