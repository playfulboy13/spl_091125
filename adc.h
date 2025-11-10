#ifndef _ADC_H
#define _ADC_H

#include "main.h"

typedef struct
{
	uint16_t adcValues[3];
	uint16_t adcAvaerage[3];
	float temp1;
	float temp2;
	float temp3;
}ADC_Value_t;

extern ADC_Value_t adc_value;

void ADC_Init_Config(void);
void TaskAdc(void *pvParameters)__attribute__((noreturn));

#endif