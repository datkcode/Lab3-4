#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include <stdint.h>

#define DEFAULT_RED_TIME    5
#define DEFAULT_AMBER_TIME  2
#define DEFAULT_GREEN_TIME  3

/* Traffic light durations in seconds (range 1 - 99) */
extern uint8_t time_red;
extern uint8_t time_amber;
extern uint8_t time_green;

/* Countdown values displayed on Road 0 and Road 1 7-segment pairs */
extern uint8_t countdown_road0;
extern uint8_t countdown_road1;

/* Inter-FSM transition handoff flags */
extern uint8_t auto_enter_flag;
extern uint8_t manual_enter_flag;
extern uint8_t setting_enter_flag;

#endif /* INC_GLOBAL_H_ */
