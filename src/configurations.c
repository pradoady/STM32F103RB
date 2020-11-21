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
	I2C_CONFIG();
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

void I2C_CONFIG(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	I2C_DeInit(I2Cx);

	RCC_APB1PeriphClockCmd(I2Cx_RCC, ENABLE);

	I2C_InitStruct.I2C_ClockSpeed = 100000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2Cx, &I2C_InitStruct);
	I2C_Cmd(I2Cx, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);

	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

}

void i2c_start()
{
	iCounter30mSec = 0;
	bCounter30mSec = FALSE;

    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
    	if (TRUE == bCounter30mSec)
    		i2c_unlockBusyFlag();
    }

    // Generate start condition
    I2C_GenerateSTART(I2Cx, ENABLE);

    // Wait for I2C EV5.
    // It means that the start condition has been correctly released
    // on the I2C bus (the bus is free, no other devices is communicating))
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));
}

void i2c_stop()
{
    // Generate I2C stop condition
    I2C_GenerateSTOP(I2Cx, ENABLE);
    // Wait until I2C stop condition is finished
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_STOPF));
}

void i2c_address_direction(uint8_t address, uint8_t direction)
{


    // Send slave address
    I2C_Send7bitAddress(I2Cx, address, direction);

    // Wait for I2C EV6
    // It means that a slave acknowledges his address
    if (direction == I2C_Direction_Transmitter)
    {
    	iCounter30mSec = 0;
		bCounter30mSec = FALSE;

        while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
        	if (TRUE == bCounter30mSec)
        	{
        		bI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED = TRUE;
        		break;
        	}

		}
    }
    else if (direction == I2C_Direction_Receiver)
    {
    	iCounter30mSec = 0;
		bCounter30mSec = FALSE;

        while (!I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        {
        	if (TRUE == bCounter30mSec)
        	{
				bI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED = TRUE;
				break;
			}
        }
    }
}

void i2c_transmit(uint8_t byte)
{
	int i=0;
    // Send data byte
    I2C_SendData(I2Cx, byte);
    // Wait for I2C EV8_2.
    // It means that the data has been physically shifted out and
    // output on the bus)
    iCounter30mSec = 0;
	bCounter30mSec = FALSE;

    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
    	if (TRUE == bCounter30mSec)
    	{
    		//if (SET == I2C_GetFlagStatus(I2Cx,I2C_FLAG_AF))
    		{
    			I2C_GenerateSTOP(I2Cx,ENABLE);
    			bI2C_EVENT_MASTER_BYTE_TRANSMITTED = TRUE;
				break;
    		}

		}
	}
}

uint8_t i2c_receive_ack()
{
	int i=0;
	// Enable ACK of received data
    I2C_AcknowledgeConfig(I2Cx, ENABLE);
    // Wait for I2C EV7
    // It means that the data has been received in I2C data register

    iCounter30mSec = 0;
	bCounter30mSec = FALSE;

    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
    {
    	if (TRUE == bCounter30mSec)
    	{
    		bI2C_EVENT_MASTER_BYTE_RECEIVED = TRUE;
    		break;
    	}

    }

    // Read and return data byte from I2C data register
    return I2C_ReceiveData(I2Cx);
}

uint8_t i2c_receive_nack()
{
    // Disable ACK of received data
    I2C_AcknowledgeConfig(I2Cx, DISABLE);
    // Wait for I2C EV7
    // It means that the data has been received in I2C data register
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

    // Read and return data byte from I2C data register
    return I2C_ReceiveData(I2Cx);
}

void i2c_write(uint8_t address, uint8_t data)
{
    i2c_start();
    i2c_address_direction(address << 1, I2C_Direction_Transmitter);
    i2c_transmit(data);
    i2c_stop();
}

void i2c_read(uint8_t address, uint8_t* data)
{
    i2c_start();
    i2c_address_direction((address << 1)|1, I2C_Direction_Receiver);
    *data = i2c_receive_ack();
    data++;
    *data = i2c_receive_ack();
    i2c_stop();
}


void i2c_unlockBusyFlag(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	I2C_Cmd(I2Cx, DISABLE);

	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

	GPIO_Write(I2C_GPIO,HighLvl_SDA_SCL_I2C);

	if (Bit_SET == GPIO_ReadInputDataBit(I2C_GPIO,I2C_PIN_SCL) )
	{
		//if (Bit_SET == GPIO_ReadInputDataBit(I2C_GPIO,I2C_PIN_SDA))
		{
			GPIO_InitStruct.GPIO_Pin = I2C_PIN_SDA;
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
			GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

			GPIO_Write(I2C_GPIO,LowLvl_SDA_I2C);

			if (Bit_RESET == GPIO_ReadInputDataBit(I2C_GPIO,I2C_PIN_SDA))
			{
				GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL;
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
				GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

				GPIO_Write(I2C_GPIO,LowLvl_SCL_I2C);

				if (Bit_RESET == GPIO_ReadInputDataBit(I2C_GPIO,I2C_PIN_SCL))
				{
					GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL;
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
					GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

					GPIO_Write(I2C_GPIO,HighLvl_SCL_I2C);

					if (Bit_SET == GPIO_ReadInputDataBit(I2C_GPIO,I2C_PIN_SCL))
					{
						GPIO_InitStruct.GPIO_Pin = I2C_PIN_SDA;
						GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
						GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
						GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

						GPIO_Write(I2C_GPIO,HighLvl_SDA_SCL_I2C);

						if (Bit_SET == GPIO_ReadInputDataBit(I2C_GPIO,I2C_PIN_SDA))
						{
							GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
							GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
							GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
							GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

							I2C_SoftwareResetCmd(I2Cx,ENABLE);
							I2C_SoftwareResetCmd(I2Cx,DISABLE);

							I2C_Cmd(I2Cx, ENABLE);
						}
					}
				}
			}
		}
	}
}


static void configurationTimer(void)
{
// para 2HZ .. ARR=20.10^3 PSC= 1790
	//para 1khz  ARR=10.10^3 PSC=5
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//TIM_TimeBaseStructure.TIM_Period = 20000; //auto-reload 0 at ́e 65535
	TIM_TimeBaseStructure.TIM_Period = 10000; //auto-reload 0 at ́e 65535
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Prescaler = 5; //prescaler de 0 at ́e 65535 TIM
	//TIM_TimeBaseStructure.TIM_Prescaler = 1790; //prescaler de 0 at ́e 65535 TIM
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);

//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure1;
//	TIM_TimeBaseStructure1.TIM_Period = 255; //auto-reload 0 at ́e 65535
//	TIM_TimeBaseStructure1.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseStructure1.TIM_Prescaler = 281; //prescaler de 0 at ́e 65535 TIM
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


