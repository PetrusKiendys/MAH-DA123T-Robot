// TODO: rewrite this file as a lib that can be used by other processes and interrupts

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
#include "../utils/lpc214x.h"


/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define P07 (1 << 7)
#define P12	(1 << 12)
#define	P25	(1 << 25)
#define P21 (1 << 21)
#define	P23 (1 << 23)
#define	P31 (1 << 31)

#define TASK	7		// task 1 - use circulary loop
						// task 2 - use constant PWM signal
						// ...
//#define MODE	1		// mode 1 - Forward, Fast Current-Decay Mode
						// mode 2 - Forward, Slow Current-Decay Mode (not implemented)
						// mode 3 - Reverse, Fast Current-Decay Mode
						// mode 4 - Reverse, Slow Current-Decay Mode (not implemented)
						// mode 5 - Brake, Fast Current-Decay Mode
						// mode 6 - Brake, No Current Control		 (not implemented)
//#define MODE2	1

#define FORWARD		1
#define BACKWARD	2
#define BRAKE		3

#define RUN_SETPWM_IN_LOOP	0	// dictates whether setPwmDutyPercentx(tU32) should be run outside the "TASK conditional statement"


/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
static void initPwm(tU32 initialFreqValue);
static void setPwmDutyPercent1(tU32 dutyValue1);
static void setPwmDutyPercent2(tU32 dutyValue2);
static void delayMs(tU16 delayInMs);
void setMode1(short mode);
void setMode2(short mode);
void init_3953_io(void);

//----------- DEV functions ------------------//
void dev_run(tU32 duty1, tU32 duty2);
void pwm_motor_init(void);
void pwm_motor_run(tU32 duty1, tU32 duty2);
void change_mode(short mode);
//----------- end of DEV functions -----------//

void delay_millis(long ms);

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
  PWM_LER = 0x25;                   //latch new values for MR0 and MR2 and MR5
  PWM_PCR = 0x2400;                 //enable PWM2 and in single edge control mode
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

	  PINSEL0 &= ~0xc0000000;	// set P0.15 to EINT2
	  PINSEL0 |=  0x80000000;

	  PINSEL1 &= ~0x000c0000;	// set P0.25 to GPIO Port 0.25
	  /*
	  	   * connect signal PWM5 to pin P0.21
	  	   */

	  PINSEL1 &= ~0x00000c00; //clear bits related to P0.21
	  PINSEL1 |= 0x00000400; //connect signal PWM5 to P0.21 (second alternative function)

	  PINSEL1 &= ~0x0000c000; //set P0.23 to GPIO

	  PINSEL1 &= ~0xc0000000;  //set P0.31 to GPO Port only
	  //PINSEL1 |= 0x80000000;
}


