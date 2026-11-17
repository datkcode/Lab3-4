#include "fsm_setting.h"
#include "button.h"
#include "global.h"
#include "software_timer.h"
#include "traffic_light.h"

#define MODE_RED_SET    2
#define MODE_AMBER_SET  3
#define MODE_GREEN_SET  4
#define BLINK_HALF_MS   250

typedef enum {
    SET_INIT,
    SET_IDLE,
    S_RED,
    S_AMBER,
    S_GREEN
} SettingState_t;

static SettingState_t current_state = SET_INIT;
static uint8_t temp_duration = 1;

/**
 * @brief Run Setting FSM for Modes 2, 3, and 4 (1-to-1 switch-case).
 */
void fsm_setting_run(void)
{
    switch (current_state) {
        case SET_INIT:
            /* Prepare environment for waiting idle state */
            setting_enter_flag = 0;
            clearTimer(TIMER_BLINK);
            current_state = SET_IDLE;
            break;

        case SET_IDLE:
            /* 1. Hardware control program for SET_IDLE (other FSM owns hardware) */

            /* 2. Outgoing transitions (1 outgoing arrow -> 1 if block) */
            if (setting_enter_flag) {
                /* Prepare environment for S_RED (Mode 2) */
                setting_enter_flag = 0;
                temp_duration = time_red;
                traffic_light_all_off();
                traffic_light_set_red_all(1);
                setTimer(TIMER_BLINK, BLINK_HALF_MS);
                traffic_light_set_display(MODE_RED_SET, temp_duration);
                current_state = S_RED;
            }
            break;

        case S_RED:
            /* 1. Hardware control program for S_RED (Mode 2: 2Hz red LEDs + 7-seg) */
            traffic_light_blink_red();
            traffic_light_set_display(MODE_RED_SET, temp_duration);

            /* 2. Outgoing transitions (3 outgoing arrows -> 3 if blocks) */
            if (is_button_pressed(0)) {
                /* Prepare environment for S_AMBER (Mode 3) */
                temp_duration = time_amber;
                traffic_light_all_off();
                traffic_light_set_amber_all(1);
                setTimer(TIMER_BLINK, BLINK_HALF_MS);
                traffic_light_set_display(MODE_AMBER_SET, temp_duration);
                current_state = S_AMBER;
            }
            if (is_button_pressed(1)) {
                /* Self-loop: increment red duration (1 - 99) */
                temp_duration = (temp_duration >= 99U) ? 1U : (uint8_t)(temp_duration + 1U);
                traffic_light_set_display(MODE_RED_SET, temp_duration);
                current_state = S_RED;
            }
            if (is_button_pressed(2)) {
                /* Self-loop: save red duration and keep intersection phases consistent */
                time_red = temp_duration;
                if (time_red > time_amber) {
                    time_green = (uint8_t)(time_red - time_amber);
                } else if (time_red > 1U) {
                    time_green = 1U;
                    time_amber = (uint8_t)(time_red - 1U);
                } else {
                    time_green = 1U;
                    time_amber = 1U;
                }
                traffic_light_set_display(MODE_RED_SET, temp_duration);
                current_state = S_RED;
            }
            break;

        case S_AMBER:
            /* 1. Hardware control program for S_AMBER (Mode 3: 2Hz amber LEDs + 7-seg) */
            traffic_light_blink_amber();
            traffic_light_set_display(MODE_AMBER_SET, temp_duration);

            /* 2. Outgoing transitions (3 outgoing arrows -> 3 if blocks) */
            if (is_button_pressed(0)) {
                /* Prepare environment for S_GREEN (Mode 4) */
                temp_duration = time_green;
                traffic_light_all_off();
                traffic_light_set_green_all(1);
                setTimer(TIMER_BLINK, BLINK_HALF_MS);
                traffic_light_set_display(MODE_GREEN_SET, temp_duration);
                current_state = S_GREEN;
            }
            if (is_button_pressed(1)) {
                /* Self-loop: increment amber duration (1 - 99) */
                temp_duration = (temp_duration >= 99U) ? 1U : (uint8_t)(temp_duration + 1U);
                traffic_light_set_display(MODE_AMBER_SET, temp_duration);
                current_state = S_AMBER;
            }
            if (is_button_pressed(2)) {
                /* Self-loop: save amber duration and synchronize red duration */
                time_amber = temp_duration;
                if ((uint16_t)time_green + time_amber <= 99U) {
                    time_red = (uint8_t)(time_green + time_amber);
                } else {
                    time_red = 99U;
                    time_green = (uint8_t)(99U - time_amber);
                }
                traffic_light_set_display(MODE_AMBER_SET, temp_duration);
                current_state = S_AMBER;
            }
            break;

        case S_GREEN:
            /* 1. Hardware control program for S_GREEN (Mode 4: 2Hz green LEDs + 7-seg) */
            traffic_light_blink_green();
            traffic_light_set_display(MODE_GREEN_SET, temp_duration);

            /* 2. Outgoing transitions (3 outgoing arrows -> 3 if blocks) */
            if (is_button_pressed(0)) {
                /* Prepare environment to return to fsm_automatic */
                clearTimer(TIMER_BLINK);
                traffic_light_all_off();
                auto_enter_flag = 1;
                current_state = SET_IDLE;
            }
            if (is_button_pressed(1)) {
                /* Self-loop: increment green duration (1 - 99) */
                temp_duration = (temp_duration >= 99U) ? 1U : (uint8_t)(temp_duration + 1U);
                traffic_light_set_display(MODE_GREEN_SET, temp_duration);
                current_state = S_GREEN;
            }
            if (is_button_pressed(2)) {
                /* Self-loop: save green duration and synchronize red duration */
                time_green = temp_duration;
                if ((uint16_t)time_green + time_amber <= 99U) {
                    time_red = (uint8_t)(time_green + time_amber);
                } else {
                    time_red = 99U;
                    time_amber = (uint8_t)(99U - time_green);
                }
                traffic_light_set_display(MODE_GREEN_SET, temp_duration);
                current_state = S_GREEN;
            }
            break;

        default:
            current_state = SET_INIT;
            break;
    }
}
