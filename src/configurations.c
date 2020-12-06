/*
 * configurations.c
 *
 *  Created on: Oct 27, 2020
 *      Author: Saldory
 */

#include "configurations.h"


/**
 * TypeDef Declarations
 */
typedef enum { false, true } bool;


/**
 *  Static functions Declarations
 */
static void RCC_Config_HSE_PLL_Max(void);
static void RCC_APB_Clocks(void);
static void GPIO_ConfigurationPorts(void);
static void USART_CONFIG(void);
static void configInterruptUSART(void);
static void configurationTimer(void);
static void configurationInterruptTim3(void);
static void printClockInfo(void);

/**
 *  Extern Variables
 */
extern volatile int iCounter100mSec;
extern volatile int iCounter30mSec;
extern int bCounter100mSec;
extern int bCounter30mSec;
extern int bI2C_EVENT_MASTER_BYTE_RECEIVED;
extern int bI2C_EVENT_MASTER_BYTE_TRANSMITTED;
extern int bI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED;
extern int bI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED;

/** Declaration of PUBLIC Interfaces **/
void configuration(void)
{
	RCC_Config_HSE_PLL_Max();
	RCC_APB_Clocks();
	GPIO_ConfigurationPorts();
	USART_CONFIG();
	configInterruptUSART();
	configurationTimer();
	configurationInterruptTim3();
	Sensor_Init();
	printClockInfo();
}


void OutString(USART_TypeDef *USARTx,char *s)
{
  while(*s)
  {
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); // Wait for Empty

    USART_SendData(USARTx, *s++); // Send Char
  }
}


/** Declaration of STATIC LOCAL FUNCTIONS **/

/**
 *  Configurations of Clock
 */
static void RCC_Config_HSE_PLL_Max(void)
{
		/*
		 * Depois de testar com o PLL usando o HSE
		 * A maxima Frecuencia obtida e de 72Mhz.
		 * Fmax = (HSE)*Pll --> Fmax = 12*6 = 72Mhz
		 */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
		RCC_HSEConfig(RCC_HSE_ON);
		while(RCC_GetSYSCLKSource()!=0x04);

		ErrorStatus HSEStartUpStatus;
		HSEStartUpStatus=RCC_WaitForHSEStartUp();
		if(HSEStartUpStatus==SUCCESS)
		{
			FLASH_SetLatency(FLASH_Latency_2);
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
			RCC_PLLCmd(ENABLE);
			while(RCC_GetSYSCLKSource()!=0x08);
		}
		else
		while(1);
}

static void RCC_APB_Clocks(void)
{
	/* ABP1 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); // Enable TIM3 to RCC. APB1.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // Enable USART2 to RCC.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // Enable USART3 to RCC.
	//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); // Enable TIM4 to RCC.

	/* ABP2 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); // Enable RCC to GPIOB
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); // Enable RCC to GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); // Enable RCC to GPIOA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}


/**
 *  PinOut Configurations I/O
 */
static void GPIO_ConfigurationPorts(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*
	 *  Used for USART2
	 *  PA2(Tx) && PA3(Rx) (Defaults Mapping)
	 *  */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*
	 *  Used for USART3
	 *  PB10(Tx) && PB11(Rx) (Defaults Mapping)
	 *  */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	/* Led Placa Nucleo STM32F103RB */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

}


/**
 *  USART Configuration
 */
static void USART_CONFIG(void)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);

	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);

}

/**
 *  Interrups Configurations
 */

static void configInterruptUSART(void)
{
	NVIC_InitTypeDef NVIC_InitStructure1;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure1.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure1);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure1.NVIC_IRQChannel = USART2_IRQn;
	NVIC_Init(&NVIC_InitStructure1);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}


static void configurationTimer(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	// para 2HZ   --> ARR=20.10^3 PSC= 1790
	// para 1khz  --> ARR=10.10^3 PSC=5
	//TIM_TimeBaseStructure.TIM_Period = 20000; //auto-reload 0 hasta 65535
	TIM_TimeBaseStructure.TIM_Period = 10000; //auto-reload 0 hasta 65535
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Prescaler = 5; //prescaler de 0 hasta 65535 TIM
	//TIM_TimeBaseStructure.TIM_Prescaler = 1790; //prescaler de 0 hasta 65535 TIM
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);

//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure1;
//	TIM_TimeBaseStructure1.TIM_Period = 255; //auto-reload 0 hasta 65535
//	TIM_TimeBaseStructure1.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseStructure1.TIM_Prescaler = 281; //prescaler de 0 hasta 65535 TIM
//	TIM_TimeBaseStructure1.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure1);
//	TIM_Cmd(TIM4, ENABLE);


}


static void configurationInterruptTim3(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
}

static void printClockInfo(void)
{
	char buffer_lcd[32];
	uint8_t clock_source;
	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);

	/*print them to LCD*/
	sprintf(buffer_lcd, "SYSCLK %d MHz \r\n", (int) RCC_Clocks.SYSCLK_Frequency/1000000 );
	OutString(USART2,&buffer_lcd);
	sprintf(buffer_lcd, "HCLK %d MHz \r\n", (int) RCC_Clocks.HCLK_Frequency /1000000 );
	OutString(USART2,&buffer_lcd);
	sprintf(buffer_lcd, "PCLK1 %d MHz \r\n", (int) RCC_Clocks.PCLK1_Frequency /1000000 );
	OutString(USART2,&buffer_lcd);
	sprintf(buffer_lcd, "PCLK2 %d MHz \r\n", (int) RCC_Clocks.PCLK2_Frequency /1000000 );
	OutString(USART2,&buffer_lcd);

	clock_source = RCC_GetSYSCLKSource();
	if(clock_source == 0x00)
		sprintf(buffer_lcd, "Using HSI. \r\n");
	else if(clock_source == 0x04)
		sprintf(buffer_lcd, "Using HSE. \r\n");
	else if(clock_source == 0x08)
		sprintf(buffer_lcd, "Using PLL. \r\n");

	OutString(USART2,&buffer_lcd);
}


void delay100mSec(void)
{
	bCounter100mSec = FALSE;
	iCounter100mSec = 0;

	while(FALSE == bCounter100mSec);

}
