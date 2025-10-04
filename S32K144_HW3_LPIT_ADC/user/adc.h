/**
 * @file adc.h
 * @brief Cung cấp thư viện cấu hình và sử dụng adc cơ bản.
 * @details Thư viện cấu hình adc sử dụng clock SPLL, sử dụng 8-bit phân giải, có hỗ trợ hiệu chỉnh thống số
 * trước khi hoạt động và kích hoạt chu tình lấy mẫu bằng phần mềm.
 *
 * @author Nguyen Vuong Trung Nam
 * @date Oct 1, 2025
 */

#ifndef ADC_H_
#define ADC_H_

#include "S32K144.h"


#define ADC_SUCCESS     0
#define ADC_INIT_FAIL   1

/**
 * @brief Khởi tạo, hiệu chuẩn và cấu hình ADC0 cho chế độ 8-bit.
 * @note  Hàm này nên được gọi một lần duy nhất khi khởi động hệ thống.
 * @return ADC_SUCCESS nếu thành công, ADC_INIT_FAIL nếu hiệu chuẩn thất bại.
 */
uint8_t ADC_Init(void);

/**
 * @brief Đọc giá trị ADC từ một kênh được chỉ định.
 * @param  channel Số của kênh cần đọc (ví dụ: 12 cho ADC0_SE12).
 * @return Giá trị số 10-bit (0-1023) của kênh.
 */
uint16_t ADC_Read_Channel(uint8_t channel);

#endif /* ADC_H_ */
