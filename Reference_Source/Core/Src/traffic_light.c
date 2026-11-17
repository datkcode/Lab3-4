#include "traffic_light.h"
#include "global.h"
#include "main.h"
#include "software_timer.h"

/* Common-anode 7-segment bit patterns (bits 6..0 = g..a, 0 = ON, 1 = OFF) */
static const uint8_t seg7_patterns[10] = {
    0x40, /* 0 */
    0x79, /* 1 */
    0x24, /* 2 */
    0x30, /* 3 */
    0x19, /* 4 */
    0x12, /* 5 */
    0x02, /* 6 */
    0x78, /* 7 */
    0x00, /* 8 */
    0x10  /* 9 */
};

static const uint16_t en_pins[4] = {
    EN0_Pin, EN1_Pin, EN2_Pin, EN3_Pin
};

static const uint16_t seg_pins[7] = {
    SEG0_Pin, SEG1_Pin, SEG2_Pin, SEG3_Pin, SEG4_Pin, SEG5_Pin, SEG6_Pin
};

static uint8_t led_buffer[4] = {0, 0, 0, 0};
static uint8_t scan_index = 0;
static uint8_t blink_state = 1;

static void write_road0(GPIO_PinState r, GPIO_PinState y, GPIO_PinState g)
{
    HAL_GPIO_WritePin(LED_RED0_GPIO_Port, LED_RED0_Pin, r);
    HAL_GPIO_WritePin(LED_AMBER0_GPIO_Port, LED_AMBER0_Pin, y);
    HAL_GPIO_WritePin(LED_GREEN0_GPIO_Port, LED_GREEN0_Pin, g);
}

static void write_road1(GPIO_PinState r, GPIO_PinState y, GPIO_PinState g)
{
    HAL_GPIO_WritePin(LED_RED1_GPIO_Port, LED_RED1_Pin, r);
    HAL_GPIO_WritePin(LED_AMBER1_GPIO_Port, LED_AMBER1_Pin, y);
    HAL_GPIO_WritePin(LED_GREEN1_GPIO_Port, LED_GREEN1_Pin, g);
}

