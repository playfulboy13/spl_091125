#include "user_task.h"

void Task1(void *pvParameters)
{
	(void)*pvParameters;
	while(1)
	{
		GPIOB->ODR^=(1<<2);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
