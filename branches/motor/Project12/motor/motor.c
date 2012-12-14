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
#define	P15	(1 << 15)
#define	P25	(1 << 25)
#define P21 (1 << 21)
#define	P23 (1 << 23)
#define	P31 (1 << 31)

#define TASK		1	// task 1 - use circulary loop
						// task 2 - use constant PWM signal
#define MODE_IN1	1	// mode 1 - Forward, Fast Current-Decay Mode
						// mode 2 - Forward, Slow Current-Decay Mode
						// mode 3 - Reverse, Fast Current-Decay Mode
						// mode 4 - Reverse, Slow Current-Decay Mode
						// mode 5 - Brake, Fast Current-Decay Mode
						// mode 6 - Brake, No Current Control
						// REMARK: only use "fast" modes
#define MODE_IN2	3		// see above description..


/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int runPwm(void);

/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void initPwm(tU32 initialFreqValue);
static void setPwmDutyPercent(tU32 dutyValue);
static void setPwmDuty(tU32 dutyValue);
static void delayMs(tU16 delayInMs);
void setModeIn1();
void setModeIn2();

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
	  PINSEL1 |= 0x00000100; //connect signal PWM5 to P0.21 (second alternative function)

	  PINSEL1 &= ~0x0000c000; //set P0.23 to GPIO

	  PINSEL1 &= ~0xc0000000;  //set P0.31 to GPIO
	  PINSEL1 |= 0x80000100;	// TODO: describe the pin functions
}

void setModeIn1() {

	// REMARK: do not use the "slow" functions, the "mode" pin on the 3953 PWM motor driver is never changed!
	if (MODE_IN1 == 1) {	// MODE_IN1 = Forward, Fast Current-Decay Mode
		IODIR |= P07;	// ENABLE (P0.7)
		IOCLR = P07;	// set to L

		IODIR |= P15;	// PHASE (P0.15)
		IOSET = P15;	// set to H

		IODIR |= P25;	// BRAKE (P0.25)
		IOSET = P25;	// set to H
	}
	//LOOK AT MODE_IN1
	else if (MODE_IN1 == 2){ // MODE_IN1 = Forward, Slow Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P15; // PHASE (P0.15)
		IOSET = P15; // set to H

		IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	}
	else if (MODE_IN1 == 3){ // Reverse, Fast Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P15; // PHASE (P0.15)
		IOCLR = P15; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	} //LOOK AT MODE
	else if (MODE_IN1 == 4){ // Reverse, Slow Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P15; // PHASE (P0.15)
		IOCLR = P15; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	}
	else if (MODE_IN1 == 5){ // Brake, Fast Current-Decay Mode
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P15; // PHASE (P0.15)
		IOCLR = P15; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOCLR = P25; // set to L
	}//LOOK AT MODE
	else if (MODE_IN1 == 6){ // Brake, No Current Control
		IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		IODIR |= P15; // PHASE (P0.15)
		IOCLR = P15; // set to L

		IODIR |= P25; // BRAKE (P0.25)
		IOCLR = P25; // set to L
	}

}
void setModeIn2() {

	if (MODE_IN2 == 1) {	// MODE_IN1 = Forward, Fast Current-Decay Mode
		IODIR |= P21;	// ENABLE (P0.7)
		IOCLR = P21;	// set to L

		IODIR |= P23;	// PHASE (P0.15)
		IOSET = P23;	// set to H

		IODIR |= P31;	// BRAKE (P0.25)
		IOSET = P31;	// set to H
	}
	//LOOK AT MODE_IN1
	else if (MODE_IN2 == 2){ // MODE_IN1 = Forward, Slow Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.15)
		IOSET = P23; // set to H

		IODIR |= P31; // BRAKE (P0.25)
		IOSET = P31; // set to H
	}
	else if (MODE_IN2 == 3){ // Reverse, Fast Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.15)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOSET = P31; // set to H
	} //LOOK AT MODE
	else if (MODE_IN2 == 4){ // Reverse, Slow Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.15)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOSET = P31; // set to H
	}
	else if (MODE_IN2 == 5){ // Brake, Fast Current-Decay Mode
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.15)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOCLR = P31; // set to L
	}//LOOK AT MODE
	else if (MODE_IN2 == 6){ // Brake, No Current Control
		IODIR |= P21; // ENABLE (P0.7)
		IOCLR = P21; // set to L

		IODIR |= P23; // PHASE (P0.15)
		IOCLR = P23; // set to L

		IODIR |= P31; // BRAKE (P0.25)
		IOCLR = P31; // set to L
	}

}

int
runPwm()
{
  tU32 duty;
  tU32 freq;

  //set frequency to 1000 Hz (1 kHz)
  freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (VPBDIV_FACTOR * 1000));

  //initialize PWM unit
  initPwm(freq);
  
  initPins();

  //vary duty cycle
  duty = 0;

  setModeIn1();
  setModeIn2();


  while(1)
  {
		//set frequency value
		setPwmDutyPercent(duty);

		//wait 10 ms
		delayMs(10);

		// COMMENT: the duty sets the speed of the motor driver
		// the value that can be set is 0-10000
		// a higher value means a lower speed, conversely a low value means a higher speed

		if (TASK == 1) {
			//update duty cycle (0.00 - 100.00%, in steps of 0.10%)
			duty += 10;
			if (duty > 10000)
				duty = 0;
		}

		else if (TASK == 2) {
			duty = 0;
		}

	}
}
