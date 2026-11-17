#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

#define MAX_TIMERS      5
#define TIMER_CYCLE_MS  10

/* Timer channel assignments */
#define TIMER_AUTO      0   /* Automatic FSM phase transition timer (flag[0]) */
#define TIMER_1SEC      1   /* 1-second tick for 7-segment countdown */
#define TIMER_BLINK     2   /* 2Hz (250ms half-period) blink timer for Setting FSM */
#define TIMER_MANUAL    3   /* 10-second inactivity timeout for Manual FSM */
#define TIMER_7SEG      4   /* 10ms multiplexing tick for 4x 7-segment display */

extern volatile uint8_t flag[MAX_TIMERS];

void setTimer(uint8_t index, uint32_t duration_ms);
void clearTimer(uint8_t index);
uint8_t isTimerExpired(uint8_t index);
void timerRun(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
