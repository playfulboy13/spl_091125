#include "user_task.h"

void Task1(void *pvParameters)
{
	(void)*pvParameters;
	while(1)
	{
		GPIOC->ODR^=(1<<13);
		vTaskDelay(pdMS_TO_TICKS(500));
	}
}
