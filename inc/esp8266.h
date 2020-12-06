#include "stm32f10x.h"
#include "stm32f1xx_nucleo.h"




typedef int ESP8266_RETURN;





#define ESP8226_USART  			(USART3)
#define ESP866_RETURN_OK		(1u)
#define ESP866_RETURN_nOK		(0u)
void initESP8266(void);
ESP8266_RETURN ESP866_SendAT(const char *comando,const char *expectedAnswer,char (*bufferRespuesta)[20],int waitTime);

#define USART_ESP8266	USART3
