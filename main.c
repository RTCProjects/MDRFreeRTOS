#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"

#include "MDR32Fx.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"

#include "task.h"
#include "can.h"



void vLed1(void *argument)
{
	
	
    while(1)
    {
      /*  PORT_SetBits(MDR_PORTC, PORT_Pin_0);
        vTaskDelay(100);
        PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
        vTaskDelay(100);
			*/
			PORT_SetBits(MDR_PORTC, PORT_Pin_0);
			CAN_SendData(0x321,0,2048);
			PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
			vTaskDelay(1000);
			//CAN_Transmit(MDR_CAN1, CAN_BUFFER_2, &TxMsg);
    }
}

void vLed2(void *argument)
{
    while(1)
    {
        PORT_SetBits(MDR_PORTC, PORT_Pin_1);
        vTaskDelay(100);
        PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
        vTaskDelay(100);
    }	
}

void vApplicationIdleHook (void)
{
	
}

void Port_Init()
{
	PORT_InitTypeDef PORT_InitStructure;
		
		//Ports for LED
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
		
		PORT_InitStructure.PORT_Pin   = PORT_Pin_0 | PORT_Pin_1;
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
    PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
    PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
		
	
		//Ports for CAN
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTA, ENABLE);
	
		PORT_InitStructure.PORT_FUNC  = PORT_FUNC_ALTER;
		PORT_InitStructure.PORT_Pin   = PORT_Pin_6;
		PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
		PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
		PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
		PORT_InitStructure.PORT_PD = PORT_PD_DRIVER;
		PORT_Init(MDR_PORTA, &PORT_InitStructure);

		PORT_InitStructure.PORT_Pin   = PORT_Pin_7;
		PORT_InitStructure.PORT_OE    = PORT_OE_IN;
		PORT_Init(MDR_PORTA, &PORT_InitStructure);
			
}

void Clock_Init()
{
	RST_CLK_DeInit();
	RST_CLK_PCLKcmd(RST_CLK_PCLK_BKP,ENABLE);
	
	RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
	if (RST_CLK_HSEstatus() == SUCCESS){
		RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, RST_CLK_CPU_PLLmul8);
		RST_CLK_CPU_PLLcmd(ENABLE);
		
			if (RST_CLK_HSEstatus() == SUCCESS){
        RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);  
        RST_CLK_CPU_PLLuse(ENABLE);
        RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
      }
	}
}



int main()
{
	Clock_Init();
	Port_Init();
	CAN_PerifInit();
	
	xTaskCreate(vLed1, "1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(vLed2, "2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	
	vTaskStartScheduler();
	
}

