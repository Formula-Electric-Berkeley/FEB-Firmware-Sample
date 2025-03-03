#include "stm32f4xx_hal.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

#include "FEB_Main.h"
#include "FEB_CAN.h"
#include "FEB_CAN_Lib.h"
#include "FEB_CAN_PingPong.h"

extern UART_HandleTypeDef huart2;

extern bool FEB_CAN_PingPong_Transmit_Flag;
extern struct feb_can_lib_pingpong_t can_rx;
extern struct feb_can_lib_pingpong_t can_tx;

void FEB_Main_Setup(void) {
	FEB_CAN_Init();

	can_tx.count = 0;
	FEB_CAN_PingPong_Transmit();
}

void FEB_Main_Loop(void) {
	char uart_str[64];

	if (FEB_CAN_PingPong_Transmit_Flag) {
		FEB_CAN_PingPong_Transmit_Flag = false;

		sprintf(uart_str, "Count: %d\n", can_rx.count);
		HAL_UART_Transmit(&huart2, (uint8_t *) uart_str, strlen(uart_str), 1000);

		FEB_CAN_PingPong_Transmit();
	}

	HAL_Delay(1000);
}
