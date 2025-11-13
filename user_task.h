#ifndef _USER_TASK_H
#define _USER_TASK_H

#include "main.h"

#define LED_GPIO_Port GPIOC
#define LED_Pin 13

void Task1(void *pvParameters)__attribute__((noreturn));

#endif
