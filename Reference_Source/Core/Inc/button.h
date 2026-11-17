#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>

#define NO_OF_BUTTONS  3

extern uint8_t button[NO_OF_BUTTONS];

void button_reading(void);
void button_latch_events(void);
uint8_t is_button_pressed(uint8_t index);
uint8_t is_button_pressed_1s(uint8_t index);

#endif /* INC_BUTTON_H_ */
