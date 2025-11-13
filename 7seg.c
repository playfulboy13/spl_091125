#include "7seg.h"

uint8_t led_7dq[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
const uint8_t ma7doan[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

HC595_Handle_t hc595_handle;

static uint8_t tt8led;

void xuat_1_byte(uint8_t x)
{
	for(uint8_t i=0;i<8;i++)
	{
		(x&0x80)?(DS(1)):(DS(0));
		x<<=1;
		XUNG_DICH()
	}
}

void xuat_8led_7doan(uint8_t cot_hthi,uint8_t so_hthi)
{
	xuat_1_byte(hc595_handle.output_state);
	xuat_1_byte((uint8_t)~(1<<cot_hthi));
	xuat_1_byte(so_hthi);
	XUNG_CHOT()
}

void giai_ma_quet(void)
{
	led_7dq[0]=ma7doan[0];
	led_7dq[1]=ma7doan[1];
	led_7dq[2]=ma7doan[2];
	led_7dq[3]=ma7doan[3];
	led_7dq[4]=ma7doan[4];
	led_7dq[5]=ma7doan[5];
	led_7dq[6]=ma7doan[6];
	led_7dq[7]=ma7doan[7];
}

void TaskHienThi(void *pvParameters)
{
	(void)*pvParameters;
	while(1)
	{
		giai_ma_quet();
		xuat_8led_7doan(tt8led,led_7dq[tt8led]);
		tt8led=(tt8led+1)%8;
		
		vTaskDelay(pdMS_TO_TICKS(5));
	}
}
