/*
 * configurations.c
 *
 *  Created on: Oct 27, 2020
 *      Author: Saldory
 */

#include "configurations.h"


/**
 *  Static functions Declarations
 */
static void RCC_Config_HSE_PLL_Max(void);
static void RCC_APB_Clocks(void);
static void GPIO_ConfigurationPorts(void);
static void USART_CONFIG(void);
static void configInterruptUSART(void);


/** Declaration of PUBLIC Interfaces **/
void configuration(void)
{
	RCC_Config_HSE_PLL_Max();
	RCC_APB_Clocks();
	GPIO_ConfigurationPorts();
	USART_CONFIG();
	configInterruptUSART();
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
}

