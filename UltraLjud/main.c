/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use the i2c library.
 *    Omarbetad för att testa hur man läser temp.sensorn /Tommy
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <ea_init.h>
#include <framework.h>
#include "startup/config.h"
#include "i2c.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD
#define pin4			  0x00000010 //0b10000

/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void delayMs(tU16 delayInMs);

/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Delay execution by a specified number of milliseconds by using
 *    timer #1. A polled implementation.
 *
 * Params:
 *    [in] delayInMs - the number of milliseconds to delay.
 *
 ****************************************************************************/
static void delayMs(tU16 delayInMs) {
	/*
	 * setup timer #1 for delay
	 */
	TIMER1_TCR = 0x02;          //stop and reset timer
	TIMER1_PR = 0x00;          //set prescaler to zero
	TIMER1_MR0 = delayInMs
			* ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (1000 * VPBDIV_FACTOR));
	TIMER1_IR = 0xff;          //reset all interrrupt flags
	TIMER1_MCR = 0x04;          //stop timer on match
	TIMER1_TCR = 0x01;          //start timer

	//wait until delay time has elapsed
	while (TIMER1_TCR & 0x01)
		;
}

/*****************************************************************************
 * If we want a delay of x µs we have to send 15•x with the function delay.
 ****************************************************************************/
void delay(long pulses)
{
	asm("wait:   SUBS R0, R0, #1");
	asm("BNE wait");
}

/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    The main-function.
 *
 * Returns:
 *    Always 0, since return value is not used.
 *
 ****************************************************************************/
int main(void) {

	//initialize printf()-functionality
	eaInit();

	//print welcome message
	printf("Distance Measuring");

	//initialice i2c communication
	i2cInit(10000);

	//enter forever loop...
	while (1) {
		tU8 data[1];

		IODIR &= ~pin4; // make pin4 to input
		IOCLR |= pin4; // make pin4 high

		data[0] = 0; // 8 mest signifikanta bitarna

	    // Sending data to I2C network
		i2cStart();
		i2cWrite(0x02, 0x1, 0x42, data, 0); // 0x42 Command "Read Distance" send buffer
		i2cStop();

		delay(1500); 	// If we want a delay of x µs we have to send 15•x with the function delay.
		IODIR |= pin4; 	// make pin4 to output
		delay(750);
		IODIR &= ~pin4; // make pin4 to input
		delay(1500);

		// Reading data from I2C network
		i2cStart();
		i2cRead(0x03, data, 1); // storing data in pin4
		i2cStop();

		printf("\nDistance: %d cm", data[0]);

		//wait 1000 ms
		delayMs(1000);
	}

	return 0;
}
