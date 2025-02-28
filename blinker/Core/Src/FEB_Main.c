#include "stm32f4xx_hal.h"
#include "stdbool.h"

static bool blink_flag = false;

void FEB_Main_setup(void) {

}

void FEB_Main_loop(void) {
	if (blink_flag) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		HAL_Delay(500);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_13) {
		if (blink_flag) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		}
		blink_flag = !blink_flag;
	}
}
