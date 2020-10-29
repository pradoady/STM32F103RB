/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "configurations.h"



static void rxPollingUSART2(void);
//uint16_t *rxData;


volatile char line_buffer[LINEMAX + 1]; // Holding buffer with space for terminating NUL
volatile int line_valid = 0;

int main(void)
{

	SystemInit();
	configuration();
	//rxData = (uint16_t *)malloc(sizeof(uint16_t) * 50);

	for(;;)
	{
		char buffer[30];
		static unsigned int tempo=0;
		for (uint32_t i=0; i<0x000FFFFF; i++)
		{
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
		}
		sprintf(buffer,"%i \n\r",tempo);
		OutString(USART2,&buffer);
		OutString(USART3,&buffer);
		tempo++;
		for (uint32_t i=0; i<0x000FFFFF; i++)
		{
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
		}
		sprintf(buffer,"%i \n\r",tempo);
		OutString(USART2,&buffer);
		OutString(USART3,&buffer);
		tempo++;

		//rxPollingUSART2();
	}
}







static void rxPollingUSART2(void)
{
	static uint8_t flagON = 1;
	char buffer[30];
	uint16_t rxData[30]={'0'};
	if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == SET)
	{
		rxData[0] = (uint16_t)USART_ReceiveData(USART3);
		USART_ClearFlag(USART3,USART_FLAG_RXNE);
		sprintf(buffer,"%c \n\r",rxData);
		OutString(USART3,&buffer);
	}

}
