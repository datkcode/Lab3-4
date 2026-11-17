#include "fsm_manual.h"
#include "button.h"
#include "global.h"
#include "software_timer.h"
#include "traffic_light.h"

#define MANUAL_TIMEOUT_SEC  10
#define MANUAL_TIMEOUT_MS   10000

typedef enum {
    MAN_INIT,
    MAN_IDLE,
    M_R_G,
    M_R_A,
    M_G_R,
    M_A_R
} ManualState_t;

static ManualState_t current_state = MAN_INIT;

/**
 * @brief Run Manual Traffic Light Control FSM (1-to-1 switch-case).
 */
void fsm_manual_run(void)
{
    switch (current_state) {
        case MAN_INIT:
            /* Prepare environment for waiting idle state */
            manual_enter_flag = 0;
            clearTimer(TIMER_MANUAL);
            current_state = MAN_IDLE;
            break;

        case MAN_IDLE:
            /* 1. Hardware control program for MAN_IDLE (other FSM owns hardware) */

            /* 2. Outgoing transitions (1 outgoing arrow -> 1 if block) */
            if (manual_enter_flag) {
                /* Prepare environment for M_R_G */
                manual_enter_flag = 0;
                countdown_road0 = 1;
                countdown_road1 = MANUAL_TIMEOUT_SEC;
                setTimer(TIMER_MANUAL, MANUAL_TIMEOUT_MS);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = M_R_G;
            }
            break;

        case M_R_G:
            /* 1. Hardware control program for M_R_G */
            traffic_light_set_R_G();
            traffic_light_manual_tick(1);

            /* 2. Outgoing transitions (2 outgoing arrows -> 2 if blocks) */
            if (is_button_pressed(0)) {
                /* Prepare environment for M_R_A */
                countdown_road0 = 2;
                countdown_road1 = MANUAL_TIMEOUT_SEC;
                setTimer(TIMER_MANUAL, MANUAL_TIMEOUT_MS);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = M_R_A;
            }
            if (isTimerExpired(TIMER_MANUAL)) {
                /* Prepare environment for fsm_automatic after 10s inactivity */
                clearTimer(TIMER_MANUAL);
                clearTimer(TIMER_1SEC);
                auto_enter_flag = 1;
                current_state = MAN_IDLE;
            }
            break;

        case M_R_A:
            /* 1. Hardware control program for M_R_A */
            traffic_light_set_R_A();
            traffic_light_manual_tick(2);

            /* 2. Outgoing transitions (2 outgoing arrows -> 2 if blocks) */
            if (is_button_pressed(0)) {
                /* Prepare environment for M_G_R */
                countdown_road0 = 3;
                countdown_road1 = MANUAL_TIMEOUT_SEC;
                setTimer(TIMER_MANUAL, MANUAL_TIMEOUT_MS);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = M_G_R;
            }
            if (isTimerExpired(TIMER_MANUAL)) {
                /* Prepare environment for fsm_automatic after 10s inactivity */
                clearTimer(TIMER_MANUAL);
                clearTimer(TIMER_1SEC);
                auto_enter_flag = 1;
                current_state = MAN_IDLE;
            }
            break;

        case M_G_R:
            /* 1. Hardware control program for M_G_R */
            traffic_light_set_G_R();
            traffic_light_manual_tick(3);

            /* 2. Outgoing transitions (2 outgoing arrows -> 2 if blocks) */
            if (is_button_pressed(0)) {
                /* Prepare environment for M_A_R */
                countdown_road0 = 4;
                countdown_road1 = MANUAL_TIMEOUT_SEC;
                setTimer(TIMER_MANUAL, MANUAL_TIMEOUT_MS);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = M_A_R;
            }
            if (isTimerExpired(TIMER_MANUAL)) {
                /* Prepare environment for fsm_automatic after 10s inactivity */
                clearTimer(TIMER_MANUAL);
                clearTimer(TIMER_1SEC);
                auto_enter_flag = 1;
                current_state = MAN_IDLE;
            }
            break;

        case M_A_R:
            /* 1. Hardware control program for M_A_R */
            traffic_light_set_A_R();
            traffic_light_manual_tick(4);

            /* 2. Outgoing transitions (2 outgoing arrows -> 2 if blocks) */
            if (is_button_pressed(0)) {
                /* Prepare environment for M_R_G */
                countdown_road0 = 1;
                countdown_road1 = MANUAL_TIMEOUT_SEC;
                setTimer(TIMER_MANUAL, MANUAL_TIMEOUT_MS);
                setTimer(TIMER_1SEC, 1000);
                traffic_light_set_display(countdown_road0, countdown_road1);
                current_state = M_R_G;
            }
            if (isTimerExpired(TIMER_MANUAL)) {
                /* Prepare environment for fsm_automatic after 10s inactivity */
                clearTimer(TIMER_MANUAL);
                clearTimer(TIMER_1SEC);
                auto_enter_flag = 1;
                current_state = MAN_IDLE;
            }
            break;

        default:
            current_state = MAN_INIT;
            break;
    }
}
