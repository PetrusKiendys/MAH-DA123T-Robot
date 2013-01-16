// TODO: rewrite this file as a lib that can be used by other processes and interrupts
// TODO: make a clearer separation between init functions and other functions

/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *    QUESTION: modified EA code belongs to EA?
 *
 * File:
 *    motor.c
 *
 * Description:
 *    ...
 *
 *****************************************************************************/

#include "motor.h"

/*****************************************************************************
 * Functions (initialization)
 *****************************************************************************/
void init() {
	// declare global and local variables
	tU32 freq;

	// initialize I/O
	init_io();

	// initialize PWM
	freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (VPBDIV_FACTOR * 1000));	//set frequency to 1000 Hz (1 kHz)
	initPwm(freq);

	// initialize pins (on port P0)
	initPins();

	// initialize VIC
	init_vic();

	//set modes for UT1 and UT2 outputs
	setMode1(MODE_FORWARD);
	setMode2(MODE_FORWARD);

	// initialize EINT (external interrupts)
	init_EINT0();
	init_EINT3();

	// initializes the default PWM duty
	duty1 = 0;
	duty2 = 0;
	setPwmDutyPercent1(duty1);
	setPwmDutyPercent2(duty2);
}

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
void initPwm(tU32 initialFreqValue)
{
  PWM_PR  = 0x00000000;             //set prescale to 0
  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
  PWM_MR2 = 0;      				//MR2 = duty cycle control, initial = 0%
  PWM_MR5 = 0;  					//MR5 = duty cycle control, initial = 0%
  PWM_LER = 0x25;                   //latch new values for MR0 and MR2 and MR5
  PWM_PCR = 0x2400;                 //enable PWM2 and in single edge control mode
  PWM_TCR = 0x09;                   //enable PWM and Counter

  // TODO: verify that this function is correctly implemented!
}

void initPins() {

	// connect PWM2 to pin P0.7 - Motor 1 ENABLE (PWM2)
	PINSEL0 &= ~0x0000c000;
	PINSEL0 |= 0x00008000;

	// connect GPIO to P0.12 - Motor 1 PHASE
	PINSEL0 &= ~0x03000000;

	// REMARK: P0.16 is initialized in init_EINT0() - Motor 1 Tacho A (EINT0)
	// REMARK: P0.17 is unused - Motor 1 Tacho A
	// REMARK: P0.18 is unused - Motor 2 Tacho A
	// REMARK: P0.19 is unused - Motor 1 Tacho B
	// REMARK: P0.20 is initialized in init_EINT3() - Motor 2 Tacho B (EINT3)

	// connect PWM5 to P0.21 - Motor 2 ENABLE (PWM5)
	PINSEL1 &= ~0x00000c00;
	PINSEL1 |= 0x00000400;

	// connect GPIO to P0.23 - Motor 2 PHASE
	PINSEL1 &= ~0x0000c000;

	// connect GPIO to P0.25 - Motor 1 BRAKE
	PINSEL1 &= ~0x000c0000;

	// connect GPIO to P0.31 - Motor 2 BRAKE
	PINSEL1 &= ~0xc0000000;

}

void init_EINT0()
{
   PINSEL1 &= ~(1<<0);
   PINSEL1 &= ~(1<<1);
   PINSEL1 |= (1<<0);			// P0.16 is connected to EINT0

   EXTMODE |= 0x00000001;
   EXTPOLAR &= ~(0x00000001);	// sets EINT0 to low-active (falling-edge sensitive)

   EXTINT = 0x00000001;			// (re)activates EINT0
}

void init_EINT3()
{
   PINSEL1 |= (1<<8);
   PINSEL1 |= (1<<9);			// P0.20 is connected to EINT3

   EXTMODE |= 0x00000008;
   EXTPOLAR &= ~(0x00000008);	// sets EINT3 to low-active (falling-edge sensitive)

   EXTINT = 0x00000008;			// (re)activates EINT3
}

