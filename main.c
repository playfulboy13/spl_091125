#include "main.h"

int main(void)
{
	SystemClock_Config();
	GPIO_Config();
	
	xTaskCreate(Task1,"Task1",128,NULL,1,NULL);
	vTaskStartScheduler();
	
	while(1);
}

void SystemClock_Config(void)
{
	RCC->CR|=RCC_CR_HSEON;
	while(!(RCC->CR&(RCC_CR_HSERDY)));
	FLASH->ACR=(FLASH_ACR_PRFTBE|FLASH_ACR_LATENCY_2);
	RCC->CFGR&=~(uint32_t)(RCC_CFGR_PLLSRC|RCC_CFGR_PLLXTPRE|RCC_CFGR_PLLMULL);
	RCC->CFGR|=(RCC_CFGR_PLLSRC|RCC_CFGR_PLLMULL9);
	RCC->CR|=RCC_CR_PLLON;
	while(!(RCC->CR&RCC_CR_PLLRDY));
	RCC->CFGR&=~(uint32_t)RCC_CFGR_SW;
	RCC->CFGR|=RCC_CFGR_SW_PLL;
	while((RCC->CFGR&(RCC_CFGR_SWS))!=RCC_CFGR_SWS_PLL);
	RCC->CFGR&=~(uint32_t)(RCC_CFGR_HPRE|RCC_CFGR_PPRE1|RCC_CFGR_PPRE2);
	RCC->CFGR|=(RCC_CFGR_HPRE_DIV1|RCC_CFGR_PPRE1_DIV2|RCC_CFGR_PPRE2_DIV1);
	SystemCoreClock=72000000;
}

void GPIO_Init_Config(GPIO_Config_t* config)
{
	if(config->GPIO_Port==GPIOA)
	{
		RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	}
	else if(config->GPIO_Port==GPIOB)
	{
		RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	}
	else if(config->GPIO_Port==GPIOC)
	{
		RCC->APB2ENR|=RCC_APB2ENR_IOPCEN;
	}
	uint32_t pinPos;
	for(pinPos=0;pinPos<16;pinPos++)
	{
		if(config->GPIO_Pin&(1<<pinPos))
		{
			volatile uint32_t *CR=(pinPos<8)?&(config->GPIO_Port->CRL):&(config->GPIO_Port->CRH);
			uint8_t shift=(pinPos%8)*4;
			uint32_t mode_bits=0;
			*CR&=~(uint32_t)(0xF<<shift);
			if(config->GPIO_Mode==GPIO_Mode_Out_PP)
			{
				mode_bits=(uint32_t)(config->GPIO_Speed<<0)|(0X0<<2);
			}
			else if(config->GPIO_Mode==GPIO_Mode_Out_OD)
			{
				mode_bits=(uint32_t)(config->GPIO_Speed<<0)|(0x1<<2);
			}
			else if(config->GPIO_Mode==GPIO_Mode_AF_PP)
			{
				mode_bits=(uint32_t)(config->GPIO_Speed<<0)|(0X2<<2);
			}
			else if(config->GPIO_Mode==GPIO_Mode_AF_OD)
			{
				mode_bits=(uint32_t)(config->GPIO_Speed<<0)|(0x3<<2);
			}
			else if(config->GPIO_Mode==GPIO_Mode_IN_FLOATING)
			{
				mode_bits=0x4;
			}
			else if(config->GPIO_Mode==GPIO_Mode_IPU)
			{
				mode_bits=0x8;
				config->GPIO_Port->ODR|=(1<<pinPos);
			}
			else if(config->GPIO_Mode==GPIO_Mode_IPD)
			{
				mode_bits=0x8;
				config->GPIO_Port->ODR&=~(uint32_t)(1<<pinPos);
			}
			else if(config->GPIO_Mode==GPIO_Mode_AIN)
			{
				mode_bits=0x0;
			}
			*CR|=(mode_bits<<shift);
		}
	}
}

void GPIO_Config(void)
{
	GPIO_Config_t ledPin={GPIOB,GPIO_Pin_2,GPIO_Mode_Out_PP,GPIO_Speed_50MHz};
	GPIO_Init_Config(&ledPin);
	
	
}

