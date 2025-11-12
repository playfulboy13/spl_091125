#include "usart.h"

USART_Handle_t usart_handle;

bool tx_done=false;

void USART_Init_Config(USART_Handle_t* usart1_handle)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(usart1_handle->usart_port==USART1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_USART1,ENABLE);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
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
	
	// DMA cho USART1_TX = DMA1_Channel4
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(DMA1_Channel4);
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr=0;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize=0;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority=DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4,&DMA_InitStructure);
	
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	USART_Cmd(usart1_handle->usart_port,ENABLE);
	
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	NVIC_EnableIRQ(DMA1_Channel4_IRQn);
	NVIC_SetPriority(DMA1_Channel4_IRQn,0);

	tx_done = true; // ?
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

void USART_Send_DMA(USART_Handle_t* usart1_handle, char *str)
{
    size_t len = strlen(str);
    if (xSemaphoreTake(usart1_handle->usart_mutex, portMAX_DELAY) == pdTRUE)
    {
        while (!tx_done); // ch? l?n g?i tru?c hoàn thành
        tx_done = false;

        DMA_Cmd(DMA1_Channel4, DISABLE);
        DMA_ClearFlag(DMA1_FLAG_TC4); // ? clear flag

        DMA1_Channel4->CMAR = (uint32_t)str;
        DMA1_Channel4->CNDTR = len;

        DMA_Cmd(DMA1_Channel4, ENABLE); // b?t DMA, b?t d?u g?i

        xSemaphoreGive(usart1_handle->usart_mutex);
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
		USART_Send_DMA(&usart_handle,buffer);
		
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


//IRQ

void DMA1_Channel4_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC4) != RESET)
    {
        DMA_ClearITPendingBit(DMA1_IT_TC4); // r?t quan tr?ng, ph?i xóa c?
        DMA_Cmd(DMA1_Channel4, DISABLE);    // t?t DMA sau khi truy?n xong
        tx_done = true;
    }
}



