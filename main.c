#include "MDR32Fx.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

void vLed1(void *argument)
{
    while(1)
    {
        /*PORT_SetBits(MDR_PORTB, PORT_Pin_0);
        vTaskDelay(1000);
        PORT_ResetBits(MDR_PORTB, PORT_Pin_0);
        vTaskDelay(1000);*/
    }
}

void vApplicationIdleHook (void)
{

}

int main()
{
	xTaskCreate(vLed1, "1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	
	vTaskStartScheduler();
}

