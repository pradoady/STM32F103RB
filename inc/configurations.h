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

/*
 * I2C Defines
 * */

#define I2C_PIN_SDA     GPIO_Pin_9
#define I2C_PIN_SCL     GPIO_Pin_8
#define HighLvl_SDA_SCL_I2C 0x300
#define LowLvl_SDA_I2C  0x100
#define LowLvl_SCL_I2C  0x000
#define HighLvl_SCL_I2C 0x100
#define I2C_GPIO_RCC    RCC_APB2Periph_GPIOB
#define I2C_GPIO        GPIOB
#define I2Cx_RCC        RCC_APB1Periph_I2C1
#define I2Cx            I2C1
#define SLAVE_ADDRESS    0x40

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
void I2C_CONFIG(void);
