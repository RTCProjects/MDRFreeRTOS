#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"

#include "MDR32Fx.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_port.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"

#include "task.h"




void vLed1(void *argument)
{
    while(1)
    {
        PORT_SetBits(MDR_PORTC, PORT_Pin_0);
        vTaskDelay(1000);
        PORT_ResetBits(MDR_PORTC, PORT_Pin_0);
        vTaskDelay(1000);
    }
}

void vLed2(void *argument)
{
    while(1)
    {
        PORT_SetBits(MDR_PORTC, PORT_Pin_1);
        vTaskDelay(1000);
        PORT_ResetBits(MDR_PORTC, PORT_Pin_1);
        vTaskDelay(1000);
    }	
}

void vApplicationIdleHook (void)
{

}

void Port_Init()
{
	PORT_InitTypeDef PORT_InitStructure;
	
	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);
		
		PORT_InitStructure.PORT_Pin   = PORT_Pin_0 | PORT_Pin_1;
    PORT_InitStructure.PORT_OE    = PORT_OE_OUT;
    PORT_InitStructure.PORT_FUNC  = PORT_FUNC_PORT;
    PORT_InitStructure.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure.PORT_SPEED = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTC, &PORT_InitStructure);
}

int main()
{
	Port_Init();
	
	xTaskCreate(vLed1, "1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(vLed2, "2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	
	vTaskStartScheduler();
}