// COMMENT: PHASE was earlier set to P0.15, this is incorrect!
void setMode1(short mode) {

	if (mode == 1) {	// MODE = Forward, Fast Current-Decay Mode
		//IODIR |= P07;	// ENABLE (P0.7)
		IOCLR = P07;	// set to L

		//IODIR |= P12;	// PHASE (P0.12)
		IOSET = P12;	// set to H

		//IODIR |= P25;	// BRAKE (P0.25)
		IOSET = P25;	// set to H
	}
	else if (mode == 2){ // Reverse, Fast Current-Decay Mode
		//IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		//IODIR |= P12; // PHASE (P0.12)
		IOCLR = P12; // set to L

		//IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	}
	else if (mode == 3){ // Brake, Fast Current-Decay Mode
		//IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		//IODIR |= P12; // PHASE (P0.12)
		IOCLR = P12; // set to L

		//IODIR |= P25; // BRAKE (P0.25)
		IOCLR = P25; // set to L
	}

}


void setMode2(short mode) {

	if (mode == 1) {	// MODE = Forward, Fast Current-Decay Mode
		//IODIR |= P21;	// ENABLE (P0.21)
		IOCLR = P21;	// set to L

		//IODIR |= P23;	// PHASE (P0.23)
		IOSET = P23;	// set to H

		//IODIR |= P31;	// BRAKE (P0.31)
		IOSET = P31;	// set to H
	}
	else if (mode == 2){ // Reverse, Fast Current-Decay Mode
		//IODIR |= P21; // ENABLE (P0.21)
		IOCLR = P21; // set to L

		//IODIR |= P23; // PHASE (P0.23)
		IOCLR = P23; // set to L

		//IODIR |= P31; // BRAKE (P0.31)
		IOSET = P31; // set to H
	}

	else if (mode == 3){ // Brake, Fast Current-Decay Mode
		//IODIR |= P21; // ENABLE (P0.21)
		IOCLR = P21; // set to L

		//IODIR |= P23; // PHASE (P0.23)
		IOCLR = P23; // set to L

		//IODIR |= P31; // BRAKE (P0.31)
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

  init_3953_io();

  //set modes for UT1 and UT2
  setMode1(FORWARD);
  setMode2(FORWARD);

  dev_run(duty1, duty2);

  //while(1);

  return 0;
}

void init_3953_io() {
	// UT1
	IODIR |= P07;	// ENABLE (P0.7)
	IODIR |= P12;	// PHASE (P0.12)
	IODIR |= P25;	// BRAKE (P0.25)

	// UT2
	IODIR |= P21;	// ENABLE (P0.21)
	IODIR |= P23;	// PHASE (P0.23)
	IODIR |= P31;	// BRAKE (P0.31)
}

// dev function - not used in release

// COMMENT: PWM signal is continuously generated when the setPwmDutyPercent(tU32) function is called, no need to endlessly iterate the calls
// UPDATE:  seems like you need to iterate over the setPwmDutyPercent(tU32) functions afterall...
// COMMENT: the duty sets the speed of the motor driver
// 			the value that can be set is 0-10000
// 			a higher value means a lower speed, conversely a low value means a higher speed
void dev_run(tU32 duty1, tU32 duty2) {
	//wait 10 ms
	//delayMs(10);

	//wait 100 ms
	//delayMs(100);

	// COMMENT: try running without delays
	// 			seems to work fine..

	while (1) {

		//set frequency value
		if (RUN_SETPWM_IN_LOOP == 1) {
			setPwmDutyPercent1(duty1);
			setPwmDutyPercent2(duty2);
		}



		switch (TASK) {
		case 1: {
			//update duty cycle (0.00 - 100.00%, in steps of 0.10%)
			duty1 += 10;
			if (duty1 > 10000)
				duty1 = 0;
			break;
		}
		case 2: {
			duty1 = 0;
			duty2 = 8500;
			// COMMENT: slowest speed = 8500 duty
			//			fastest speed = 0	 duty
			break;
		}
		case 3: { // left
			duty1 = 6000;
			duty2 = 8000;
			break;
		}
		case 4: { // right
			duty1 = 8000;
			duty2 = 6000;
			break;
		}
		case 5: {
			duty1 = 6000;
			duty2 = 6000;
			break;
		}
		case 6: {
			short delay = 1;
			short duty_vals = 6000;
			short section[3] = { 1, 0, 0 };

			if (section[0] == 1) {
				setMode1(FORWARD);
				setMode2(FORWARD);
				duty1 = duty_vals;
				duty2 = duty_vals;
				setPwmDutyPercent1(duty1);
				setPwmDutyPercent2(duty2);
				//delayMs(0);
				delay_millis(delay);
			}

			if (section[1] == 1) {
				setMode1(BRAKE);
				setMode2(BRAKE);
				setPwmDutyPercent1(duty1);
				setPwmDutyPercent2(duty2);
				delay_millis(delay);
			}

			if (section[2] == 1) {
				setMode1(BACKWARD);
				setMode2(BACKWARD);
				duty1 = duty_vals;
				duty2 = duty_vals;
				setPwmDutyPercent1(duty1);
				setPwmDutyPercent2(duty2);
				delay_millis(delay);
			}
			break;
		}
		case 7: {
			pwm_motor_init();
			duty1 = 6000;
			duty2 = 6000;

			delay_millis(10);

			pwm_motor_run(duty1, duty2);
			break;
		}

		}
	}
}

void pwm_motor_init() {
	setMode1(FORWARD);
	setMode2(FORWARD);

	delay_millis(200);
}

void pwm_motor_run(tU32 duty1, tU32 duty2) {
	setPwmDutyPercent1(duty1);
	setPwmDutyPercent2(duty2);

	while(1) {
		change_mode(FORWARD);
		change_mode(BRAKE);
		change_mode(BACKWARD);
		change_mode(BRAKE);
	}
}

void change_mode(short mode) {
	setMode1(mode);
	setMode2(mode);
	delay_millis(200);
}
