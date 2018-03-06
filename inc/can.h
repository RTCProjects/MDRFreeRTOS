#ifndef _CAN_H
#define _CAN_H

#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"

#include "MDR32Fx.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_can.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <string.h>

#define CANBUS_TXD_QUEUE_LENGTH	32
#define CANBUS_RXD_QUEUE_LENGTH	32

void	CAN_PerifInit(void);
void	CAN_SendMessage(CAN_TxMsgTypeDef	*pTxMessage);
void	CAN_SendData(uint16_t	StdId,uint8_t 	*pData,uint16_t Size);
void	CAN_ReceiveCallback(CAN_RxMsgTypeDef	*pRxMessage);
#endif
