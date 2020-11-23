/*
 * sensor.c
 *
 *  Created on: Nov 21, 2020
 *      Author: Ady Salas
 */
#include "sensor.h"


/**
 *  Static Function Declarations
 */

static void Sensor_start(I2C_TypeDef* I2C, uint8_t address, uint8_t direction);
static void Sensor_write(I2C_TypeDef* I2C, uint8_t data);
static uint8_t Sensor_read_ack(I2C_TypeDef* I2C);
static uint8_t Sensor_read_nack(I2C_TypeDef* I2C);
static void Sensor_stop(I2C_TypeDef* I2C);


/* This function issues a start condition and
 * transmits the slave address + R/W bit
 *
 * Parameters:
 * 		I2C --> the I2C peripheral e.g. I2C1
 * 		address --> the 7 bit slave address
 * 		direction --> the transmission direction can be:
 * 						I2C_Direction_Tranmitter for Master transmitter mode
 * 						I2C_Direction_Receiver for Master receiver
 */
static void Sensor_start(I2C_TypeDef* I2C, uint8_t address, uint8_t direction){
	// wait until I2C1 is not busy anymore
	while(I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY));

	// Send I2C1 START condition
	I2C_GenerateSTART(I2C, ENABLE);

	// wait for I2C1 EV5 --> Slave has acknowledged start condition
	while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT));

	// Send slave Address for write
	I2C_Send7bitAddress(I2C, address, direction);

	/* wait for I2C1 EV6, check if
	 * either Slave has acknowledged Master transmitter or
	 * Master receiver mode, depending on the transmission
	 * direction
	 */
	if(direction == I2C_Direction_Transmitter){
		while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	}
	else if(direction == I2C_Direction_Receiver){
		while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	}
}

/* This function transmits one byte to the slave device
 * Parameters:
 *		I2C --> the I2C peripheral e.g. I2C1
 *		data --> the data byte to be transmitted
 */
static void Sensor_write(I2C_TypeDef* I2C, uint8_t data)
{
	I2C_SendData(I2C, data);
	// wait for I2C1 EV8_2 --> byte has been transmitted
	while(!I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}

/* This function reads one byte from the slave device
 * and acknowledges the byte (requests another byte)
 */
static uint8_t Sensor_read_ack(I2C_TypeDef* I2C){
	// enable acknowledge of recieved data
	I2C_AcknowledgeConfig(I2C, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2C);
	return data;
}

/* This function reads one byte from the slave device
 * and doesn't acknowledge the recieved data
 */
static uint8_t Sensor_read_nack(I2C_TypeDef* I2C){
	// disabe acknowledge of received data
	// nack also generates stop condition after last byte received
	// see reference manual for more info
	I2C_AcknowledgeConfig(I2C, DISABLE);
	I2C_GenerateSTOP(I2C, ENABLE);
	// wait until one byte has been received
	while( !I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	// read data from I2C data register and return data byte
	uint8_t data = I2C_ReceiveData(I2C);
	return data;
}

/* This function issues a stop condition and therefore
 * releases the bus
 */
static void Sensor_stop(I2C_TypeDef* I2C){
	// Send I2C1 STOP Condition
	I2C_GenerateSTOP(I2C, ENABLE);
}

/**
 *  Global Functions.
 * 
 */
void Sensor_Init(void)
{
	I2C_InitTypeDef I2C_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;

	I2C_DeInit(I2Cx);

	RCC_APB1PeriphClockCmd(I2Cx_RCC, ENABLE);

	I2C_InitStruct.I2C_ClockSpeed = MASTER_CLOCK_SPEED;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStruct.I2C_OwnAddress1 = 0x00;
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2Cx, &I2C_InitStruct);
	I2C_Cmd(I2Cx, ENABLE);

	#ifdef STM32F1_REMAP
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
	#endif

	GPIO_InitStruct.GPIO_Pin = I2C_PIN_SCL | I2C_PIN_SDA;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C_GPIO, &GPIO_InitStruct);

}

void Sensor_TEMP_Deinit(void)
{
	I2C_DeInit(I2Cx);
	I2C_SoftwareResetCmd(I2Cx,ENABLE);

	GPIO_DeInit(I2C_GPIO);

}


float Sensor_TEMP_Read()
{
	uint8_t raw_temprature[3];

	Sensor_start(I2Cx, SI7021_SLAVE_ADDR_Write, I2C_Direction_Transmitter); // comienzo de trama y primer comando con r/w en 0
	Sensor_write(I2Cx, TRIGGER_TEMP_MEASURE_NOHOLD); // escribo el comando de lectura
	//Sensor_write(I2Cx, 0x03); // write another byte to the slave
	Sensor_stop(I2Cx); // Deten la transmision, y genera estado de stop.

	delay_ms(50);

	Sensor_start(I2Cx, SI7021_SLAVE_ADDR_Read, I2C_Direction_Receiver); // start a transmission in Master receiver mode
	raw_temprature[0] = Sensor_read_ack(I2Cx); 					   //MSB Byte
	raw_temprature[1] = Sensor_read_ack(I2Cx); 					   //LSB Byte
	raw_temprature[2] = Sensor_read_nack(I2Cx);					   //CRC Byte

	unsigned int temperatureRaw = (raw_temprature[0] << 8 ) | raw_temprature[1];
	uint16_t raw_value = ((uint16_t) raw_temprature[0] << 8) | (uint16_t) raw_temprature[1];
	if(temperatureRaw == 0)
	{
		return -404;
	}

	return ((175.72*temperatureRaw)/65536)-46.85;
}


float Sensor_HUM_Read()
{
	uint8_t raw_humidity[3];

	Sensor_start(I2Cx, SI7021_SLAVE_ADDR_Write, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
	Sensor_write(I2Cx, TRIGGER_HUMD_MEASURE_NOHOLD); // write one byte to the slave
	//Sensor_write(I2Cx, 0x03); // write another byte to the slave
	Sensor_stop(I2Cx); // stop the transmission

	delay_ms(50);

	Sensor_start(I2Cx, SI7021_SLAVE_ADDR_Read, I2C_Direction_Receiver); // start a transmission in Master receiver mode
	raw_humidity[0] = Sensor_read_ack(I2Cx); 							//MSB Byte
	raw_humidity[1] = Sensor_read_ack(I2Cx); 							//LSB Byte
	raw_humidity[2] = Sensor_read_nack(I2Cx);							//CRC Byte

	uint16_t raw_value = ((uint16_t) raw_humidity[0] << 8) | (uint16_t) (raw_humidity[1] & 0xFD);
	if(raw_value == 0)
	{
		return -404;
	}

	// return the actual value according to the formula in the doc.
	return (raw_value * 125.0 / 65536.0) - 6.0;
}
