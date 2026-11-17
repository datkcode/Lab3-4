#include "button.h"
#include "main.h"

#define DURATION_FOR_AUTO_INCREASING  100  /* 100 * 10ms = 1s */
#define BUTTON_IS_PRESSED             GPIO_PIN_RESET
#define BUTTON_IS_RELEASED            GPIO_PIN_SET

static GPIO_TypeDef *const button_ports[NO_OF_BUTTONS] = {
    BTN_MODE_GPIO_Port,
    BTN_SET_GPIO_Port,
    BTN_SAVE_GPIO_Port
};

static const uint16_t button_pins[NO_OF_BUTTONS] = {
    BTN_MODE_Pin,
    BTN_SET_Pin,
    BTN_SAVE_Pin
};

static GPIO_PinState buttonBuffer[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};
static GPIO_PinState prevButtonBuffer[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};
static GPIO_PinState debounceBuffer1[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};
static GPIO_PinState debounceBuffer2[NO_OF_BUTTONS] = {
    BUTTON_IS_RELEASED, BUTTON_IS_RELEASED, BUTTON_IS_RELEASED
};

static uint16_t counterForButtonPress1s[NO_OF_BUTTONS] = {0};
static uint8_t flagForButtonPress1s[NO_OF_BUTTONS] = {0};
static volatile uint8_t isr_button_event[NO_OF_BUTTONS] = {0};

uint8_t button[NO_OF_BUTTONS] = {0};

/**
 * @brief Sample and debounce all buttons; call every 10ms in TIM2 ISR.
 */
void button_reading(void)
{
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        debounceBuffer2[i] = debounceBuffer1[i];
        debounceBuffer1[i] = HAL_GPIO_ReadPin(button_ports[i], button_pins[i]);

        if (debounceBuffer1[i] == debounceBuffer2[i]) {
            prevButtonBuffer[i] = buttonBuffer[i];
            buttonBuffer[i] = debounceBuffer1[i];

            if (prevButtonBuffer[i] == BUTTON_IS_RELEASED &&
                buttonBuffer[i] == BUTTON_IS_PRESSED) {
                isr_button_event[i] = 1;
            }
        }

        if (buttonBuffer[i] == BUTTON_IS_PRESSED) {
            if (counterForButtonPress1s[i] < DURATION_FOR_AUTO_INCREASING) {
                counterForButtonPress1s[i]++;
            } else {
                flagForButtonPress1s[i] = 1;
            }
        } else {
            counterForButtonPress1s[i] = 0;
            flagForButtonPress1s[i] = 0;
        }
    }
}

/**
 * @brief Atomically latch edge-triggered press events once per superloop tick.
 */
void button_latch_events(void)
{
    __disable_irq();
    for (uint8_t i = 0; i < NO_OF_BUTTONS; i++) {
        button[i] = isr_button_event[i];
        isr_button_event[i] = 0;
    }
    __enable_irq();
}

/**
 * @brief Consume and return 1 if button[index] had a new debounced press event.
 */
uint8_t is_button_pressed(uint8_t index)
{
    if (index >= NO_OF_BUTTONS) {
        return 0;
    }

    if (button[index] != 0) {
        button[index] = 0;
        return 1;
    }
    return 0;
}

/**
 * @brief Return 1 if button[index] has been held continuously for at least 1s.
 */
uint8_t is_button_pressed_1s(uint8_t index)
{
    if (index >= NO_OF_BUTTONS) {
        return 0;
    }

    return (flagForButtonPress1s[index] == 1) ? 1U : 0U;
}
