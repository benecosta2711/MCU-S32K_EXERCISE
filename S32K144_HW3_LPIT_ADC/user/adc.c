/*
 * adc.c
 *
 * Created on: Oct 1, 2025
 * Author: ADMIN
 */

#include "adc.h"

/**
 * @brief  Khởi tạo, hiệu chuẩn và cấu hình ADC0.
 * @note   Hàm này thực hiện toàn bộ quá trình thiết lập cho ADC0, bao gồm
 * cấp xung nhịp, hiệu chuẩn bắt buộc và cấu hình cho chế độ hoạt động
 * bình thường (10-bit, software trigger). Hàm này chỉ nên được gọi một
 * lần khi hệ thống khởi động.
 * @warning Logic kiểm tra lỗi hiệu chuẩn trong hàm này chưa chính xác.
 * @return ADC_SUCCESS nếu thành công, ADC_INIT_FAIL nếu thất bại.
 */
uint8_t ADC_Init(void)
{
    /* BƯỚC 1: BẬT VÀ CHỌN NGUỒN CLOCK CHO ADC0 TỪ PCC */

    /* Xóa cấu hình clock cũ của ADC0 trong thanh ghi PCC */
    IP_PCC->PCCn[PCC_ADC0_INDEX] = 0;
    /* Chọn SPLLDIV2_CLK làm nguồn clock cho module ADC0 */
    IP_PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(6);
    /* Bật clock gate, cho phép xung nhịp đi vào module ADC0 */
    IP_PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;

    /* BƯỚC 2: THỰC HIỆN HIỆU CHUẨN (CALIBRATION) */

    /* Vô hiệu hóa ADC bằng cách chọn kênh không hợp lệ, đảm bảo ADC ở trạng thái nghỉ */
    IP_ADC0->SC1[0] = ADC_SC1_ADCH_MASK;

    /* Cấu hình bộ chia clock chậm (chia 8) để đảm bảo hiệu chuẩn chính xác */
    IP_ADC0->CFG1 = ADC_CFG1_ADIV(3);

    /* Bắt đầu quá trình hiệu chuẩn: bật hardware average, chọn 32 mẫu, và set cờ CAL */
    IP_ADC0->SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3) | ADC_SC3_CAL_MASK;

    /* Vòng lặp chờ cho đến khi quá trình hiệu chuẩn hoàn tất, chỉ định bởi cờ COCO */
    while (!(IP_ADC0->SC1[0] & ADC_SC1_COCO_MASK)) {}

    /* Kiểm tra xem hiệu chuẩn có thất bại không */
    if (IP_ADC0->SC3 & ADC_SC3_CAL_MASK) {
        return ADC_INIT_FAIL;
    }

    /* BƯỚC 3: CẤU HÌNH CHO HOẠT ĐỘNG BÌNH THƯỜNG */

    /* Thiết lập cấu hình hoạt động: clock nguồn, độ phân giải 10-bit và bộ chia */
    IP_ADC0->CFG1 = ADC_CFG1_ADICLK(0) |   /* Chọn Alternate Clock 1 làm clock nội bộ */
                  ADC_CFG1_MODE(1)   |   /* Cấu hình độ phân giải 10-BIT */
                  ADC_CFG1_ADIV(1);      /* Cấu hình bộ chia clock là 2 */

    /* Chọn chế độ kích hoạt chuyển đổi bằng phần mềm */
    IP_ADC0->SC2 = ADC_SC2_ADTRG(0);
    /* Xóa thanh ghi SC3 để tắt chế độ hiệu chuẩn và các chế độ khác */
    IP_ADC0->SC3 = 0;

    return ADC_SUCCESS;
}

/**
 * @brief  Thực hiện một lần chuyển đổi ADC trên một kênh chỉ định.
 * @param  channel Số thứ tự của kênh analog cần đọc giá trị.
 * @return Giá trị số 10-bit (0-1023) sau khi chuyển đổi.
 */
uint16_t ADC_Read_Channel(uint8_t channel)
{
    /* Ghi vào thanh ghi SC1 để chọn kênh và bắt đầu một lần chuyển đổi mới */
    IP_ADC0->SC1[0] = ADC_SC1_ADCH(channel);

    /* Vòng lặp chờ cho đến khi chuyển đổi hoàn tất, chỉ định bởi cờ COCO */
    while (!(IP_ADC0->SC1[0] & ADC_SC1_COCO_MASK)) {}

    /* Đọc giá trị từ thanh ghi kết quả và trả về. Thao tác này cũng tự động xóa cờ COCO. */
    return (uint16_t)IP_ADC0->R[0];
}
