#ifndef _7SEG_H
#define _7SEG_H

#include "main.h"

#define DS_GPIO_Port GPIOB
#define SH_CP_GPIO_Port GPIOB
#define ST_CP_GPIO_Port GPIOB

#define DS_Pin GPIO_Pin_12
#define SH_CP_Pin GPIO_Pin_13
#define ST_CP_Pin GPIO_Pin_14

#define DS(x) GPIO_WriteBit(DS_GPIO_Port,DS_Pin,(x)?(Bit_SET):(Bit_RESET))
#define SH_CP(x) GPIO_WriteBit(SH_CP_GPIO_Port,SH_CP_Pin,(x)?(Bit_SET):(Bit_RESET))
#define ST_CP(x) GPIO_WriteBit(ST_CP_GPIO_Port,ST_CP_Pin,(x)?(Bit_SET):(Bit_RESET))

#define XUNG_DICH() {SH_CP(1);SH_CP(0);}
#define XUNG_CHOT() {ST_CP(1);ST_CP(0);}

extern uint8_t led_7dq[];

typedef struct
{
	uint8_t output_state;
	uint8_t gio;
	uint8_t phut;
	uint8_t giay;
}HC595_Handle_t;

extern HC595_Handle_t hc595_handle;


void xuat_1_byte(uint8_t x);
void xuat_8led_7doan(uint8_t cot_hthi,uint8_t so_hthi);
void giai_ma_quet(void);

void TaskHienThi(void *pvParameters)__attribute__((noreturn));

#endif

