#include "fsm_automatic.h"
#include "button.h"
#include "global.h"
#include "software_timer.h"
#include "traffic_light.h"

typedef enum {
    AUTO_INIT,
    AUTO_IDLE,
    R_G,
    R_A,
    G_R,
    A_R
} AutoState_t;

static AutoState_t current_state = AUTO_INIT;

/**
 * @brief Run Mode 1 Automatic Traffic Light FSM (1-to-1 switch-case).
 */
void fsm_automatic_run(void)
{
    switch (current_state) {
        case AUTO_INIT:
            /* Prepare environment for first operational state (R_G) */
            auto_enter_flag = 0;
            countdown_road0 = time_red;
            countdown_road1 = time_green;
            setTimer(TIMER_AUTO, (uint32_t)time_green * 1000U);
            setTimer(TIMER_1SEC, 1000);
            traffic_light_set_display(countdown_road0, countdown_road1);
            current_state = R_G;
            break;

        case AUTO_IDLE:
            /* 1. Hardware control program for AUTO_IDLE (other FSM owns hardware) */

            /* 2. Outgoing transitions (1 outgoing arrow -> 1 if block) */
            if (auto_enter_flag) {
                /* Prepare environment for R_G */
                auto_enter_flag = 0;
                countdown_road0 = time_red;
                countdown_road1 = time_green;
                setTimer(TIMER_AUTO, (uint32_t)time_green * 1000U);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = R_G;
            }
            break;

        case R_G:
            /* 1. Hardware control program for R_G */
            traffic_light_set_R_G();
            traffic_light_countdown_tick();

            /* 2. Outgoing transitions (3 outgoing arrows -> 3 if blocks) */
            if (isTimerExpired(TIMER_AUTO)) {
                /* Prepare environment for R_A using flag[0] */
                countdown_road0 = time_amber;
                countdown_road1 = time_amber;
                setTimer(TIMER_AUTO, (uint32_t)time_amber * 1000U);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = R_A;
            }
            if (is_button_pressed(0)) {
                /* Prepare environment for fsm_setting */
                clearTimer(TIMER_AUTO);
                clearTimer(TIMER_1SEC);
                setting_enter_flag = 1;
                current_state = AUTO_IDLE;
            }
            if (is_button_pressed(1)) {
                /* Prepare environment for fsm_manual */
                clearTimer(TIMER_AUTO);
                clearTimer(TIMER_1SEC);
                manual_enter_flag = 1;
                current_state = AUTO_IDLE;
            }
            break;

        case R_A:
            /* 1. Hardware control program for R_A */
            traffic_light_set_R_A();
            traffic_light_countdown_tick();

            /* 2. Outgoing transitions (1 outgoing arrow -> 1 if block) */
            if (isTimerExpired(TIMER_AUTO)) {
                /* Prepare environment for G_R using flag[0] */
                countdown_road0 = time_green;
                countdown_road1 = time_red;
                setTimer(TIMER_AUTO, (uint32_t)time_green * 1000U);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = G_R;
            }
            break;

        case G_R:
            /* 1. Hardware control program for G_R */
            traffic_light_set_G_R();
            traffic_light_countdown_tick();

            /* 2. Outgoing transitions (1 outgoing arrow -> 1 if block) */
            if (isTimerExpired(TIMER_AUTO)) {
                /* Prepare environment for A_R using flag[0] */
                countdown_road0 = time_amber;
                countdown_road1 = time_amber;
                setTimer(TIMER_AUTO, (uint32_t)time_amber * 1000U);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = A_R;
            }
            break;

        case A_R:
            /* 1. Hardware control program for A_R */
            traffic_light_set_A_R();
            traffic_light_countdown_tick();

            /* 2. Outgoing transitions (1 outgoing arrow -> 1 if block) */
            if (isTimerExpired(TIMER_AUTO)) {
                /* Prepare environment for R_G using flag[0] */
                countdown_road0 = time_red;
                countdown_road1 = time_green;
                setTimer(TIMER_AUTO, (uint32_t)time_green * 1000U);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = R_G;
            }
            break;

        default:
            current_state = AUTO_INIT;
            break;
    }
}
