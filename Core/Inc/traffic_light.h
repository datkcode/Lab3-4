#ifndef INC_TRAFFIC_LIGHT_H
#define INC_TRAFFIC_LIGHT_H
#include "global.h"
#include "main.h"

void traffic_red1_green2(void);
void traffic_red1_amber2(void);
void traffic_green1_red2(void);
void traffic_amber1_red2(void);
void traffic_all_on(void);
void traffic_all_toggle(void);
void traffic_red_blink_init(void);
void traffic_red_toggle(void);
void traffic_amber_blink_init(void);
void traffic_amber_toggle(void);
void traffic_green_blink_init(void);
void traffic_green_toggle(void);

#endif /*INC_TRAFFIC_LIGHT_H*/
