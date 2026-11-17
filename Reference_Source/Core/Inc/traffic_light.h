#ifndef INC_TRAFFIC_LIGHT_H_
#define INC_TRAFFIC_LIGHT_H_

#include <stdint.h>

void traffic_light_init(void);
void traffic_light_all_off(void);

/* Two-road phase outputs */
void traffic_light_set_R_G(void);
void traffic_light_set_R_A(void);
void traffic_light_set_G_R(void);
void traffic_light_set_A_R(void);

/* Color-group controls and 2Hz blinking helpers for Setting FSM */
void traffic_light_set_red_all(uint8_t on);
void traffic_light_set_amber_all(uint8_t on);
void traffic_light_set_green_all(uint8_t on);
void traffic_light_blink_red(void);
void traffic_light_blink_amber(void);
void traffic_light_blink_green(void);

/* 4-digit 7-segment display and non-blocking countdown helpers */
void traffic_light_set_display(uint8_t road0_val, uint8_t road1_val);
void traffic_light_countdown_tick(void);
void traffic_light_manual_tick(uint8_t label_val);
void traffic_light_scan_7seg(void);

#endif /* INC_TRAFFIC_LIGHT_H_ */
