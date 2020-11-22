#include "configurations.h"






/**
 * 
 */
void DRV_TEMP_Deinit(I2C_TypeDef* i2cType,GPIO_TypeDef* gpioType);
float DRV_TEMP_Read();
float DRV_HUM_Read();
void Sensor_start(I2C_TypeDef* I2C, uint8_t address, uint8_t direction);
void Sensor_write(I2C_TypeDef* I2C, uint8_t data);
uint8_t Sensor_read_ack(I2C_TypeDef* I2C);
uint8_t Sensor_read_nack(I2C_TypeDef* I2C);
void Sensor_stop(I2C_TypeDef* I2C);

 /*Defines*/

#define SI7021_SLAVE_ADDR				0x40

// SI7021 Sensor Commands
#define TRIGGER_TEMP_MEASURE_HOLD		0xE3
#define TRIGGER_HUMD_MEASURE_HOLD  		0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD  	0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD  	0xF5
#define WRITE_USER_REG  				0xE6
#define READ_USER_REG  					0xE7
#define SOFT_RESET  					0xFE

// SI7021 Driver Error Codes
#define SI7021_ERR_OK					"Operation returned OK."
#define SI7021_ERR_CONFIG				"Error on configration."
#define SI7021_ERR_INSTALL				"Error on installation."
#define SI7021_ERR_NOTFOUND				"Error - NOT FOUND"
#define SI7021_ERR_INVALID_ARG			"Error - Invalid Argument"
#define SI7021_ERR_FAIL		 			"Error - Failed"
#define SI7021_ERR_INVALID_STATE		"Error - Invalid State"
#define SI7021_ERR_TIMEOUT	 			"Error - Time-Out"

#define MASTER_CLOCK_SPEED 				100000
