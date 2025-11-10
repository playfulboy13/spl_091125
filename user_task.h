#ifndef _USER_TASK_H
#define _USER_TASK_H

#include "main.h"

#define LED_GPIO_Port GPIOB
#define LED_Pin 2

void Task1(void *pvParameters)__attribute__((noreturn));

#endif
