#ifndef INC_FEB_CAN_PINGPONG_H_
#define INC_FEB_CAN_PINGPONG_H_

#include "stm32f4xx_hal.h"

uint8_t FEB_CAN_PingPong_Filter(CAN_HandleTypeDef *hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_PingPong_Store_Msg(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[8]);
void FEB_CAN_PingPong_Transmit(void);

#endif /* INC_FEB_CAN_PINGPONG_H_ */
