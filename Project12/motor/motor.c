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

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
#include <lpc2xxx.h>
#include "../startup/config.h"


/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define P07 (1 << 7)
#define	P12	(1 << 12)
#define	P25	(1 << 25)
#define P21 (1 << 21)
#define	P23 (1 << 23)
#define	P31 (1 << 31)

#define TASK	2		// task 1 - use circulary loop
						// task 2 - use constant PWM signal
//#define MODE	1		// mode 1 - Forward, Fast Current-Decay Mode
						// mode 2 - Forward, Slow Current-Decay Mode
						// mode 3 - Reverse, Fast Current-Decay Mode
						// mode 4 - Reverse, Slow Current-Decay Mode
						// mode 5 - Brake, Fast Current-Decay Mode
						// mode 6 - Brake, No Current Control
//#define MODE2	3

int mode1 = 0;
int mode2 = 0;
/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
static void initPwm(tU32 initialFreqValue);
static void setPwmDutyPercent1(tU32 dutyValue1);
static void setPwmDutyPercent2(tU32 dutyValue2);
static void delayMs(tU16 delayInMs);
void setMode1();
void setMode2();
void dev_run(tU32 duty1, tU32 duty2);

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
  PWM_MR2 = 0;      				//MR2 = duty cycle control, initial = 0%
  PWM_MR5 = 0;  					//MR5 = duty cycle control, initial = 0%
  PWM_LER = 0x25;                   //latch new values for MR0 and MR2
  PWM_PCR = 0x2400;                 //enable PWM2 in single edge control mode
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
setPwmDutyPercent1(tU32 dutyValue1)
{
  PWM_MR2 = (PWM_MR0 * dutyValue1) / 10000; //update duty cycle
  PWM_LER = 0x04;                           //latch new values for MR2
}

static void
setPwmDutyPercent2(tU32 dutyValue2)
{
  PWM_MR5 = (PWM_MR0 * dutyValue2) / 10000;  //update duty cycle
  PWM_LER = 0x20;         //latch new values for MR5
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
void initPins() {

	/*
	   * connect signal PWM2 to pin P0.7
	   */
	  PINSEL0 &= ~0x0000c000;  //clear bits related to P0.7
	  PINSEL0 |=  0x00008000;  //connect signal PWM2 to P0.7 (second alternative function)

	  PINSEL0 &= ~0xC0000000;	// set P0.15 to EINT2
	  PINSEL0 |=  0x80000000;

	  PINSEL1 &= ~0x000C0000;	// set P0.25 to GPIO Port 0.25
	  /*
	  	   * connect signal PWM5 to pin P0.21
	  	   */

	  PINSEL1 &= ~0x00000c00; //clear bits related to P0.21
	  PINSEL1 |= 0x00000400; //connect signal PWM5 to P0.21 (second alternative function)

	  PINSEL1 &= ~0x0000c000; //set P0.23 to GPIO

	  PINSEL1 &= ~0xc0000000;  //set P0.31 to GPO Port only
	  //PINSEL1 |= 0x80000000;
}

void setMode1( int MODE) {

	if (MODE == 1) {	// MODE = Forward, Fast Current-Decay Mode
		IODIR |= P07;	// ENABLE (P0.7)
		IOCLR = P07;	// set to L

		IODIR |= P12;	// PHASE (P0.15)
		IOSET = P12;	// set to H

		IODIR |= P25;	// BRAKE (P0.25)
		IOSET = P25;	// set to H
	}
	//LOOK AT MODE
	else if (MODE == 2){ // MODE = Forward, Slow Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P12; // PHASE (P0.15)
		IOSET = P12; // set to H

		IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	}
	else if (MODE == 3){ // Reverse, Fast Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P12; // PHASE (P0.15)
		IOCLR = P12; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	} //LOOK AT MODE
	else if (MODE == 4){ // Reverse, Slow Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P12; // PHASE (P0.15)
		IOCLR = P12; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	}
	else if (MODE == 5){ // Brake, Fast Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P12; // PHASE (P0.15)
		IOCLR = P12; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOCLR = P25; // set to L
	}//LOOK AT MODE
	else if (MODE == 6){ // Brake, No Current Control
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P12; // PHASE (P0.12)
		IOCLR = P12; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOCLR = P25; // set to L
	}

}
void setMode2(int MODE2) {

	if (MODE2 == 1) {	// MODE = Forward, Fast Current-Decay Mode
		IODIR |= P21;	// ENABLE (P0.7)
		IOCLR = P21;	// set to L

		IODIR |= P23;	// PHASE (P0.12)
		IOSET = P23;	// set to H

		IODIR |= P31;	// BRAKE (P0.25)
		IOSET = P31;	// set to H
	}
	//LOOK AT MODE
	else if (MODE2 == 2){ // MODE = Forward, Slow Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.12)
		IOSET = P23; // set to H

		IODIR |= P31; // BRAKE (P0.25)
		IOSET = P31; // set to H
	}
	else if (MODE2 == 3){ // Reverse, Fast Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.12)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOSET = P31; // set to H
	} //LOOK AT MODE
	else if (MODE2 == 4){ // Reverse, Slow Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.12)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOSET = P31; // set to H
	}
	else if (MODE2 == 5){ // Brake, Fast Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.12)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOCLR = P31; // set to L
	}//LOOK AT MODE
	else if (MODE2 == 6){ // Brake, No Current Control
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.12)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOCLR = P31; // set to L
	}

}

int
runPwm()
{
  tU32 duty1;
  tU32 duty2;
  tU32 freq;

  //set frequency to 1000 Hz (1 kHz)
  freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (VPBDIV_FACTOR * 1000));
  //initialize PWM unit
  initPwm(freq);
  
  initPins();

  //vary duty cycle
  duty1 = 0;
  duty2 = 0;

  setMode1(mode1);
  setMode2(mode2);

  //dev_run(duty1, duty2);

  return 0;
}

// dev function - not used in release
void dev_run(tU32 duty1, tU32 duty2) {
	while (1) {
		//set frequency value
		setPwmDutyPercent1(duty1);
		setPwmDutyPercent2(duty2);

		//wait 10 ms
		delayMs(10);

		// COMMENT: the duty sets the speed of the motor driver
		// the value that can be set is 0-10000
		// a higher value means a lower speed, conversely a low value means a higher speed

		if (TASK == 1) {
			//update duty cycle (0.00 - 100.00%, in steps of 0.10%)
			duty1 += 10;
			if (duty1 > 10000)
				duty1 = 0;
		}

		else if (TASK == 2) {
			duty1 = 0;
			duty2 = 8500;
							// COMMENT: slowest speed = 8500 duty
							//			fastest speed = 0	 duty
		}

	}
}
