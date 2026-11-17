#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

#define NO_OF_BUTTONS       3
#define BUTTON_PRESSED      GPIO_PIN_RESET  // Active-Low (nhấn mức 0)
#define BUTTON_RELEASED     GPIO_PIN_SET    // Nhả mức 1 (kéo lên nguồn)

void button_reading(void);
uint8_t isButtonPressed(uint8_t index);
uint8_t isButtonLongPressed(uint8_t index);

#endif /* INC_BUTTON_H_ */