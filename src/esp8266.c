/*
 * esp8266.c
 *
 *  Created on: Dec 5, 2020
 *      Author: adyprado
 */

#include "esp8266.h"




/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <string.h>

static void parser(const char *comando,char (*respuesta)[20]);
static void ESP8266_SendOverUsart(USART_TypeDef *USARTx,char *s);

#define cadena "test:uno,dos,tres,cuarto\r\n" //This define is for testing porpose of the function parser.


char respuesta[10][20];
int ESP8266_WaitingForAnswer = 0;


/******************************************************************************
 *  																	      *
 * 					Private Static Functions.                                 *
 *     																		  *
 ******************************************************************************/
static void ESP8266_SendOverUsart(USART_TypeDef *USARTx,char *s)
{
  while(*s)
  {
    while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET); // Wait for Empty

    USART_SendData(USARTx, *s++); // Send Char
  }
}


static void parser(const char *comando,char (*respuesta)[20])
{
    int p1=0,p2=0;
    char *aux = (char *)comando;
    while (*comando != '\r' && *(comando+1) != '\n')
    {
        static int counter = 0;
        if(*(comando) == ':')
        {
            comando++;
            aux = (char *)comando;
            p1=counter;
            do
            {
                if (*(comando++) == ',')
                {
                    p2 = counter++;
                    memcpy(*respuesta,aux,p2-p1);
                    //printf("%s\n",respuesta);
                    aux = (char *)comando;
                    p1 = p2+1;
                    respuesta++;
                }
                else
                {
                    counter++;
                    if (*comando == '\r' && *(comando+1) == '\n')
                    {
                        p2 = counter;
                        memcpy(*respuesta,aux,p2-p1);
                        //printf("%s\n",respuesta);
                        counter = 0;
                        break;
                    }
                }
            }while (1);
        }
        else
        {
            comando++;
            counter++;
        }
    }
}


/******************************************************************************
 *  																	      *
 * 							Global Functions.                                 *
 *     																		  *
 ******************************************************************************/


void initESP8266()
{

}

ESP8266_RETURN ESP866_SendAT(const char *comando,const char *expectedAnswer,char (*bufferRespuesta)[20],int _waitTime)
{
	char auxBuffer[100];
	sprintf("%s\r\n",comando);
	ESP8266_WaitingForAnswer = _waitTime;
	ESP8266_SendOverUsart(ESP8226_USART,comando);
	while(ESP8266_WaitingForAnswer != 0 )
	{

	}

}
