#ifndef INC_LED_7SEG_H
#define INC_LED_7SEG_H

#include "global.h"

void display7SEG(int num);
void update7SEG(int index);
void updateClockBuffer(int time1, int time2);
void led_7seg_run(void);

#endif /* INC_LED_7SEG_H */
