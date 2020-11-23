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




void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_address_direction(uint8_t address, uint8_t direction);
void i2c_transmit(uint8_t byte);
uint8_t i2c_receive_ack(void);
uint8_t i2c_receive_nack(void);
void i2c_write(uint8_t address, uint8_t data);
void i2c_read(uint8_t address, uint8_t* data);
void i2c_unlockBusyFlag(void);
void Sensor_Init(void);
