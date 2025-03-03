#include "stm32f4xx_hal.h"
#include "stdbool.h"

#include "FEB_CAN_PingPong.h"
#include "FEB_CAN_Lib.h"

#define FEB_CAN_PINGPONG_NUM_BOARDS 1		// Configure globally
#define FEB_CAN_PINGPONG_BOARD_ID 0 		// Configure per board

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern uint32_t FEB_CAN_Tx_Mailbox;

bool FEB_CAN_PingPong_Transmit_Flag = false;
uint16_t FEB_CAN_PingPong_Recv_Count;
uint16_t FEB_CAN_PingPong_Transmit_Count;

struct feb_can_lib_pingpong_t can_rx;
struct feb_can_lib_pingpong_t can_tx;

uint8_t FEB_CAN_PingPong_Filter(CAN_HandleTypeDef *hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	CAN_FilterTypeDef filter_config;

	// Standard CAN - 2.0A - 11 bit
	filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = FEB_CAN_LIB_PINGPONG_FRAME_ID << 5;
	filter_config.FilterIdLow = 0;
	filter_config.FilterMaskIdHigh = 0xFFE0;
	filter_config.FilterMaskIdLow = 0;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 28;
	filter_bank++;

	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
		// Code Error - Shutdown
	}

	return filter_bank;
}


int feb_can_lib_pingpong_unpack(
    struct feb_can_lib_pingpong_t *dst_p,
    const uint8_t *src_p,
    size_t size);


void FEB_CAN_PingPong_Store_Msg(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[8]) {
	if (feb_can_lib_pingpong_unpack(&can_rx, FEB_CAN_Rx_Data, FEB_CAN_Rx_Header->DLC) == 0) {
		uint32_t next_CAN_id = (FEB_CAN_Rx_Header->StdId + 1) % FEB_CAN_PINGPONG_NUM_BOARDS;
		if (next_CAN_id == FEB_CAN_PINGPONG_BOARD_ID) {
			FEB_CAN_PingPong_Transmit_Flag = true;
			can_tx.count = can_rx.count + 1;
		}
	}

}

void FEB_CAN_PingPong_Transmit(void) {
	// Initialize Transmission Header
	FEB_CAN_Tx_Header.DLC = FEB_CAN_LIB_PINGPONG_LENGTH;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_LIB_PINGPONG_FRAME_ID;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Configure FEB_CAN_Tx_Data
	feb_can_lib_pingpong_pack(FEB_CAN_Tx_Data, &can_tx, FEB_CAN_LIB_PINGPONG_LENGTH);

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}
