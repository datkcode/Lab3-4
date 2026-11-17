#include "software_timer.h"

static volatile uint32_t timer_counter[MAX_TIMERS] = {0};
volatile uint8_t flag[MAX_TIMERS] = {0};

/**
 * @brief Arm a software timer channel in milliseconds (non-blocking).
 */
void setTimer(uint8_t index, uint32_t duration_ms)
{
    if (index >= MAX_TIMERS) {
        return;
    }

    uint32_t ticks = duration_ms / TIMER_CYCLE_MS;
    if (ticks == 0 && duration_ms > 0) {
        ticks = 1;
    }

    timer_counter[index] = ticks;
    flag[index] = 0;
}

/**
 * @brief Disarm a software timer channel and clear its flag.
 */
void clearTimer(uint8_t index)
{
    if (index >= MAX_TIMERS) {
        return;
    }

    timer_counter[index] = 0;
    flag[index] = 0;
}

/**
 * @brief Return 1 if the specified software timer has expired, otherwise 0.
 */
uint8_t isTimerExpired(uint8_t index)
{
    if (index >= MAX_TIMERS) {
        return 0;
    }

    return (flag[index] == 1) ? 1U : 0U;
}

/**
 * @brief Advance all active software timers; call inside the 10ms TIM2 ISR.
 */
void timerRun(void)
{
    for (uint8_t i = 0; i < MAX_TIMERS; i++) {
        if (timer_counter[i] > 0) {
            timer_counter[i]--;
            if (timer_counter[i] == 0) {
                flag[i] = 1;
            }
        }
    }
}
