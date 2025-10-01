/**
 * @file software_timer.c
 * @brief File triển khai thư viện software timer.
 * @details File này chứa mã nguồn để khởi tạo hardware timer (LPIT) và
 * quản lý các software timer con.
 *
 * @author ADMIN
 * @date Sep 30, 2025
 */

#include "software_timer.h"

// Nên đổi tên SOFT_TIMER_NUM thành MAX_SOFTWARE_TIMERS để đồng bộ với file .h
#define MAX_SOFTWARE_TIMERS   10

/**
 * @brief Mảng lưu giá trị đếm ngược cho mỗi software timer.
 * @note Khai báo là 'volatile' vì biến này được thay đổi trong một ngắt (ISR)
 * và được truy cập ở vòng lặp chính.
 */
volatile uint32_t timer_counter[MAX_SOFTWARE_TIMERS];

/**
 * @brief Mảng cờ báo hiệu khi một software timer đã hết hạn.
 * @note Khai báo là 'volatile' vì lý do tương tự như timer_counter.
 */
volatile uint8_t timer_flag[MAX_SOFTWARE_TIMERS];


/**
 * @brief Hàm cập nhật cốt lõi cho tất cả software timer.
 * @details Hàm này duyệt qua tất cả các timer, giảm bộ đếm của chúng đi 1.
 * Khi một bộ đếm về 0, cờ báo tương ứng sẽ được bật lên.
 * @note Hàm này là static và chỉ nên được gọi từ bên trong ISR của hardware timer.
 * @param None
 * @return None
 */
static void TIM_TimerRun(void)
{
    for(uint8_t i = 0; i < MAX_SOFTWARE_TIMERS; i++)
    {
        if(timer_counter[i] > 0)
        {
            timer_counter[i]--;
        }
        else
        {
            timer_flag[i] = 1;
        }
    }
}

/**
 * @brief Xóa cờ ngắt phần cứng cho một kênh LPIT cụ thể.
 * @details Hardware LPIT yêu cầu ghi '1' vào bit TIFx trong thanh ghi MSR
 * để xóa cờ ngắt tương ứng.
 * @note Hàm này phải được gọi ở đầu ISR để tránh ISR bị gọi lại ngay lập tức.
 * Tên hàm 'Set' có thể gây hiểu lầm, thực chất nó đang 'Clear' cờ.
 * @param[in] channel Kênh LPIT (0-3) cần xóa cờ ngắt.
 * @return None
 */
static void TIM_ClearInterruptFlag(uint8_t channel)
{
    // Ghi 1 để xóa cờ
    switch(channel)
    {
    case 0:
        IP_LPIT0->MSR = LPIT_MSR_TIF0_MASK;
        break;
    case 1:
        IP_LPIT0->MSR = LPIT_MSR_TIF1_MASK;
        break;
    case 2:
        IP_LPIT0->MSR = LPIT_MSR_TIF2_MASK;
        break;
    case 3:
        IP_LPIT0->MSR = LPIT_MSR_TIF3_MASK;
        break;
    default:
        break;
    }
}

/**
 * @brief Khởi tạo module hardware timer LPIT.
 * @copydoc TIM_Init
 */
void TIM_Init(void)
{
    /* 1. Cấp clock cho LPIT */
    IP_PCC->PCCn[PCC_LPIT_INDEX] &= ~PCC_PCCn_CGC_MASK;
    /* chọn nguồn SPLLDIV2_CLK */
    IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_PCS(6);
    IP_PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK;

    /* 2. Reset ngoại vi bằng phần mềm */
    IP_LPIT0->MCR |= LPIT_MCR_SW_RST_MASK;
    IP_LPIT0->MCR &= ~LPIT_MCR_SW_RST_MASK;

    /* 3. Bật module LPIT */
    IP_LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;

    /* 4. Cấu hình kênh 0 của LPIT */
    IP_LPIT0->TMR[0].TCTRL = 0;
    IP_LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_MODE(0);
    /* SPLLDIV2_CLK là 20MHz */
    IP_LPIT0->TMR[0].TVAL = 20000 - 1;
    IP_LPIT0->MIER |= LPIT_MIER_TIE0_MASK;

    /* 5. Cấu hình chế độ hoạt động khi debug/doze */
    IP_LPIT0->MCR |= LPIT_MCR_DBG_EN_MASK | LPIT_MCR_DOZE_EN_MASK;

    /* 6. Bật ngắt trong NVIC và khởi động timer */
    NVIC->ISER[LPIT0_Ch0_IRQn / 32] = (1 << (LPIT0_Ch0_IRQn % 32));
    IP_LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;


    for (uint8_t i = 0;  i < MAX_SOFTWARE_TIMERS; i++)
    {
        timer_counter[i] = 0;
        timer_flag[i] = 0;
    }
}

/**
 * @brief Kiểm tra xem một software timer cụ thể đã hết hạn hay chưa.
 * @copydoc TIM_IsFlag
 */
uint8_t TIM_IsFlag(uint8_t index)
{
    if(index >= MAX_SOFTWARE_TIMERS)
    {
        return 0xFF;
    }
    else
    {
        uint8_t flag_status = timer_flag[index];

        timer_flag[index] = 0;
        return flag_status;
    }
}

/**
 * @brief Đặt thời gian và khởi động một software timer.
 * @copydoc TIM_SetTime
 */
uint8_t TIM_SetTime(uint8_t index, uint32_t duration_ms)
{
    if(index >= MAX_SOFTWARE_TIMERS)
    {
        return 0;
    }

    timer_counter[index] = duration_ms;
    timer_flag[index] = 0;

    return 1;
}

/**
 * @brief Trình xử lý ngắt cho kênh 0 của LPIT.
 * @details Hàm này được hardware tự động gọi khi timer kênh 0 hết hạn.
 * Nó có nhiệm vụ xóa cờ ngắt phần cứng và gọi hàm cập nhật
 * cho các software timer.
 * @param None
 * @return None
 */
void LPIT0_Ch0_IRQHandler(void)
{
    TIM_ClearInterruptFlag(0);

    TIM_TimerRun();
}
