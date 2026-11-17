#include "fsm_automatic.h"
#include "button.h"
#include "global.h"
#include "led_7seg.h"
#include "software_timer.h"
#include "traffic_light.h"

void fsm_automatic_run(void) {
  switch (status) {
  case INIT:
    status = RED1_GREEN2;
    setTimer(0, time_green);
    setTimer(3, time_7SEG);
    break;
  case RED1_GREEN2:
    // Logic for RED1_GREEN2
    traffic_red1_green2();
    updateClockBuffer((timers[0].counter * TIMER_TICK + 999) / 1000 +
                          time_amber / 1000,
                      (timers[0].counter * TIMER_TICK + 999) / 1000);
    // Set enviroment for next state in fsm_automatic
    if (timers[0].flag == 1) {
      status = RED1_AMBER2;
      setTimer(0, time_amber);
    }
    // Set enviroment for next state in fsm_manual
    if (isButtonPressed(1) == 1) {
      isButtonPressed(0);
      isButtonPressed(2);
      status = MAN_RED1_GREEN2;
      setTimer(1, time_MAN_OUT);
    }
    // Set enviroment for next state in fsm_setting
    if (isButtonPressed(0) == 1) {
      isButtonPressed(1);
      isButtonLongPressed(1);
      isButtonPressed(2);
      temp_time = time_red / 1000;
      traffic_red_blink_init();
      status = SET_RED;
      setTimer(2, time_BLINK);
    }
    break;
  case RED1_AMBER2:
    // Logic for RED1_AMBER2
    traffic_red1_amber2();
    updateClockBuffer((timers[0].counter * TIMER_TICK + 999) / 1000,
                      (timers[0].counter * TIMER_TICK + 999) / 1000);
    // Set enviroment for next state
    if (timers[0].flag == 1) {
      status = GREEN1_RED2;
      setTimer(0, time_green);
    }
    break;
  case GREEN1_RED2:
    // Logic for GREEN1_RED2
    traffic_green1_red2();
    updateClockBuffer((timers[0].counter * TIMER_TICK + 999) / 1000,
                      (timers[0].counter * TIMER_TICK + 999) / 1000 +
                          time_amber / 1000);
    // Set enviroment for next state
    if (timers[0].flag == 1) {
      status = AMBER1_RED2;
      setTimer(0, time_amber);
    }
    break;
  case AMBER1_RED2:
    // Logic for AMBER1_RED2
    traffic_amber1_red2();
    updateClockBuffer((timers[0].counter * TIMER_TICK + 999) / 1000,
                      (timers[0].counter * TIMER_TICK + 999) / 1000);
    // Set enviroment for next state
    if (timers[0].flag == 1) {
      status = RED1_GREEN2;
      setTimer(0, time_green);
    }
    break;
  default:
    break;
  }
}