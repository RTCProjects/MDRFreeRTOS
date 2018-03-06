#include "can.h"

//static CAN_TxMsgTypeDef TxMsg;
static xQueueHandle xCanbusTxQueue;
static xQueueHandle xCanbusRxQueue;


void	CAN_vTxTask(void *argument);
void	CAN_vRxTask(void *argument);


void CAN_PerifInit()
{
	CAN_InitTypeDef  sCAN;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_CAN1,ENABLE);
	
	CAN_BRGInit(MDR_CAN1,CAN_HCLKdiv1);
	  
	CAN_StructInit (&sCAN);
	
	sCAN.CAN_ROP  = DISABLE;
  sCAN.CAN_SAP  = ENABLE;
  sCAN.CAN_STM  = DISABLE;
  sCAN.CAN_ROM  = DISABLE;
  sCAN.CAN_PSEG = CAN_PSEG_Mul_2TQ;
  sCAN.CAN_SEG1 = CAN_SEG1_Mul_7TQ;
  sCAN.CAN_SEG2 = CAN_SEG2_Mul_6TQ;
  sCAN.CAN_SJW  = CAN_SJW_Mul_4TQ;
  sCAN.CAN_SB   = CAN_SB_1_SAMPLE;
  sCAN.CAN_BRP  = 7;
  CAN_Init (MDR_CAN1,&sCAN);
  CAN_Cmd(MDR_CAN1, ENABLE);
	
	CAN_ITConfig(MDR_CAN1, CAN_IT_GLBINTEN | CAN_IT_RXINTEN, ENABLE);
	CAN_RxITConfig(MDR_CAN1, (1 << CAN_BUFFER_1), ENABLE);
	CAN_Receive(MDR_CAN1, CAN_BUFFER_1, ENABLE);
	
	NVIC_EnableIRQ(CAN1_IRQn);
	NVIC_SetPriority(CAN1_IRQn, 4);
	/*
		TxMsg.IDE     = CAN_ID_STD;
		TxMsg.DLC     = 0x08;
		TxMsg.PRIOR_0 = DISABLE;
		TxMsg.ID = (uint32_t)(0x320 << 18) & 0x1FFC0000;
		
		TxMsg.Data[0] = 0x12345678;
		TxMsg.Data[1] = 0x87654321;
		*/
		
		xTaskCreate(CAN_vTxTask, "1", configMINIMAL_STACK_SIZE + 0x100, NULL, tskIDLE_PRIORITY + 2, NULL);
		xTaskCreate(CAN_vRxTask, "1", configMINIMAL_STACK_SIZE + 0x100, NULL, tskIDLE_PRIORITY + 2, NULL);
		
		
		xCanbusTxQueue = xQueueCreate(CANBUS_TXD_QUEUE_LENGTH, sizeof(CAN_TxMsgTypeDef));
		xCanbusRxQueue = xQueueCreate(CANBUS_TXD_QUEUE_LENGTH, sizeof(CAN_RxMsgTypeDef));
		
}

void	CAN_SendMessage(CAN_TxMsgTypeDef	*pTxMessage)
{
	CAN_Transmit(MDR_CAN1, CAN_BUFFER_2, pTxMessage);
}

void	CAN_SendData(uint16_t	StdId,uint8_t 	*pData,uint16_t Size)
{
	uint16_t	dataSize = Size;
	
	static CAN_TxMsgTypeDef TxMessage;
												  TxMessage.IDE = CAN_ID_STD;
												  TxMessage.PRIOR_0 = DISABLE;
													TxMessage.ID = (uint32_t)(StdId << 18) & 0x1FFC0000;
	
	do{
		if(dataSize > 8)TxMessage.DLC = 8; else TxMessage.DLC = dataSize;
			//memcpy(TxMessage.Data,pData,sizeof(uint8_t) * TxMessage.DLC);
			memcpy(TxMessage.Data,pData,sizeof(uint8_t) * TxMessage.DLC);
		
			xQueueSendToBack(xCanbusTxQueue,&TxMessage,0);
		
		dataSize -= TxMessage.DLC ;
		pData += TxMessage.DLC ;
	}
	while(dataSize > 0);
}

void	CAN_ReceiveCallback(CAN_RxMsgTypeDef	*pRxMessage)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	
	xHigherPriorityTaskWoken = pdFALSE;
	xQueueSendToBackFromISR(xCanbusRxQueue, pRxMessage, &xHigherPriorityTaskWoken);
	
	if (xHigherPriorityTaskWoken != pdFALSE) {
		// Макрос, выполняющий переключение контекста.
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }
}

void	CAN_vTxTask(void *argument)
{
	portBASE_TYPE 		xStatus;
	CAN_TxMsgTypeDef 	TxMessage;
	
	while(1){
			xStatus=xQueueReceive(xCanbusTxQueue, &TxMessage, portMAX_DELAY);
			
			if (xStatus == pdPASS) {
				
				while( (CAN_GetBufferStatus(MDR_CAN1,CAN_BUFFER_2) & CAN_STATUS_TX_REQ)!=0);
				CAN_SendMessage(&TxMessage);
			}
		}
}

void	CAN_vRxTask(void *argument)
{
	portBASE_TYPE xStatus;
	CAN_TxMsgTypeDef 	RxMessage;
	
	while(1){
		xStatus=xQueueReceive(xCanbusRxQueue, &RxMessage, portMAX_DELAY);
		
		if (xStatus == pdPASS) {
			uint16_t	StdId = (RxMessage.ID >> 18) & 0x7FF;
			
 			if(StdId == 0x120)
			{
				int	sPackSize = 0;
				uint8_t	*pData = 0;
				
				for(uint8_t i = 0;i<RxMessage.DLC;i++){
					/*pData = protocol_GetPackageFromInputStream(rxCanBuffer,CAN_RX_BUFSIZE,&rxCanBufferInd,RxMessage.Data[i],&sPackSize);
					
					if(pData){
						rxCanBufferInd = 0;
						Protocol_PackageAnalysis(pData[1],pData[2],pData + 7);
					}*/
				}
			}
		}
	}
}
void CAN1_IRQHandler(void)
{
	CAN_RxMsgTypeDef RxMessage; 
	CAN_GetRawReceivedData(MDR_CAN1, CAN_BUFFER_1, &RxMessage);
	
	CAN_ReceiveCallback(&RxMessage);
	
	CAN_ITClearRxTxPendingBit(MDR_CAN1, CAN_BUFFER_1, CAN_STATUS_RX_READY);
}
