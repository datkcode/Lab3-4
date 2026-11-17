#ifndef INC_GLOBAL_H
#define INC_GLOBAL_H

#include "main.h"
#include "software_timer.h"

// GPIO_PORT & GPIO_PIN
#define LED_PORT GPIOA
#define RED1_PIN GPIO_PIN_4
#define AMBER1_PIN GPIO_PIN_5
#define GREEN1_PIN GPIO_PIN_6
#define RED2_PIN GPIO_PIN_7
#define AMBER2_PIN GPIO_PIN_8
#define GREEN2_PIN GPIO_PIN_9

#define BTN_PORT GPIOA
#define BTN_MODE_PIN GPIO_PIN_1
#define BTN_SET_PIN GPIO_PIN_2
#define BTN_SAVE_PIN GPIO_PIN_3

#define SEG_PORT GPIOB
#define SEG0_PIN GPIO_PIN_0
#define SEG1_PIN GPIO_PIN_1
#define SEG2_PIN GPIO_PIN_2
#define SEG3_PIN GPIO_PIN_3
#define SEG4_PIN GPIO_PIN_4
#define SEG5_PIN GPIO_PIN_5
#define SEG6_PIN GPIO_PIN_6
#define EN0_PIN GPIO_PIN_7
#define EN1_PIN GPIO_PIN_8
#define EN2_PIN GPIO_PIN_9
#define EN3_PIN GPIO_PIN_10
// LED
#define LED_ON GPIO_PIN_SET
#define LED_OFF GPIO_PIN_RESET
// automatic_time
#define time_R_G 3000
#define time_R_A 2000
// manual_time
#define time_MAN_OUT    7000
#define time_BLINK      500
#define time_7SEG       10

// automatic_state
#define INIT 0
#define RED1_GREEN2 1
#define RED1_AMBER2 2
#define GREEN1_RED2 3
#define AMBER1_RED2 4
// manual_state
#define MAN_RED1_GREEN2       10
#define MAN_RED1_AMBER2       11
#define MAN_GREEN1_RED2       12
#define MAN_AMBER1_RED2       13
#define MAN_BLINK             14
// setting_state
#define SET_RED               20
#define SET_AMBER             21
#define SET_GREEN             22

extern int status;
extern int time_red;
extern int time_amber;
extern int time_green;
extern int temp_time;

#endif /*INC_GLOBAL_H*/