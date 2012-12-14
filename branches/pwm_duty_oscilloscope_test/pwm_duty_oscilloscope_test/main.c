/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to generate a PWM signal
 *    with a fixed frequency and varying duty cycle.
 *    This feature can for example be used to generate an analogue signal
 *    if the PWM signal is low pass filtered.
 *
 *****************************************************************************/

// TODO: make sure config/startup settings are adjusted for LPC2148

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include "startup/config.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY	FOSC
#define PLL_FACTOR			PLL_MUL
#define VPBDIV_FACTOR		PBSD

#define TEST_OUTPUT			(1 << 20)
#define PINSEL0_P07_MASK	(1 << 15) | (1 << 14)
#define PINSEL0_P07_PWM2	(1 << 15)
#define PINSEL1_P020_MASK	0x00000300
#define GPIO_IO_P20			0x00100000
#define GPIO_IO_P7			0x00000080


/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void initPwm(tU32 initialFreqValue);
static void setPwmDutyPercent(tU32 dutyValue);
static void setPwmDuty(tU32 dutyValue);
static void delayMs(tU16 delayInMs);
void put_on_other_port(void);
void do_nothing(void);
void init(void);

/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Initialize the PWM unit to generate a variable duty cycle signal on
 *    PWM2. Connect signal PWM2 to pin P0.7.
 *    The function sets initial frequency. Initial duty cucle is set to 0%.
 *
 * Params:
 *    [in] initialFreqValue - the initial frequency value. Value calculated as:
 *
 *                     (crystal frequency * PLL multiplication factor)
 *                     -----------------------------------------------
 *                           (VPBDIV factor * desired frequency)
 *
 ****************************************************************************/
static void
initPwm(tU32 initialFreqValue)
{
  /*
   * initialize PWM
   */
  PWM_PR  = 0x00000000;             //set prescale to 0
  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
  PWM_MR2 = 0;                      //MR2 = duty cycle control, initial = 0%
  PWM_LER = 0x05;                   //latch new values for MR0 and MR2
  PWM_PCR = 0x0400;                 //enable PWM2 in single edge control mode
  PWM_TCR = 0x09;                   //enable PWM and Counter

}

/*****************************************************************************
 *
 * Description:
 *    Update the duty cycle value of the PWM signal.
 *
 * Params:
 *    [in] dutyValue - the new duty cycle value in percent in increments
 *                     of 0.01% (i.e., 10% = 1000).
 *
 ****************************************************************************/
static void
setPwmDutyPercent(tU32 dutyValue)
{
  PWM_MR2 = (PWM_MR0 * dutyValue) / 10000;  //update duty cycle
  PWM_LER = 0x04;                           //latch new values for MR2
}

/*****************************************************************************
 *
 * Description:
 *    Update the duty cycle value of the PWM signal.
 *
 * Params:
 *    [in] dutyValue - the new duty cycle value. Value calculated as:
 *    (crystal frequency * PLL multiplication factor)
 *    ----------------------------------------------- * duty cycle in percent
 *           (VPBDIV factor * PWM frequency)
 *
 *                    or
 *
 *    value in MR0 register * duty cycle in percent
 *
 ****************************************************************************/
static void
setPwmDuty(tU32 dutyValue)
{
  PWM_MR2 = dutyValue;    //update duty cycle
  PWM_LER = 0x04;         //latch new values for MR2
}
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
static void
delayMs(tU16 delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = delayInMs * ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (1000 * VPBDIV_FACTOR));
  TIMER1_IR  = 0xff;          //reset all interrrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer
  
  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01)
    ;
}

void initPorts(void) {
	// connect signal PWM2 to pin P0.7
	PINSEL0 &= ~PINSEL0_P07_MASK;  //clear bits related to P0.7 (binds P0.7 to "GPIO Port 0.7" functionality)
							 //COMMENT: this operation is equivalent to:
							 //	PINSEL0 &= 0xFFFF3FFF
							 //for more info see p.75 of manualLPC2148.pdf
	PINSEL0 |= PINSEL0_P07_PWM2; //connect signal PWM2 to P0.7 (second alternative function)
						   //for more info see p.75 of manualLPC2148.pdf

						   // QUESTION: no need to set P0.7 as an output (via IODIR)?

	// connect P0.20 to output
	PINSEL1 &= ~PINSEL1_P020_MASK;	// clear bits 9:8 on P0.20
								// which makes P0.20 a GPIO

	IODIR |= GPIO_IO_P20;	// P0.20 becomes an output
	//IOSET = 0x00000000;		// QUESTION: this statement doesn't do anything?


}

void init(void) {
	tU32 freq;

	//set frequency to 1000 Hz (1 kHz)
	freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (VPBDIV_FACTOR * 1000));

	//initialize PWM unit
	initPwm(freq);

	initPorts();

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

	init();

	//vary duty cycle
	tU32 duty = 0;

	while (1) {
		//set frequency value
		setPwmDutyPercent(duty);

		//wait 10 ms
		delayMs(10);

		//update duty cycle (0.00 - 100.00%, in steps of 0.10%)
		duty += 10;
		if (duty > 10000)
			duty = 0;

		put_on_other_port();
		// TODO: read the output of PWM2 on the oscilloscope (what's the most performance efficient way to do this?)
		//	1. move value of P0.07 to P0.20? (assembly code)
		//	2. if-else statements that set/clear P0.20 depending on the value of P0.07

		// COMMENT: Det jag gjorde fel första gången var att jag aldrig anropade put_on_other_port(void) eftersom den låg utanför den oändliga loopen... OMFG!!!
	}

	return 0;
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------

void test1(void) {	// test function for putting values on other port
//	asm("MOV R0, 0x00000080");	// moves contents of P0.7 to R0
//	asm("STR R0, 0x00100000");	// moves contents of R0 to P0.20
}

void test2(void) {	// test function for putting values on other port
//	asm("LDR R0, P0.7");
//	asm("STR R0, P0.20");
}

void test3(void) {	// test function for putting values on other port
//	asm("MOV [0x00000080], [0x00100000]");
}

void test4(void) {	// test function for putting values on other port
//	asm("LDR R0, =0x00000080");
//	asm("STR R0, [0x00100000]");
}

void test5(void) {	// test function for putting values on other port
	if (IOPIN & 0x00000080)
		IOSET = 0x00100000;
	else
		IOCLR = 0x00100000;
}

void test6(void) {	// test function for putting values on other port
	int i;

	IOSET |= TEST_OUTPUT;

	for (i = 0; i < 10000; i++) {
		do_nothing();
	}
	delayMs(500);

	IOCLR |= TEST_OUTPUT;

	for (i = 0; i < 10000; i++) {
		do_nothing();
	}
	delayMs(500);
}

// ------------------------------------------------------------------------
// ------------------------------------------------------------------------






void put_on_other_port(void) {

	// QUESTION: P0.7 can't be connected to P0.20?
	int mask;

	mask = IOPIN;
	mask &= GPIO_IO_P7;

	if (mask & GPIO_IO_P7)	// value on P0.7 is 0
		IOCLR |= TEST_OUTPUT;
	else
		IOSET |= TEST_OUTPUT;


}

void do_nothing(void) {
	asm("NOP");
}
