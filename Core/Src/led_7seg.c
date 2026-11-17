#include "led_7seg.h"
#include "software_timer.h"

static int led_buffer[4] = {0, 0, 0, 0};
static int index_led = 0;

static const uint8_t seg_code[10] = {
    0x40, // 0
    0x79, // 1
    0x24, // 2
    0x30, // 3
    0x19, // 4
    0x12, // 5
    0x02, // 6
    0x78, // 7
    0x00, // 8
    0x10  // 9
};

void display7SEG(int num) {
  if (num < 0 || num > 9) {
    return;
  }
  uint8_t code = seg_code[num];
  HAL_GPIO_WritePin(SEG_PORT, SEG0_PIN,
                    (code & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SEG_PORT, SEG1_PIN,
                    (code & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SEG_PORT, SEG2_PIN,
                    (code & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SEG_PORT, SEG3_PIN,
                    (code & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SEG_PORT, SEG4_PIN,
                    (code & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SEG_PORT, SEG5_PIN,
                    (code & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(SEG_PORT, SEG6_PIN,
                    (code & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void update7SEG(int index) {
  HAL_GPIO_WritePin(SEG_PORT, EN0_PIN | EN1_PIN | EN2_PIN | EN3_PIN,
                    GPIO_PIN_SET);
  display7SEG(led_buffer[index]);
  switch (index) {
  case 0:
    HAL_GPIO_WritePin(SEG_PORT, EN0_PIN, GPIO_PIN_RESET);
    break;
  case 1:
    HAL_GPIO_WritePin(SEG_PORT, EN1_PIN, GPIO_PIN_RESET);
    break;
  case 2:
    HAL_GPIO_WritePin(SEG_PORT, EN2_PIN, GPIO_PIN_RESET);
    break;
  case 3:
    HAL_GPIO_WritePin(SEG_PORT, EN3_PIN, GPIO_PIN_RESET);
    break;
  default:
    break;
  }
}

void updateClockBuffer(int time1, int time2) {
  led_buffer[0] = (time1 / 10) % 10;
  led_buffer[1] = time1 % 10;
  led_buffer[2] = (time2 / 10) % 10;
  led_buffer[3] = time2 % 10;
}

void led_7seg_run(void) {
  if (timers[3].flag == 1) {
    update7SEG(index_led);
    index_led = (index_led + 1) % 4;
    setTimer(3, time_7SEG);
  }
}
