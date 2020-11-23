#include "stm32f10x.h"
#include "stm32f1xx_nucleo.h"
#include "stdio.h"
#include <string.h>






void configuration(void);
void OutString(USART_TypeDef *USARTx,char *s);




/**
 * Defines and Macro Functions
 *
 */
#define CR		0xA
#define LF		0xD
#define LINEMAX 200 // Maximal allowed/expected line lengtha
#define TRUE 	1
#define FALSE	0


#ifdef DEBUG
/**
 * This macro will print on USART2.
 * The (...) parameters should be as you were using the 
 * sprintf function.
 */ 
#define printDebug(buffer,...) do{\
                                sprintf(buffer,__VA_ARGS__);\
                                OutString(USART2, &buffer);\
                                }while(0)
#endif
