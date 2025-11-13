#ifndef _USART_H
#define _USART_H

#include "main.h"

#include "stm32f10x_usart.h"

#include "adc.h"

#define RX_MAX_BUFFERSIZE 64

typedef struct
{
	USART_TypeDef* usart_port;
	SemaphoreHandle_t usart_mutex;
	QueueHandle_t usart_queue;
	
}USART_Handle_t;


typedef struct
{
	uint8_t rxChar;
	uint8_t rxIndex;
}USART_Data_t;

extern USART_Data_t usart_data;

extern USART_Handle_t usart_handle;

extern bool tx_done;


void USART_Init_Config(USART_Handle_t* usart1_handle);
void USART1_Config(void);
void USART_SendChar(USART_Handle_t* usart1_handle,char str);
void USART_Send(USART_Handle_t* usart1_handle,char* str);
void USART_Send_DMA(USART_Handle_t* usart1_handle,char* str);
void DMA1_Channel4_IRQHandler(void);
void USART1_IRQHandler(void);

void TaskUsart1Send(void *pvParameters)__attribute__((noreturn));
void TaskUsart1Receive(void *pvParameters)__attribute__((noreturn));

#endif
