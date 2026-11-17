#include "fsm_manual.h"
#include "button.h"
#include "global.h"
#include "led_7seg.h"
#include "software_timer.h"
#include "traffic_light.h"

void fsm_manual_run(void) {
  switch (status) {
  case MAN_RED1_GREEN2:
    // Logic for MAN_RED1_GREEN2
    traffic_red1_green2();
    updateClockBuffer((timers[1].counter * TIMER_TICK + 999) / 1000, 1);
    // Set enviroment for next state
    if (isButtonPressed(0) == 1) {
      status = MAN_RED1_AMBER2;
      setTimer(1, time_MAN_OUT);
    }
    if (isButtonPressed(2) == 1) {
      traffic_all_on();
      status = MAN_BLINK;
      setTimer(1, time_MAN_OUT);
      setTimer(2, time_BLINK);
    }
    // Return to auto mode
    if (timers[1].flag == 1) {
      timers[1].flag = 0;
      isButtonPressed(1);
      isButtonPressed(2);
      status = RED1_GREEN2;
      setTimer(0, time_green);
    }
    break;
  case MAN_RED1_AMBER2:
    // Logic for MAN_RED1_AMBER2
    traffic_red1_amber2();
    updateClockBuffer((timers[1].counter * TIMER_TICK + 999) / 1000, 1);
    // Set enviroment for next state
    if (isButtonPressed(0) == 1) {
      status = MAN_GREEN1_RED2;
      setTimer(1, time_MAN_OUT);
    }
    if (isButtonPressed(2) == 1) {
      traffic_all_on();
      status = MAN_BLINK;
      setTimer(1, time_MAN_OUT);
      setTimer(2, time_BLINK);
    }
    // Return to auto mode
    if (timers[1].flag == 1) {
      timers[1].flag = 0;
      isButtonPressed(1);
      isButtonPressed(2);
      status = RED1_AMBER2;
      setTimer(0, time_amber);
    }
    break;
  case MAN_GREEN1_RED2:
    // Logic for MAN_GREEN1_RED2
    traffic_green1_red2();
    updateClockBuffer((timers[1].counter * TIMER_TICK + 999) / 1000, 1);
    // Set enviroment for next state
    if (isButtonPressed(0) == 1) {
      status = MAN_AMBER1_RED2;
      setTimer(1, time_MAN_OUT);
    }
    if (isButtonPressed(2) == 1) {
      traffic_all_on();
      status = MAN_BLINK;
      setTimer(1, time_MAN_OUT);
      setTimer(2, time_BLINK);
    }
    // Return to auto mode
    if (timers[1].flag == 1) {
      timers[1].flag = 0;
      isButtonPressed(1);
      isButtonPressed(2);
      status = GREEN1_RED2;
      setTimer(0, time_green);
    }
    break;
  case MAN_AMBER1_RED2:
    // Logic for MAN_AMBER1_RED2
    traffic_amber1_red2();
    updateClockBuffer((timers[1].counter * TIMER_TICK + 999) / 1000, 1);
    // Set enviroment for next state
    if (isButtonPressed(0) == 1) {
      status = MAN_RED1_GREEN2;
      setTimer(1, time_MAN_OUT);
    }
    if (isButtonPressed(2) == 1) {
      traffic_all_on();
      status = MAN_BLINK;
      setTimer(1, time_MAN_OUT);
      setTimer(2, time_BLINK);
    }
    // Return to auto mode
    if (timers[1].flag == 1) {
      timers[1].flag = 0;
      isButtonPressed(1);
      isButtonPressed(2);
      status = AMBER1_RED2;
      setTimer(0, time_amber);
    }
    break;
  case MAN_BLINK:
    // Logic for MAN_BLINK
    updateClockBuffer((timers[1].counter * TIMER_TICK + 999) / 1000, 1);
    if (timers[2].flag == 1) {
      traffic_all_toggle();
      setTimer(2, time_BLINK);
    }
    // Return to MAN_RED1_GREEN2
    if (isButtonPressed(2) == 1) {
      timers[2].flag = 0;
      isButtonPressed(0);
      status = MAN_RED1_GREEN2;
      setTimer(1, time_MAN_OUT);
    }
    // Return to auto mode
    if (timers[1].flag == 1) {
      timers[1].flag = 0;
      timers[2].flag = 0;
      isButtonPressed(0);
      isButtonPressed(1);
      status = RED1_GREEN2;
      setTimer(0, time_green);
    }
    break;
  default:
    break;
  }
}
