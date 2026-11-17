#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#include <stdint.h>

#define TIMER_TICK  10
#define NUM_TIMERS 4

typedef struct {
    int counter;
    int flag;
} SoftwareTimer;

// External Global Variable declaration
extern SoftwareTimer timers[NUM_TIMERS];
// Function Prototype
void setTimer(int id, int duration);
int isTimerExpired(int id);
void timerRun(void);

#endif /* INC_SOFTWARE_TIMER_H_ */
