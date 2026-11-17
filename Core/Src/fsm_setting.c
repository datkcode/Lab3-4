#include "fsm_setting.h"
#include "button.h"
#include "global.h"
#include "led_7seg.h"
#include "software_timer.h"
#include "traffic_light.h"

void fsm_setting_run(void) {
  switch (status) {
  case SET_RED:
    // Logic for SET_RED
    updateClockBuffer(temp_time, 2);
    if (timers[2].flag == 1) {
      traffic_red_toggle();
      setTimer(2, time_BLINK);
    }
    if (isButtonPressed(1) == 1 || isButtonLongPressed(1) == 1) {
      temp_time++;
      if (temp_time > 99) {
        temp_time = 1;
      }
    }
    if (isButtonPressed(2) == 1) {
      time_red = temp_time * 1000;
      if (time_red > time_amber) {
        time_green = time_red - time_amber;
      }
    }
    // Set enviroment for next state
    if (isButtonPressed(0) == 1) {
      isButtonPressed(1);
      isButtonLongPressed(1);
      isButtonPressed(2);
      temp_time = time_amber / 1000;
      traffic_amber_blink_init();
      status = SET_AMBER;
      setTimer(2, time_BLINK);
    }
    break;
  case SET_AMBER:
    // Logic for SET_AMBER
    updateClockBuffer(temp_time, 3);
    if (timers[2].flag == 1) {
      traffic_amber_toggle();
      setTimer(2, time_BLINK);
    }
    if (isButtonPressed(1) == 1 || isButtonLongPressed(1) == 1) {
      temp_time++;
      if (temp_time > 99) {
        temp_time = 1;
      }
    }
    if (isButtonPressed(2) == 1) {
      time_amber = temp_time * 1000;
      time_red = time_green + time_amber;
    }
    // Set enviroment for next state
    if (isButtonPressed(0) == 1) {
      isButtonPressed(1);
      isButtonLongPressed(1);
      isButtonPressed(2);
      temp_time = time_green / 1000;
      traffic_green_blink_init();
      status = SET_GREEN;
      setTimer(2, time_BLINK);
    }
    break;
  case SET_GREEN:
    // Logic for SET_GREEN
    updateClockBuffer(temp_time, 4);
    if (timers[2].flag == 1) {
      traffic_green_toggle();
      setTimer(2, time_BLINK);
    }
    if (isButtonPressed(1) == 1 || isButtonLongPressed(1) == 1) {
      temp_time++;
      if (temp_time > 99) {
        temp_time = 1;
      }
    }
    if (isButtonPressed(2) == 1) {
      time_green = temp_time * 1000;
      time_red = time_green + time_amber;
    }
    // Set enviroment for next state
    if (isButtonPressed(0) == 1) {
      isButtonPressed(1);
      isButtonLongPressed(1);
      isButtonPressed(2);
      timers[2].flag = 0;
      status = RED1_GREEN2;
      setTimer(0, time_green);
    }
    break;
  default:
    break;
  }
}
