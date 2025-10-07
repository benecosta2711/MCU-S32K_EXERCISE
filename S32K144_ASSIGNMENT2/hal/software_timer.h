/**
 * @file software_timer.h
 * @brief Cung cấp thư viện software timer dựa trên tick của hardware timer.
 * @details Thư viện này cho phép tạo nhiều bộ đếm thời gian phần mềm (software timer)
 * dựa trên một bộ đếm thời gian phần cứng (hardware timer) duy nhất (ví dụ: LPIT).
 * Nó hoạt động dựa trên một ngắt định kỳ (tick) để quản lý các bộ đếm.
 *
 * @author Nguyen Vuong Trung Nam
 * @date Sep 30, 2025
 */

#ifndef SOFTWARE_TIMER_H_
#define SOFTWARE_TIMER_H_

#include "stdint.h"
#include "my_nvic.h"
#include "S32K144.h"

#define MAX_SOFTWARE_TIMERS   10

/**
 * @brief Khởi tạo module hardware timer
 * @note Hàm này cấu hình hardware timer để tạo ra một ngắt định kỳ (tick),
 * thường là mỗi 1ms. Nó phải được gọi một lần duy nhất trước khi sử dụng
 * bất kỳ hàm timer nào khác.
 * @param None
 * @return None
 */
void TIM_Init(void);

/**
 * @brief Kiểm tra xem một software timer cụ thể đã hết hạn hay chưa.
 * @note Hàm này nên được gọi lặp đi lặp lại trong vòng lặp chính (polling).
 * Khi hàm trả về 1, cờ báo bên trong sẽ tự động được xóa.
 * @param[in] index Chỉ số của software timer cần kiểm tra (từ 0 đến MAX_SOFTWARE_TIMERS - 1).
 * @return uint8_t
 * - 1: Nếu timer đã hết hạn.
 * - 0: Nếu timer vẫn đang chạy.
 */
uint8_t TIM_IsFlag(uint8_t index);

/**
 * @brief Đặt thời gian và khởi động một software timer.
 * @note Hàm này thiết lập giá trị đếm ngược cho timer được chỉ định. Timer sẽ
 * bắt đầu chạy ngay lập tức. Dùng hàm TIM_IsFlag() để kiểm tra khi nào nó hết hạn.
 * @param[in] index Chỉ số của software timer cần đặt (từ 0 đến MAX_SOFTWARE_TIMERS - 1).
 * @param[in] duration_ms Thời gian đếm mong muốn, tính bằng mili-giây (ms).
 * @return uint8_t
 * - 0: Nếu cài đặt thành công.
 * - 1: Nếu chỉ số (index) không hợp lệ.
 */
uint8_t TIM_SetTime(uint8_t index, uint32_t duration_ms);


#endif /* SOFTWARE_TIMER_H_ */
