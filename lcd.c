#include "lcd.h"

uint8_t LCD_BackLightState=LCD_BACKLIGHT;
I2C_Handle_t i2c_handle;

void I2C_Init_Config(I2C_Handle_t* i2c1_handle)
{
	if(i2c1_handle->i2c_port==I2C1)
	{
		GPIO_Config_t i2cPins={GPIOB,GPIO_Pin_6|GPIO_Pin_7,GPIO_Mode_AF_OD,GPIO_Speed_2MHz};
		GPIO_Init_Config(&i2cPins);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	}
	
	I2C_InitTypeDef I2C_InitStructure;
	
	I2C_InitStructure.I2C_ClockSpeed=100000;
	I2C_InitStructure.I2C_Mode=I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
	I2C_InitStructure.I2C_Ack=I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_OwnAddress1=0X00;
	I2C_Init(i2c1_handle->i2c_port,&I2C_InitStructure);
	I2C_Cmd(i2c1_handle->i2c_port,ENABLE);
	
}

void I2C_Config(void)
{
	i2c_handle.i2c_port=I2C1;
	i2c_handle.i2c_mutex=xSemaphoreCreateMutex();
	
	I2C_Init_Config(&i2c_handle);
	
}

void LCD_Write(I2C_Handle_t* i2c1_handle,uint8_t data)
{
	data|=LCD_BackLightState;
	if(xSemaphoreTake(i2c1_handle->i2c_mutex,portMAX_DELAY)==pdTRUE)
	{
		while(I2C_GetFlagStatus(i2c1_handle->i2c_port,I2C_FLAG_BUSY));
		I2C_GenerateSTART(i2c1_handle->i2c_port,ENABLE);
		while(!I2C_CheckEvent(i2c1_handle->i2c_port,I2C_EVENT_MASTER_MODE_SELECT));
		I2C_Send7bitAddress(i2c1_handle->i2c_port,LCD_ADDR,I2C_Direction_Transmitter);
		while(!(I2C_CheckEvent(i2c1_handle->i2c_port,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
		I2C_SendData(i2c1_handle->i2c_port,data);
		while(!(I2C_CheckEvent(i2c1_handle->i2c_port,I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
		I2C_GenerateSTOP(i2c1_handle->i2c_port,ENABLE);
		xSemaphoreGive(i2c1_handle->i2c_mutex);
	}
}

void LCD_SendCommand(I2C_Handle_t* i2c1_handle,uint8_t cmd)
{
	uint8_t data=(cmd&0XF0);
	LCD_Write(i2c1_handle,data|0x04);
	vTaskDelay(pdMS_TO_TICKS(1));
	LCD_Write(i2c1_handle,data);
	
	data=(cmd<<4)&0XF0;
	LCD_Write(i2c1_handle,data|0x04);
	vTaskDelay(pdMS_TO_TICKS(1));
	LCD_Write(i2c1_handle,data);
	
}

void LCD_SendData(I2C_Handle_t* i2c1_handle,uint8_t data)
{
	uint8_t data_h,data_l;
	
	data_h=(data&0XF0);
	LCD_Write(i2c1_handle,data_h|0x05);
	vTaskDelay(pdMS_TO_TICKS(1));
	LCD_Write(i2c1_handle,data_h|0x01);
	
	data_l=(data<<4)&0XF0;
	LCD_Write(i2c1_handle,data_l|0x05);
	vTaskDelay(pdMS_TO_TICKS(1));
	LCD_Write(i2c1_handle,data_l|0x01);
}

void LCD_Init(void)
{
	vTaskDelay(pdMS_TO_TICKS(50));
	LCD_SendCommand(&i2c_handle,0x02); 
	LCD_SendCommand(&i2c_handle,0x28);
	LCD_SendCommand(&i2c_handle,0x0c);
	LCD_SendCommand(&i2c_handle,0x06);
	LCD_SendCommand(&i2c_handle,0x01);
	vTaskDelay(pdMS_TO_TICKS(5));
}

void LCD_SendString(I2C_Handle_t* i2c1_handle,char *str)
{
	while(*str)
	{
		LCD_SendData(i2c1_handle,*str++);
	}
}

void LCD_SetCursor(I2C_Handle_t* i2c1_handle,uint8_t row,uint8_t col)
{
	uint8_t address;
	
	switch(row)
	{
		case 0:
		{
			address=col; break;
		}
		case 1:
		{
			address=col+0x40; break;
		}
		case 2: 
		{
			address=col+0x14; break;
		}
		case 3:
		{
			address=col+0x54; break;
		}
		default: return;
	}
	LCD_SendCommand(i2c1_handle,address|0x80);
}

void LCD_SetBackLight(uint8_t state)
{
	(state)?(LCD_BackLightState=LCD_BACKLIGHT):(LCD_BackLightState=0x00);
}

void TaskLcd(void *pvParameters)
{
	(void)*pvParameters;
	
	LCD_Init();
	LCD_SetBackLight(1);
	
	char line1Str[17];
	char line2Str[17];
	
	uint8_t count=0;
	
	while(1)
	{
		snprintf(line1Str,sizeof(line1Str),"HELLO STM32");
		snprintf(line2Str,sizeof(line2Str),"Count: %d%d  ",count/10,count%10);
		LCD_SetCursor(&i2c_handle,0,0);
		LCD_SendString(&i2c_handle,line1Str);
		LCD_SetCursor(&i2c_handle,1,0);
		LCD_SendString(&i2c_handle,line2Str);
		(count>99)?(count=0):(count++);
		
		
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


