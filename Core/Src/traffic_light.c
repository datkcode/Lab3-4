#include "traffic_light.h"

void traffic_red1_green2(void) {
  HAL_GPIO_WritePin(LED_PORT, RED1_PIN | GREEN2_PIN, LED_ON);
  HAL_GPIO_WritePin(LED_PORT, AMBER1_PIN | GREEN1_PIN | RED2_PIN | AMBER2_PIN,
                    LED_OFF);
}

void traffic_red1_amber2(void) {
  HAL_GPIO_WritePin(LED_PORT, RED1_PIN | AMBER2_PIN, LED_ON);
  HAL_GPIO_WritePin(LED_PORT, AMBER1_PIN | GREEN1_PIN | RED2_PIN | GREEN2_PIN,
                    LED_OFF);
}

void traffic_green1_red2(void) {
  HAL_GPIO_WritePin(LED_PORT, GREEN1_PIN | RED2_PIN, LED_ON);
  HAL_GPIO_WritePin(LED_PORT, RED1_PIN | AMBER1_PIN | AMBER2_PIN | GREEN2_PIN,
                    LED_OFF);
}

void traffic_amber1_red2(void) {
  HAL_GPIO_WritePin(LED_PORT, AMBER1_PIN | RED2_PIN, LED_ON);
  HAL_GPIO_WritePin(LED_PORT, RED1_PIN | GREEN1_PIN | AMBER2_PIN | GREEN2_PIN,
                    LED_OFF);
}

void traffic_all_on(void) {
  HAL_GPIO_WritePin(LED_PORT,
                    RED1_PIN | AMBER1_PIN | GREEN1_PIN | RED2_PIN | AMBER2_PIN |
                        GREEN2_PIN,
                    LED_ON);
}

void traffic_all_toggle(void) {
  HAL_GPIO_TogglePin(LED_PORT, RED1_PIN | AMBER1_PIN | GREEN1_PIN | RED2_PIN |
                                   AMBER2_PIN | GREEN2_PIN);
}

void traffic_red_blink_init(void) {
  HAL_GPIO_WritePin(LED_PORT, RED1_PIN | RED2_PIN, LED_ON);
  HAL_GPIO_WritePin(LED_PORT, AMBER1_PIN | GREEN1_PIN | AMBER2_PIN | GREEN2_PIN,
                    LED_OFF);
}

void traffic_red_toggle(void) {
  HAL_GPIO_TogglePin(LED_PORT, RED1_PIN | RED2_PIN);
}

void traffic_amber_blink_init(void) {
  HAL_GPIO_WritePin(LED_PORT, AMBER1_PIN | AMBER2_PIN, LED_ON);
  HAL_GPIO_WritePin(LED_PORT, RED1_PIN | GREEN1_PIN | RED2_PIN | GREEN2_PIN,
                    LED_OFF);
}

void traffic_amber_toggle(void) {
  HAL_GPIO_TogglePin(LED_PORT, AMBER1_PIN | AMBER2_PIN);
}

void traffic_green_blink_init(void) {
  HAL_GPIO_WritePin(LED_PORT, GREEN1_PIN | GREEN2_PIN, LED_ON);
  HAL_GPIO_WritePin(LED_PORT, RED1_PIN | AMBER1_PIN | RED2_PIN | AMBER2_PIN,
                    LED_OFF);
}

void traffic_green_toggle(void) {
  HAL_GPIO_TogglePin(LED_PORT, GREEN1_PIN | GREEN2_PIN);
}
