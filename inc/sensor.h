//#include "configurations.h"
#include "stm32f10x.h"
#include "stm32f1xx_nucleo.h"





/**
 * 
 */
void Sensor_Init(void);
void Sensor_TEMP_Deinit(void);
float Sensor_TEMP_Read();
float Sensor_HUM_Read();

/*
 * I2C Defines
 * */
#define STM32F1_REMAP

#ifdef STM32F1_REMAP
#define I2C_PIN_SDA                 GPIO_Pin_9
#define I2C_PIN_SCL                 GPIO_Pin_8
#else
#define I2C_PIN_SDA                 GPIO_Pin_7
#define I2C_PIN_SCL                 GPIO_Pin_6
#endif
#define HighLvl_SDA_SCL_I2C         0x300
#define LowLvl_SDA_I2C              0x100
#define LowLvl_SCL_I2C              0x000
#define HighLvl_SCL_I2C             0x100
#define SI7021_SLAVE_ADDR	        0x40
#define SI7021_SLAVE_ADDR_Read      ((SI7021_SLAVE_ADDR<<1)| 0x01)
#define SI7021_SLAVE_ADDR_Write     (SI7021_SLAVE_ADDR<<1)
#define I2C_GPIO_RCC                RCC_APB2Periph_GPIOB
#define I2C_GPIO                    GPIOB
#define I2Cx_RCC                    RCC_APB1Periph_I2C1
#define I2Cx                        I2C1


/**
 * SI7021 Sensor Commands
 */
#define TRIGGER_TEMP_MEASURE_HOLD		0xE3
#define TRIGGER_HUMD_MEASURE_HOLD  		0xE5
#define TRIGGER_TEMP_MEASURE_NOHOLD  	0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD  	0xF5
#define WRITE_USER_REG  				0xE6
#define READ_USER_REG  					0xE7
#define SOFT_RESET  					0xFE

/**
 *  SI7021 Driver Error Codes
 */
#define SI7021_ERR_OK					"Operation returned OK."
#define SI7021_ERR_CONFIG				"Error on configration."
#define SI7021_ERR_INSTALL				"Error on installation."
#define SI7021_ERR_NOTFOUND				"Error - NOT FOUND"
#define SI7021_ERR_INVALID_ARG			"Error - Invalid Argument"
#define SI7021_ERR_FAIL		 			"Error - Failed"
#define SI7021_ERR_INVALID_STATE		"Error - Invalid State"
#define SI7021_ERR_TIMEOUT	 			"Error - Time-Out"

#define MASTER_CLOCK_SPEED 				100000
