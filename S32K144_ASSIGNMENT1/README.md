# Driver GPIO S32K144 theo chuẩn CMSIS
Dự án này hiện thực một bộ driver GPIO cho vi điều khiển NXP S32K144, được xây dựng theo kiến trúc phần mềm 3 lớp rõ ràng. Mục tiêu của kiến trúc này là tách biệt logic ứng dụng khỏi các chi tiết phần cứng, tuân thủ giao diện (interface) của chuẩn CMSIS-Driver, đồng thời đảm bảo tính linh hoạt và khả năng chuyển đổi (portability) cao.
## 1. Kiến trúc phần mềm
Dự án được cấu trúc theo mô hình 3 lớp kinh điển, trong đó luồng điều khiển chỉ đi theo một chiều từ trên xuống dưới.
``` Note
+----------------------------------------+
|    Tầng Ứng Dụng (Application Layer)   |  -> Ví dụ: main.c
|      (Logic chính của chương trình)      |
+----------------------------------------+
                  |
                  v (Gọi hàm qua API chuẩn của CMSIS)
+----------------------------------------+
|      Tầng CMSIS Driver (CMSIS Layer)     |  -> Ví dụ: Driver_GPIO.c (Hộp đen)
|     (Giao diện chuẩn, không sửa đổi)      |
+----------------------------------------+
                  |
                  v (Gọi hàm qua API của HAL)
+----------------------------------------+
| Tầng HAL (Hardware Abstraction Layer)  |  -> Ví dụ: hal_gpio.c
|    (Truy cập thanh ghi, xử lý ngắt)     |
+----------------------------------------+
                  |
                  v (Thao tác trực tiếp)
+----------------------------------------+
|           Phần Cứng (Hardware)         |  -> Thanh ghi S32K144
+----------------------------------------+
```
### Tầng ứng dụng
- Vai trò: Chứa logic chính của ứng dụng (ví dụ: đọc nút nhấn, điều khiển đèn LED).
- Đặc điểm: Tầng này chỉ được phép tương tác với Tầng CMSIS Driver thông qua các API chuẩn đã được định nghĩa (ví dụ: Driver_GPIO0.Setup(...)). Nó hoàn toàn không biết gì về thanh ghi hay chi tiết phần cứng.
### Tầng CMSIS Driver
- Vai trò: Cung cấp một bộ giao diện (API) ổn định và tiêu chuẩn hóa cho các tầng trên.
- Đặc điểm: Trong dự án này, tầng CMSIS được xem như một "hộp đen" không thể sửa đổi. Nhiệm vụ của nó là nhận lệnh từ Tầng Ứng dụng và "dịch" chúng thành các lệnh gọi hàm tương ứng xuống Tầng HAL. Chức năng xử lý ngắt bằng callback của lớp này bị bỏ qua.
### Tầng HAL
- Vai trò: Là lớp duy nhất được phép tương tác trực tiếp với phần cứng (đọc/ghi thanh ghi).
- Đặc điểm: Lớp này chứa tất cả các đoạn mã đặc thù cho vi điều khiển S32K144. Nó che giấu sự phức tạp của phần cứng và cung cấp một bộ API đơn giản cho Tầng CMSIS sử dụng. Toàn bộ cơ chế xử lý ngắt (đăng ký callback, IRQHandler) cũng được quản lý tại đây.
## 2. Quy tắc giữa các tầng
Để đảm bảo kiến trúc được duy trì, các quy tắc sau phải được tuân thủ nghiêm ngặt:
- **a. Luồng gọi hàm từ trên xuống (Top-Down Call Flow):**
  - Tầng Ứng dụng có thể gọi Tầng CMSIS.
  - Tầng CMSIS có thể gọi Tầng HAL.
  - Tuyệt đối không có trường hợp ngược lại: Tầng HAL không bao giờ được gọi Tầng CMSIS, và Tầng CMSIS không bao giờ được gọi Tầng Ứng dụng.
- **b. Giao tiếp qua API công khai:**
  - Các tầng chỉ được giao tiếp với nhau thông qua các hàm đã được định nghĩa trong file header (.h) của tầng bên dưới.
  - Nghiêm cấm việc truy cập vào các biến static hoặc gọi các hàm static của một tầng khác.
- **c. Quy tắc đặc biệt cho xử lý ngắt:**
  - Vì Tầng CMSIS là không thể sửa đổi, cơ chế callback của nó không được sử dụng.
  - Thay vào đó, Tầng Ứng dụng sẽ đăng ký callback trực tiếp với Tầng HAL thông qua hàm HAL_GPIO_RegisterCallback().
  - Khi ngắt phần cứng xảy ra, IRQHandler được định nghĩa trong Tầng HAL sẽ được thực thi và gọi đến callback đã được Tầng Ứng dụng đăng ký.
## 3. Đặc tả các file
`my_nvic.h`
- Mục đích: Cung cấp một lớp truy cập tối thiểu cho các thanh ghi của NVIC (Nested Vectored Interrupt Controller).
- Đặc điểm: File này được tạo ra để giải quyết tình huống khi môi trường phát triển không cung cấp sẵn file header chuẩn của CMSIS-Core. Nó định nghĩa struct NVIC_Type dựa trên bản đồ bộ nhớ (memory map) và tạo một con trỏ NVIC để truy cập các thanh ghi một cách thuận tiện.
