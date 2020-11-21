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


volatile char line_buffer_Usart3[LINEMAX + 1]; // Holding buffer with space for terminating NUL
volatile char line_buffer_Usart2[LINEMAX + 1]; // Holding buffer with space for terminating NUL
volatile int line_valid = 0;
uint8_t receivedByte;
uint8_t data[2] = {0};
volatile int iCounter100mSec = 0;
volatile int iCounter30mSec = 0;
int bCounter100mSec = 0;
int bCounter30mSec = 0;
int b5Sec = 0;
uint16_t iSecCounter = 0;
int bI2C_EVENT_MASTER_BYTE_RECEIVED = FALSE;
int bI2C_EVENT_MASTER_BYTE_TRANSMITTED = FALSE;
int bI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED = FALSE;
int bI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED = FALSE;


int main(void)
{

	SystemInit();
	configuration();



	for(;;)
	{
		char buffer[100];
		/*for (uint32_t i=0; i<0x000FFFFF; i++)
		{
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
		}
		sprintf(buffer,"%i \n\r",tempo);
		OutString(USART2,&buffer);
		//OutString(USART3,&buffer);
		tempo++;
		for (uint32_t i=0; i<0x000FFFFF; i++)
		{
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
		}
		sprintf(buffer,"%i \n\r",tempo);
		OutString(USART2,&buffer);
		tempo++;*/

		/*while(GPIO_ReadInputDataBit(I2C_GPIO,I2C_PIN_SDA) == RESET)
		{
			static int i =0;
			if(i++ > 10)
			{
				i =0;
				break;
			}
			GPIO_WriteBit(I2C_GPIO,I2C_PIN_SDA, 0);
			for (uint32_t i=0; i<0x0FFF; i++)
			{
			}
			GPIO_WriteBit(I2C_GPIO,I2C_PIN_SDA, 1);
			for (uint32_t i=0; i<0x0FFF; i++)
			{
			}
		}*/
		if (TRUE == b5Sec)
		{
			sprintf(buffer,"I2C START \n\r");
			OutString(USART2,&buffer);
			i2c_write(SLAVE_ADDRESS,0xF3);
			sprintf(buffer,"I2C WRITE OK \n\r");
			OutString(USART2,&buffer);

			delay100mSec();

			sprintf(buffer,"I2C read start \n\r");
			OutString(USART2,&buffer);
			i2c_read(SLAVE_ADDRESS,&data);

			sprintf(buffer,"I2C read ok: %i %i\n\r",data[0],data[1]);
			OutString(USART2,&buffer);

			unsigned int temperatureRaw = (data[0] << 8 ) | data[1];
			float temperature = ((175.72*temperatureRaw)/65536)-46.85;
			sprintf(buffer,"Temperature %f\n\r",temperature);
			OutString(USART2,&buffer);

			b5Sec = FALSE;
			iSecCounter = 0;
		}
		if (TRUE == bI2C_EVENT_MASTER_BYTE_RECEIVED)
		{
			bI2C_EVENT_MASTER_BYTE_RECEIVED = FALSE;
			sprintf(buffer,"I2C FAIL ON I2C_EVENT_MASTER_BYTE_RECEIVED \n\r");
			OutString(USART2,&buffer);
		}
		if( TRUE == bI2C_EVENT_MASTER_BYTE_TRANSMITTED)
		{
			bI2C_EVENT_MASTER_BYTE_TRANSMITTED = FALSE;
			sprintf(buffer,"I2C FAIL ON bI2C_EVENT_MASTER_BYTE_TRANSMITTED \n\r");
			OutString(USART2,&buffer);
		}
		if (TRUE == bI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)
		{
			bI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED = FALSE;
			sprintf(buffer,"I2C FAIL ON bI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED \n\r");
			OutString(USART2,&buffer);
		}
		if (TRUE == bI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)
		{
			bI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED = FALSE;
			sprintf(buffer,"I2C FAIL ON I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED \n\r");
			OutString(USART2,&buffer);
		}

		if (TRUE == (bI2C_EVENT_MASTER_BYTE_RECEIVED || bI2C_EVENT_MASTER_BYTE_TRANSMITTED
				|| bI2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED || bI2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			I2C_DeInit(I2Cx);
			I2C_CONFIG();
		}

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
