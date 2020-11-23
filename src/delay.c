/*
 * delay.c
 *
 *  Created on: Nov 21, 2020
 *      Author: Saldory
 */

#include "delay.h"


/**
 * This variable is used in Interrupt context to handle a simple delay.
 * The interrupt is generated each mSecond.
 * For that reason this delay can be used in any context that is needed a simply delay.
 */
volatile uint32_t delayTimer=0; 

void delay_ms(u32 n)
{
	delayTimer = n;
	while (delayTimer > 0);

}

