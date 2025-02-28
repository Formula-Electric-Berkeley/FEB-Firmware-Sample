#include "FEB_Main.h"
#include "stm32f4xx_hal.h"

#define QUEUE_SIZE 8

extern UART_HandleTypeDef huart2;

static uint8_t rx_queue[QUEUE_SIZE];
static uint8_t head = 0;
static uint8_t tail = 0;

void FEB_Main_setup(void) {
	HAL_UART_Receive_IT(&huart2, &rx_queue[tail], 1);
}

void FEB_Main_loop(void) {
	if (head != tail) {
		HAL_UART_Transmit(&huart2, &rx_queue[head], 1, 1000);
		head = (head + 1) % QUEUE_SIZE;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	tail = (tail + 1) % QUEUE_SIZE;
	HAL_UART_Receive_IT(&huart2, &rx_queue[tail], 1);
}
