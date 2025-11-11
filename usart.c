#include "usart.h"

USART_Handle_t usart_handle;

void USART_Init_Config(USART_Handle_t* usart1_handle)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(usart1_handle->usart_port==USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9; //TX PIN
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10; //RX PIN
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	}
	
	USART_InitTypeDef USART_InitStructure;
	
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(usart1_handle->usart_port,&USART_InitStructure);
	USART_Cmd(usart1_handle->usart_port,ENABLE);
	
}

void USART1_Config(void)
{
	usart_handle.usart_port=USART1;
	usart_handle.usart_mutex=xSemaphoreCreateMutex();
	
	USART_Init_Config(&usart_handle);
	
}

void USART_SendChar(USART_Handle_t* usart1_handle,char str)
{
	if(xSemaphoreTake(usart1_handle->usart_mutex,portMAX_DELAY)==pdTRUE)
	{
		while(USART_GetFlagStatus(usart1_handle->usart_port,USART_FLAG_TXE)==RESET);
		USART_SendData(usart1_handle->usart_port,str);
		xSemaphoreGive(usart1_handle->usart_mutex);
	}
}

void USART_Send(USART_Handle_t* usart1_handle,char *str)
{
	while(*str)
	{
		USART_SendChar(usart1_handle,*str++);
	}
}

void TaskUsart1Send(void *pvParameters)
{
	(void)*pvParameters;
	char buffer[64];
	
	while(1)
	{
		snprintf(buffer,sizeof(buffer),"ADC1: %04d \n ADC2: %04d \n ADC3: %04d\n",adc_value.adcValues[0],
			adc_value.adcValues[1],adc_value.adcValues[2]);
		USART_Send(&usart_handle,buffer);
		
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