void init_vic() {
	// REMARK: Timer1 (used by lightsensor) is already assigned to VICVectCntl7 (priority slot 7)

	VICIntSelect &= ~(0x00024000);		// sets EINT0 and EINT3 as IRQ routines

	VICVectCntl6 = 0x20 | 17;			// assigns EINT3 to priority slot 6 of VIC
	VICVectCntl5 = 0x20 | 14;			// assigns EINT0 to priority slot 5 of VIC

	VICVectAddr6 = (tU32)Tacho2_ISR;	// binds the address of Tacho2_ISR to the VIC
	VICVectAddr5 = (tU32)Tacho1_ISR;	// binds the address of Tacho1_ISR to the VIC
	// REMARK:	Writing to this register does not set the value for future reads from it.
	// 			Rather, this register should be written near the end of an ISR, to update the priority hardware.

	VICIntEnable =	  0x00024000;		// enables EINT0 and EINT3 as interrupts

	// TODO: initialize this IRQ/ISR in main.c
}

void init_io() {
	// REMARK: Note that the direction control register IODIR is effective only when the GPIO function is selected for a pin.

	// OUTPUT INITIALIZATION
	// initializes UT1
	IODIR |= P07;	// ENABLE (P0.7)
	IODIR |= P12;	// PHASE (P0.12)
	IODIR |= P25;	// BRAKE (P0.25)

	// initializes UT2
	IODIR |= P21;	// ENABLE (P0.21)
	IODIR |= P23;	// PHASE (P0.23)
	IODIR |= P31;	// BRAKE (P0.31)


	// INPUT INITIALIZATION
	IODIR &= ~(P16);
	IODIR &= ~(P20);	// makes EINT0 and EINT3 inputs
}




/*****************************************************************************
 *****************************************************************************
 *****************************************************************************/









/*****************************************************************************
 * Functions
 *****************************************************************************/

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
void setPwmDutyPercent1(tU32 dutyValue1)
{
  PWM_MR2 = (PWM_MR0 * dutyValue1) / 10000; //update duty cycle
  PWM_LER = 0x04;                           //latch new values for MR2

  // TODO: verify that this function is correctly implemented!
}

void setPwmDutyPercent2(tU32 dutyValue2)
{
  PWM_MR5 = (PWM_MR0 * dutyValue2) / 10000;  //update duty cycle
  PWM_LER = 0x20;         //latch new values for MR5

  // TODO: verify that this function is correctly implemented!
}

void setMode1(short mode) {
	// COMMENT: PHASE was earlier set to P0.15, this is incorrect!

	if (MODE_FORWARD == 1) {	// MODE = Forward, Fast Current-Decay Mode
		//IODIR |= P07;	// ENABLE (P0.7)
		IOCLR = P07;	// set to L

		//IODIR |= P12;	// PHASE (P0.12)
		IOSET = P12;	// set to H

		//IODIR |= P25;	// BRAKE (P0.25)
		IOSET = P25;	// set to H
	}
	else if (MODE_REVERSE == 2){ // Reverse, Fast Current-Decay Mode
		//IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		//IODIR |= P12; // PHASE (P0.12)
		IOCLR = P12; // set to L

		//IODIR |= P25; // BRAKE (P0.25)
		IOSET = P25; // set to H
	}
	else if (MODE_BRAKE == 3){ // Brake, Fast Current-Decay Mode
		//IODIR |= P07; // ENABLE (P0.7)
		IOCLR = P07; // set to L

		//IODIR |= P12; // PHASE (P0.12)
		IOCLR = P12; // set to L

		//IODIR |= P25; // BRAKE (P0.25)
		IOCLR = P25; // set to L
	}

}

void setMode2(short mode) {

	if (MODE_FORWARD == 1) {	// MODE = Forward, Fast Current-Decay Mode
		//IODIR |= P21;	// ENABLE (P0.21)
		IOCLR = P21;	// set to L

		//IODIR |= P23;	// PHASE (P0.23)
		IOSET = P23;	// set to H

		//IODIR |= P31;	// BRAKE (P0.31)
		IOSET = P31;	// set to H
	}
	else if (MODE_REVERSE == 2){ // Reverse, Fast Current-Decay Mode
		//IODIR |= P21; // ENABLE (P0.21)
		IOCLR = P21; // set to L

		//IODIR |= P23; // PHASE (P0.23)
		IOCLR = P23; // set to L

		//IODIR |= P31; // BRAKE (P0.31)
		IOSET = P31; // set to H
	}

	else if (MODE_BRAKE == 3){ // Brake, Fast Current-Decay Mode
		//IODIR |= P21; // ENABLE (P0.21)
		IOCLR = P21; // set to L

		//IODIR |= P23; // PHASE (P0.23)
		IOCLR = P23; // set to L

		//IODIR |= P31; // BRAKE (P0.31)
		IOCLR = P31; // set to L
	}

}

