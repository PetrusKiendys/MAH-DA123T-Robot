/*************************************************************
 *  Filename: motor.c
 *  Author: Embedded Artists AB
 *  Created on: 2005-xx-xx
 *  	Copyright:
 *    	(C) 2005 Embedded Artists AB
 *    	QUESTION: modified EA code belongs to EA?
 *
 *  Modified by: Petrus K. & Ardiana O. (2012-12-03)
 *	Description: Manages and interfaces to motor functionality.
 *************************************************************/
// TODO: rewrite this file as a lib that can be used by other processes and interrupts

/*****************************************************************************
 * Includes
 *****************************************************************************/
#include "motor.h"
#include "../interrupt/interrupts.h"


/*****************************************************************************
 * Functions (initialization)
 *****************************************************************************/
void motor_init() {
	// declare global and local variables
	tU32 freq;

	// initialize I/O
	motor_initIO();

	// initialize PWM
	freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (VPBDIV_FACTOR * 1000));	//set frequency to 1000 Hz (1 kHz)
	motor_initPwm(freq);														//60000 clock pulses per pulse

	// initialize pins (on port P0)
	motor_initPins();

	// initialize VIC
	motor_initVIC();

	//set modes for UT1 and UT2 outputs
	motor_setMode1(MODE_FORWARD);
	motor_setMode2(MODE_FORWARD);

	// initialize EINT (external interrupts)
	motor_initEINT0();
	motor_initEINT3();

	// initializes the default PWM duty
	duty1 = 0;
	duty2 = 0;
	motor_setPwmDutyPercent1(duty1);
	motor_setPwmDutyPercent2(duty2);
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
void motor_initPwm(tU32 initialFreqValue)
{
  PWM_PR  = 0x00000000;             //set prescale to 0
  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
  PWM_MR2 = 0;      				//MR2 = duty cycle control, initial = 0%
  PWM_MR5 = 0;  					//MR5 = duty cycle control, initial = 0%
  PWM_LER = 0x25;                   //latch new values for MR0 and MR2 and MR5
  PWM_PCR = 0x2400;                 //enable PWM2 and in single edge control mode
  PWM_TCR = 0x09;                   //enable PWM and Counter

  // TODO: verify that this function is correctly implemented for our application!
}

// initializes pin functionality
void motor_initPins() {

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

// initializes external interrupt 0
void motor_initEINT0()
{
   PINSEL1 &= ~(1<<0);
   PINSEL1 &= ~(1<<1);
   PINSEL1 |= (1<<0);			// P0.16 is connected to EINT0

   EXTMODE |= 0x00000001;
   EXTPOLAR &= ~(0x00000001);	// sets EINT0 to low-active (falling-edge sensitive)

   EXTINT = 0x00000001;			// (re)activates EINT0
}

// initializes external interrupt 3
void motor_initEINT3()
{
   PINSEL1 |= (1<<8);
   PINSEL1 |= (1<<9);			// P0.20 is connected to EINT3

   EXTMODE |= 0x00000008;
   EXTPOLAR &= ~(0x00000008);	// sets EINT3 to low-active (falling-edge sensitive)

   EXTINT = 0x00000008;			// (re)activates EINT3
}

// initializes the VIC
void motor_initVIC() {
	// REMARK: Timer1 (used by lightsensor) is already assigned to VICVectCntl7 (priority slot 7)

	VICIntSelect &= ~(0x00024000);		// sets EINT0 and EINT3 as IRQ routines

	VICVectCntl6 = 0x20 | 17;			// assigns EINT3 to priority slot 6 of VIC
	VICVectCntl5 = 0x20 | 14;			// assigns EINT0 to priority slot 5 of VIC

	VICVectAddr6 = (tU32)interruptTacho_Tacho2ISR;	// binds the address of Tacho2_ISR to the VIC
	VICVectAddr5 = (tU32)interruptTacho_Tacho1ISR;	// binds the address of Tacho1_ISR to the VIC
	// REMARK:	Writing to this register does not set the value for future reads from it.
	// 			Rather, this register should be written near the end of an ISR, to update the priority hardware.

	VICIntEnable =	  0x00024000;		// enables EINT0 and EINT3 as interrupts

	// TODO: initialize this IRQ/ISR in main.c
}

// initializes GPIO
void motor_initIO() {
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
void motor_setPwmDutyPercent1(tU32 dutyValue1)
{
  PWM_MR2 = (PWM_MR0 * dutyValue1) / 10000; //update duty cycle
  PWM_LER = 0x04;                           //latch new values for MR2

  // TODO: verify that this function is correctly implemented for our application!
}

void motor_setPwmDutyPercent2(tU32 dutyValue2)
{
  PWM_MR5 = (PWM_MR0 * dutyValue2) / 10000;  //update duty cycle
  PWM_LER = 0x20;         //latch new values for MR5

  // TODO: verify that this function is correctly implemented for our application!
}

// Sets motor 1 in forward rotation, reverse rotation or brake
void motor_setMode1(short mode) {

	if (mode == MODE_FORWARD)		{	// Forward, Fast Current-Decay Mode
		IOCLR = P07;	// ENABLE (P0.7) - set to L
		IOSET = P12;	// PHASE (P0.12) - set to H
		IOSET = P25;	// BRAKE (P0.25) - set to H
	}
	else if (mode == MODE_REVERSE)	{	// Reverse, Fast Current-Decay Mode
		IOCLR = P07; // ENABLE (P0.7) - set to L
		IOCLR = P12; // PHASE (P0.12) - set to L
		IOSET = P25; // BRAKE (P0.25) - set to H
	}
	else if (mode == MODE_BRAKE)	{	// Brake, Fast Current-Decay Mode
		IOCLR = P07; // ENABLE (P0.7) - set to L
		IOCLR = P12; // PHASE (P0.12) - set to L
		IOCLR = P25; // BRAKE (P0.25) - set to L
	}

}

// Sets motor 2 in forward rotation, reverse rotation or brake
void motor_setMode2(short mode) {

	if (mode == MODE_FORWARD){		// Forward, Fast Current-Decay Mode
		IOCLR = P21;	// ENABLE (P0.21) - set to L
		IOSET = P23;	// PHASE (P0.23) - set to H
		IOSET = P31;	// BRAKE (P0.31) - set to H
	}
	else if (mode == MODE_REVERSE){	// Reverse, Fast Current-Decay Mode
		IOCLR = P21; // ENABLE (P0.21) - set to L
		IOCLR = P23; // PHASE (P0.23) - set to L
		IOSET = P31; // BRAKE (P0.31) - set to H
	}

	else if (mode == MODE_BRAKE){ // Brake, Fast Current-Decay Mode
		IOCLR = P21; // ENABLE (P0.21) - set to L
		IOCLR = P23; // PHASE (P0.23) - set to L
		IOCLR = P31; // BRAKE (P0.31) - set to L
	}

}

