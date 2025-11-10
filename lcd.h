#ifndef _LCD_H
#define _LCD_H

#include "main.h"

#include "stm32f10x_i2c.h"

#include "adc.h"

#define LCD_ADDR 0X4E
#define LCD_BACKLIGHT (1<<3)

extern uint8_t LCD_BackLightState;

typedef struct
{
	I2C_TypeDef* i2c_port;
	SemaphoreHandle_t i2c_mutex;
}I2C_Handle_t;

extern I2C_Handle_t i2c_handle;

void I2C_Init_Config(I2C_Handle_t* i2c1_handle);
void I2C_Config(void);
void LCD_Write(I2C_Handle_t* i2c1_handle,uint8_t data);
void LCD_SendCommand(I2C_Handle_t* i2c1_handle,uint8_t cmd);
void LCD_SendData(I2C_Handle_t* i2c1_handle,uint8_t data);
void LCD_Init(void);
void LCD_SendString(I2C_Handle_t* i2c1_handle,char *str);
void LCD_SetCursor(I2C_Handle_t* i2c1_handle,uint8_t row,uint8_t col);
void LCD_SetBackLight(uint8_t state);
void TaskLcd(void *pvParameters)__attribute__((noreturn));

#endif
