#include "software_timer.h"

SoftwareTimer timers[NUM_TIMERS];

void setTimer(int id, int duration) {
    if (id >= NUM_TIMERS) return;
    timers[id].counter = duration / TIMER_TICK;
    timers[id].flag = 0;
}

int isTimerExpired(int id) {
    if (id >= NUM_TIMERS) return 0;
    if (timers[id].flag == 1) {
        timers[id].flag = 0; 
        return 1;
    }
    return 0;
}

void timerRun(void){
    for (int i = 0; i < NUM_TIMERS; i++) {
        if (timers[i].counter > 0) {
            timers[i].counter--;
            if (timers[i].counter == 0) {
                timers[i].flag = 1;
            }
        }
    }
}
