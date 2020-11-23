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
#include "sensor.h"



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
	
		if (TRUE == b5Sec)
		{
			float temperature = 0;
			float humedad = 0;
			temperature = Sensor_TEMP_Read();
			humedad = Sensor_HUM_Read();
			printDebug(buffer,"Temperature %.3f C Humedade:  %.3f %%\n\r",temperature,humedad);
			b5Sec = FALSE;
			iSecCounter = 0;
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
