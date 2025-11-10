#include "adc.h"

ADC_Value_t adc_value;

void ADC_Init_Config(void)
{
	GPIO_Config_t adcPins={GPIOA,GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3,GPIO_Mode_AIN,GPIO_Speed_50MHz};
	GPIO_Init_Config(&adcPins);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitTypeDef DMA_InitStructure;
	
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr=(uint32_t)adc_value.adcValues;
	DMA_InitStructure.DMA_DIR=DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize=3;
	DMA_InitStructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode=DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority=DMA_Priority_Low;
	DMA_InitStructure.DMA_M2M=DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	ADC_InitTypeDef ADC_InitStructure;
	
	ADC_InitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode=ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode=ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel=3;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,2,ADC_SampleTime_71Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,3,ADC_SampleTime_71Cycles5);
	
	ADC_DMACmd(ADC1,ENABLE);
	ADC_Cmd(ADC1,ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	
}

void TaskAdc(void *pvParameters)
{
	(void)*pvParameters;
	 ADC_Init_Config();
	while(1)
	{
		adc_value.temp1=(float)adc_value.adcValues[0];
		adc_value.temp2=(float)adc_value.adcValues[1];
		adc_value.temp3=(float)adc_value.adcValues[2];
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

