#include "button.h"
#include "software_timer.h"
#include "main.h"   

// Chu kỳ ngắt của TIM2 (ms). Khi đổi chu kỳ ngắt ở Exercise 4 (1ms, 10ms, 100ms),
// chỉ cần cập nhật đúng hằng số này (hoặc #include "software_timer.h" để dùng chung).
#define TIMER_CYCLE         10
#define TIME_OUT_1S         (1000 / TIMER_CYCLE)
#define TIME_OUT_AUTO_INC   (500 / TIMER_CYCLE)

// Ánh xạ chân GPIO tương ứng với 3 nút nhấn đặt tên trong STM32CubeMX (.ioc)
static GPIO_TypeDef* button_port[NO_OF_BUTTONS] = {
    BTN0_GPIO_Port,
    BTN1_GPIO_Port,
    BTN2_GPIO_Port
};

static uint16_t button_pin[NO_OF_BUTTONS] = {
    BTN0_Pin,
    BTN1_Pin,
    BTN2_Pin
};

// Thanh ghi dịch chống dội 3 mẫu và trạng thái ổn định trước đó
static GPIO_PinState KeyReg0[NO_OF_BUTTONS] = {BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED};
static GPIO_PinState KeyReg1[NO_OF_BUTTONS] = {BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED};
static GPIO_PinState KeyReg2[NO_OF_BUTTONS] = {BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED};
static GPIO_PinState KeyReg3[NO_OF_BUTTONS] = {BUTTON_RELEASED, BUTTON_RELEASED, BUTTON_RELEASED};

static uint16_t timeOutForKeyPress[NO_OF_BUTTONS] = {TIME_OUT_1S, TIME_OUT_1S, TIME_OUT_1S};
static uint8_t button_flag[NO_OF_BUTTONS] = {0, 0, 0};
static uint8_t button_long_flag[NO_OF_BUTTONS] = {0, 0, 0};

uint8_t isButtonPressed(uint8_t index) {
    if (index >= NO_OF_BUTTONS) return 0;
    if (button_flag[index] == 1) {
        button_flag[index] = 0; // Xóa cờ ngay khi đọc để tránh kích hoạt lặp lại
        return 1;
    }
    return 0;
}

uint8_t isButtonLongPressed(uint8_t index) {
    if (index >= NO_OF_BUTTONS) return 0;
    if (button_long_flag[index] == 1) {
        button_long_flag[index] = 0;
        return 1;
    }
    return 0;
}

void button_reading(void) {
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        KeyReg2[i] = KeyReg1[i];
        KeyReg1[i] = KeyReg0[i];
        KeyReg0[i] = HAL_GPIO_ReadPin(button_port[i], button_pin[i]);

        // Chỉ xử lý khi tín hiệu ổn định qua 3 lần lấy mẫu liên tiếp
        if ((KeyReg1[i] == KeyReg0[i]) && (KeyReg1[i] == KeyReg2[i])) {
            if (KeyReg2[i] != KeyReg3[i]) {
                // Phát hiện thay đổi trạng thái (sườn nhấn hoặc sườn nhả)
                KeyReg3[i] = KeyReg2[i];

                if (KeyReg3[i] == BUTTON_PRESSED) {
                    button_flag[i] = 1;
                    timeOutForKeyPress[i] = TIME_OUT_1S;
                }
            } else {
                // Giữ nguyên trạng thái: đếm thời gian nếu nút đang được giữ
                if (KeyReg3[i] == BUTTON_PRESSED) {
                    if (timeOutForKeyPress[i] > 0) {
                        timeOutForKeyPress[i]--;
                    }
                    if (timeOutForKeyPress[i] == 0) {
                        button_long_flag[i] = 1;
                        timeOutForKeyPress[i] = TIME_OUT_AUTO_INC; // Tự động lặp lại mỗi 0.5s
                    }
                }
            }
        }
    }
}