static void write_7seg_digit(uint8_t num)
{
    uint8_t pattern = (num < 10U) ? seg7_patterns[num] : 0x7FU;
    for (uint8_t bit = 0; bit < 7; bit++) {
        GPIO_PinState level = (pattern & (1U << bit)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
        HAL_GPIO_WritePin(SEG_GPIO_Port, seg_pins[bit], level);
    }
}

/**
 * @brief Initialize traffic light LEDs, 7-segment enables, and scan timer.
 */
void traffic_light_init(void)
{
    traffic_light_all_off();
    /* PNP base enables are active-low: SET turns all 4 digits OFF */
    HAL_GPIO_WritePin(EN_GPIO_Port, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);
    write_7seg_digit(0xFF);
    setTimer(TIMER_7SEG, 10);
}

/**
 * @brief Turn off all 12 traffic LEDs on both axes.
 */
void traffic_light_all_off(void)
{
    write_road0(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET);
    write_road1(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET);
}

void traffic_light_set_R_G(void)
{
    write_road0(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET);
    write_road1(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET);
}

void traffic_light_set_R_A(void)
{
    write_road0(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET);
    write_road1(GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET);
}

void traffic_light_set_G_R(void)
{
    write_road0(GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET);
    write_road1(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET);
}

void traffic_light_set_A_R(void)
{
    write_road0(GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET);
    write_road1(GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET);
}

void traffic_light_set_red_all(uint8_t on)
{
    GPIO_PinState lvl = on ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_road0(lvl, GPIO_PIN_RESET, GPIO_PIN_RESET);
    write_road1(lvl, GPIO_PIN_RESET, GPIO_PIN_RESET);
    blink_state = on ? 1U : 0U;
}

void traffic_light_set_amber_all(uint8_t on)
{
    GPIO_PinState lvl = on ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_road0(GPIO_PIN_RESET, lvl, GPIO_PIN_RESET);
    write_road1(GPIO_PIN_RESET, lvl, GPIO_PIN_RESET);
    blink_state = on ? 1U : 0U;
}

void traffic_light_set_green_all(uint8_t on)
{
    GPIO_PinState lvl = on ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_road0(GPIO_PIN_RESET, GPIO_PIN_RESET, lvl);
    write_road1(GPIO_PIN_RESET, GPIO_PIN_RESET, lvl);
    blink_state = on ? 1U : 0U;
}

static uint8_t update_blink_level(void)
{
    if (isTimerExpired(TIMER_BLINK)) {
        setTimer(TIMER_BLINK, 250);
        blink_state = (uint8_t)!blink_state;
    }
    return blink_state;
}

void traffic_light_blink_red(void)
{
    GPIO_PinState lvl = update_blink_level() ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_road0(lvl, GPIO_PIN_RESET, GPIO_PIN_RESET);
    write_road1(lvl, GPIO_PIN_RESET, GPIO_PIN_RESET);
}

void traffic_light_blink_amber(void)
{
    GPIO_PinState lvl = update_blink_level() ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_road0(GPIO_PIN_RESET, lvl, GPIO_PIN_RESET);
    write_road1(GPIO_PIN_RESET, lvl, GPIO_PIN_RESET);
}

void traffic_light_blink_green(void)
{
    GPIO_PinState lvl = update_blink_level() ? GPIO_PIN_SET : GPIO_PIN_RESET;
    write_road0(GPIO_PIN_RESET, GPIO_PIN_RESET, lvl);
    write_road1(GPIO_PIN_RESET, GPIO_PIN_RESET, lvl);
}

/**
 * @brief Update the 4-digit 7-segment display buffer for two 2-digit numbers.
 */
void traffic_light_set_display(uint8_t road0_val, uint8_t road1_val)
{
    if (road0_val > 99U) {
        road0_val = 99U;
    }
    if (road1_val > 99U) {
        road1_val = 99U;
    }

    led_buffer[0] = (uint8_t)(road0_val / 10U);
    led_buffer[1] = (uint8_t)(road0_val % 10U);
    led_buffer[2] = (uint8_t)(road1_val / 10U);
    led_buffer[3] = (uint8_t)(road1_val % 10U);
}

/**
 * @brief Decrement Road 0 and Road 1 countdowns once per second and refresh buffer.
 */
void traffic_light_countdown_tick(void)
{
    if (isTimerExpired(TIMER_1SEC)) {
        setTimer(TIMER_1SEC, 1000);
        if (countdown_road0 > 1U) {
            countdown_road0--;
        }
        if (countdown_road1 > 1U) {
            countdown_road1--;
        }
        traffic_light_set_display(countdown_road0, countdown_road1);
    }
}

/**
 * @brief Decrement manual inactivity timeout countdown once per second.
 */
void traffic_light_manual_tick(uint8_t label_val)
{
    if (isTimerExpired(TIMER_1SEC)) {
        setTimer(TIMER_1SEC, 1000);
        if (countdown_road1 > 0U) {
            countdown_road1--;
        }
        traffic_light_set_display(label_val, countdown_road1);
    }
}

/**
 * @brief Multiplex the 4 7-segment digits at a 10ms per-digit scan rate.
 */
void traffic_light_scan_7seg(void)
{
    if (!isTimerExpired(TIMER_7SEG)) {
        return;
    }
    setTimer(TIMER_7SEG, 10);

    /* Blank all digit enables before switching segments to avoid ghosting */
    HAL_GPIO_WritePin(EN_GPIO_Port, EN0_Pin | EN1_Pin | EN2_Pin | EN3_Pin, GPIO_PIN_SET);
    write_7seg_digit(led_buffer[scan_index]);
    HAL_GPIO_WritePin(EN_GPIO_Port, en_pins[scan_index], GPIO_PIN_RESET);

    scan_index = (uint8_t)((scan_index + 1U) & 0x03U);
}
