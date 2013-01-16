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

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
#include <lpc2xxx.h>
#include "../startup/config.h"
#include "../utils/lpc214x.h"
#include "../interrupt/interrupt_tacho.c"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define P07 (1 << 7)
#define P12	(1 << 12)
#define P16 (1 << 16)
#define P20 (1 << 20)
#define P21 (1 << 21)
#define	P23 (1 << 23)
#define	P25	(1 << 25)
#define	P31 (1 << 31)

#define TASK	9		// task 1 - use circulary loop
						// task 2 - use constant PWM signal
						// ...
//#define MODE	1		// mode 1 - Forward, Fast Current-Decay Mode
						// mode 2 - Forward, Slow Current-Decay Mode (not implemented)
						// mode 3 - Reverse, Fast Current-Decay Mode
						// mode 4 - Reverse, Slow Current-Decay Mode (not implemented)
						// mode 5 - Brake, Fast Current-Decay Mode
						// mode 6 - Brake, No Current Control		 (not implemented)
//#define MODE2	1

#define MODE_FORWARD		1
#define MODE_BACKWARD	2
#define MODE_BRAKE		3

#define RUN_SETPWM_IN_LOOP	1	// dictates whether setPwmDutyPercentx(tU32) should be run outside the "TASK conditional statement"

/*****************************************************************************
 * Global variables
 ****************************************************************************/
static tU32 duty1;
static tU32 duty2;

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
static void initPwm(tU32 initialFreqValue);
static void setPwmDutyPercent1(tU32 dutyValue1);
static void setPwmDutyPercent2(tU32 dutyValue2);
void setMode1(short mode);
void setMode2(short mode);
void init_io(void);
void init(void);
//----------- DEV functions ------------------//
void dev_run(tU32 duty1, tU32 duty2);
void pwm_motor_init(void);
void pwm_motor_run(tU32 duty1, tU32 duty2);
void change_mode(short mode);
//----------- end of DEV functions -----------//
void delay_millis(long ms);





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
static void initPwm(tU32 initialFreqValue)
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
static void setPwmDutyPercent1(tU32 dutyValue1)
{
  PWM_MR2 = (PWM_MR0 * dutyValue1) / 10000; //update duty cycle
  PWM_LER = 0x04;                           //latch new values for MR2

  // TODO: verify that this function is correctly implemented!
}

static void setPwmDutyPercent2(tU32 dutyValue2)
{
  PWM_MR5 = (PWM_MR0 * dutyValue2) / 10000;  //update duty cycle
  PWM_LER = 0x20;         //latch new values for MR5

  // TODO: verify that this function is correctly implemented!
}


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

void setMode1(short mode) {
	// COMMENT: PHASE was earlier set to P0.15, this is incorrect!

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

void init_EINT0(void)
{
   PINSEL1 &= ~(1<<0);
   PINSEL1 &= ~(1<<1);
   PINSEL1 |= (1<<0);			// P0.16 is connected to EINT0

   EXTMODE |= 0x00000001;
   EXTPOLAR &= ~(0x00000001);	// sets EINT0 to low-active (falling-edge sensitive)

   EXTINT = 0x00000001;			// (re)activates EINT0
}

void init_EINT3(void)
{
   PINSEL1 |= (1<<8);
   PINSEL1 |= (1<<9);			// P0.20 is connected to EINT3

   EXTMODE |= 0x00000008;
   EXTPOLAR &= ~(0x00000008);	// sets EINT3 to low-active (falling-edge sensitive)

   EXTINT = 0x00000008;			// (re)activates EINT3
}

void init_vic(void) {
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

int runPwm() {

	init();
	dev_run(duty1, duty2);

	return 0;
}





// dev function - not used in release
// COMMENT: PWM signal is continuously generated when the setPwmDutyPercent(tU32) function is called, no need to endlessly iterate the calls
// EDIT:  	seems like you need to iterate over the setPwmDutyPercent(tU32) functions afterall...
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

		//	delayMs(10);

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
				setMode1(MODE_FORWARD);
				setMode2(MODE_FORWARD);
				duty1 = duty_vals;
				duty2 = duty_vals;
				setPwmDutyPercent1(duty1);
				setPwmDutyPercent2(duty2);
				//delayMs(0);
				delay_millis(delay);
			}

			if (section[1] == 1) {
				setMode1(MODE_BRAKE);
				setMode2(MODE_BRAKE);
				setPwmDutyPercent1(duty1);
				setPwmDutyPercent2(duty2);
				delay_millis(delay);
			}

			if (section[2] == 1) {
				setMode1(MODE_BACKWARD);
				setMode2(MODE_BACKWARD);
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

			//delayMs(10);
			delay_millis(10);

			pwm_motor_run(duty1, duty2);
			break;
		}
		case 8: {


			setMode1(MODE_FORWARD);
			setMode2(MODE_FORWARD);

			delay_millis(700);

			duty1 = 7000;
			duty2 = 7000;


			setMode1(MODE_BACKWARD);
			setMode2(MODE_BACKWARD);

			delay_millis(700);

			duty1 = 1000;
			duty2 = 1000;

			break;
		}

		case 9: {
			duty1 = 0;
			duty2 = 0;
			break;
		}

		}
	}
}

/*****************************************************************************
 * Temporary developer functions
 ****************************************************************************/
void pwm_motor_init() {
	setMode1(MODE_FORWARD);
	setMode2(MODE_FORWARD);

	delay_millis(10);
}

void pwm_motor_run(tU32 duty1, tU32 duty2) {
	setPwmDutyPercent1(duty1);
	setPwmDutyPercent2(duty2);

	while(1) {
		change_mode(MODE_FORWARD);
		change_mode(MODE_BRAKE);
		change_mode(MODE_BACKWARD);
		change_mode(MODE_BRAKE);
	}
}

void change_mode(short mode) {
	setMode1(mode);
	setMode2(mode);
	delay_millis(10);
}
