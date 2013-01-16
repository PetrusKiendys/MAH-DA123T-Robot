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

// todo: verify that EINT0 and EINT3 (also VIC) is correctly implemented!

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
//#include <lpc2xxx.h>
#include "../startup/lpc2xxx.h"
#include "../startup/config.h"
//#include "../utils/lpc214x.h"


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

//#define TASK	6		// task 1 - use circulary loop
						// task 2 - use constant PWM signal
						// ...
//#define MODE	1		// mode 1 - Forward, Fast Current-Decay Mode
						// mode 2 - Forward, Slow Current-Decay Mode (not implemented)
						// mode 3 - Reverse, Fast Current-Decay Mode
						// mode 4 - Reverse, Slow Current-Decay Mode (not implemented)
						// mode 5 - Brake, Fast Current-Decay Mode
						// mode 6 - Brake, No Current Control		 (not implemented)
//#define MODE2	1
/*
#define FORWARD		1
#define BACKWARD	2
#define BRAKE		3


static int task = 0;
static tU32 duty1 = 0;
static tU32 duty2 = 0;
static tU32 modeVal1 = 0;
static tU32 modeVal2 = 0;
*/
/*****************************************************************************
 * Function prototypes
 ****************************************************************************/

//int runPwm();
//void initPwm(tU32 initialFreqValue);
//void setPwmDutyPercent1(tU32 dutyValue1);
//void setPwmDutyPercent2(tU32 dutyValue2);
////void delayMs(tU16 delayInMs);
//void setMode1(int mode);
//void setMode2(int mode);
//void init_io(void);


//void setTask(int newTask);
//int getTask();
//void dev_run2(int newTask);

//void setDuty1(int newDuty1);
//void setDuty2(int newDuty2);
//void setModeVal1(int newModeVal1);
//void setModeVal2(int newModeVal2);

//int getDuty1();
//int getDuty2();
//int getModeVal1();
//int getModeVal2();

//void delay_millis(long ms);

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
//void initPwm(tU32 initialFreqValue)
//{
//  /*
//   * initialize PWM
//   */
//  PWM_PR  = 0x00000000;             //set prescale to 0
//  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
//  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
//  PWM_MR2 = 0;      				//MR2 = duty cycle control, initial = 0%
//  PWM_MR5 = 0;  					//MR5 = duty cycle control, initial = 0%
//  PWM_LER = 0x25;                   //latch new values for MR0 and MR2 and MR5
//  PWM_PCR = 0x2400;                 //enable PWM2 and in single edge control mode
//  PWM_TCR = 0x09;                   //enable PWM and Counter
//
//
//}

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
void setPwmDutyPercent1(int dutyValue1)
{
	printf("setPwmDutyPercent1, %d\n", dutyValue1);
	PWM_MR2 = (PWM_MR0 * dutyValue1) / 10000; //update duty cycle
	//printf("PWM_MR2 = %d\n", PWM_MR2);
	PWM_LER = 0x04;                           //latch new values for MR2
}

void setPwmDutyPercent2(int dutyValue2)
{
	printf("setPwmDutyPercent2, %d\n", dutyValue2);
	PWM_MR5 = (PWM_MR0 * dutyValue2) / 10000;  //update duty cycle
	//printf("PWM_MR5 = %d\n", PWM_MR5);
	PWM_LER = 0x20;         //latch new values for MR5
}
void
setPwmDuty1(tU32 dutyValue1)
{
  PWM_MR2 = dutyValue1;    //update duty cycle
  PWM_LER = 0x04;         //latch new values for MR2
}
void
setPwmDuty2(tU32 dutyValue2)
{
  PWM_MR5 = dutyValue2;    //update duty cycle
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

/*void delayMs(tU16 delayInMs)
{

  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = delayInMs * ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (1000 * VPBDIV_FACTOR));
  TIMER1_IR  = 0xff;          //reset all interrrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer
  
  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01)
    ;
}*/

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
//void initPins() {
//
//	/*
//	   * connect signal PWM2 to pin P0.7
//	   */
//	  PINSEL0 &= ~0x0000c000;  //clear bits related to P0.7
//	  PINSEL0 |=  0x00008000;  //connect signal PWM2 to P0.7 (second alternative function)
//
//	  PINSEL0 &= ~0x03000000;	// set P0.15 to EINT2
//
//
//	  PINSEL1 &= ~0x000c0000;	// set P0.25 to GPIO Port 0.25
//
//
//	  /*
//	  	   * connect signal PWM5 to pin P0.21
//	  	   */
//
//	  PINSEL1 &= ~0x00000c00; //clear bits related to P0.21
//	  PINSEL1 |= 0x00000400; //connect signal PWM5 to P0.21 (second alternative function)
//
//	  PINSEL1 &= ~0x0000c000; //set P0.23 to GPIO
//
//	  PINSEL1 &= ~0xc0000000; //set P0.31 to GPO Port only
//	  //PINSEL1 |= 0x80000000;
//
//}


// COMMENT: PHASE was earlier set to P0.15, this is incorrect!
void setMode1(int mode) {

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


void setMode2(int mode) {

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
/*
void init_EINT0(void)
{
   PINSEL1 &= ~(1<<0);
   PINSEL1 &= ~(1<<1);
   PINSEL1 |= (1<<0);			// P0.16 is connected to EINT0

   EXTMODE |= 0x00000001;
   EXTPOLAR &= ~(0x00000001);	// sets EINT0 to low-active (falling-edge sensitive)

   EXTINT = 0x00000001;			// (re)activates the interrupt
}

void init_EINT3(void)
{
   PINSEL1 |= (1<<8);
   PINSEL1 |= (1<<9);			// P0.20 is connected to EINT3

   EXTMODE |= 0x00000008;
   EXTPOLAR &= ~(0x00000008);	// sets EINT3 to low-active (falling-edge sensitive)

   EXTINT = 0x00000008;			// (re)activates the interrupt
}

void init_vic(void) {
	VICIntEnable =	  0x00024000;	// sets EINT0 and EINT3 as ISR (IRQ/FIQ routines)
	VICIntSelect &= ~(0x00024000);	// sets EINT0 and EINT3 as IRQ routines
}
*/
/*
int runPwm()
{

  tU32 freq;

  //set frequency to 1000 Hz (1 kHz)
  freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (VPBDIV_FACTOR * 1000));

  //initialize PWM unit
  initPwm(freq);

  initPins();

  //vary duty cycle
 // duty1 = 0;
 // duty2 = 0;

  init_io();

  //init_vic();

  //set modes for UT1 and UT2
  //setMode1(BACKWARD);
  //setMode2(BACKWARD);

//  setMode1(getModeVal1());
//  setMode2(getModeVal2());

  //init_EINT0();
  //init_EINT3();

  //dev_run(getDuty1(), getDuty2());

//  dev_run2(getTask());

  //while(1);

  return 0;
}
*/
/*
void init_io() {
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
*/
/*
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

//	while (1) {

		//set frequency value
			setPwmDutyPercent1(duty1);
			setPwmDutyPercent2(duty2);


			//delayMs(10);

		//switch (TASK) {
		switch (task) {
		case 0: { // BREAK
			duty1 = 10000;	// right fastest
			duty2 = 10000; // left fastest
			break;
		}
		case 1: { // FASTEST
			//update duty cycle (0.00 - 100.00%, in steps of 0.10%)
			//duty1 += 10;
			//if (duty1 > 10000)
			//	duty1 = 0;
			duty1 = 0;	// right fastest
			duty2 = 0; // left fastest
			break;
		}
		case 2: {
			duty1 = 500; 	// right 500
			duty2 = 0; 		// left fastest
			// COMMENT: slowest speed = 8500 duty
			//			fastest speed = 0	 duty
			break;
		}
		case 3: {
			duty1 = 0;		// right fastest
			duty2 = 500;	// left 500
			break;
		}
		case 4: {	// sväng vänster
			duty1 = 1000;
			duty2 = 0;
			break;
		}
		case 5: {	// sväng höger
			duty1 = 0;
			duty2 = 1000;
			break;
		}
		case 6: {
			duty1 = 2000;
			duty2 = 0;
			break;
		}
		case 7: {
			duty1 = 2000;
			duty2 = 0;
			break;
		}
		case 8: {	// sväng vänster
			duty1 = 0;
			duty2 = 4000;
			break;
		}
		case 9: {	// sväng höger
			duty1 = 4000;
			duty2 = 0;
			break;
		}


	//	}
	}
}

void setTask(int newTask)
{
	printf("setTask = %d\n", newTask);
	task = newTask;
}

int getTask()
{
	return task;
}

void dev_run2(int newTask) {
	//wait 10 ms
	//delayMs(10);

	//wait 100 ms
	//delayMs(100);

	// COMMENT: try running without delays
	// 			seems to work fine..

	tU32 duty1;
	tU32 duty2;


	printf("dev_run2 Before while \n");

	//while (1) {

		//set frequency value


			//delayMs(10);

		//switch (TASK) {
		if (newTask == 0) {
			duty1 = 10000;	// right fastest
			duty2 = 10000; // left fastest
		} else if( newTask == 1) { // FASTEST
			//update duty cycle (0.00 - 100.00%, in steps of 0.10%)
			//duty1 += 10;
			//if (duty1 > 10000)
			//	duty1 = 0;
			duty1 = 7000;	// right fastest
			duty2 = 7000; // left fastest
		} else if( newTask == 2) { // FASTEST
			duty1 = 0; 	// right 500
			duty2 = 0; 		// left fastest
			// COMMENT: slowest speed = 8500 duty
			//			fastest speed = 0	 duty
		} else if( newTask == 3) { // FASTEST
			duty1 = 0;		// right fastest
			duty2 = 500;	// left 500

		} else if( newTask == 4) { // FASTEST
			duty1 = 1000;
			duty2 = 0;
		} else if( newTask == 5) { // FASTEST
			duty1 = 0;
			duty2 = 1000;
		} else if( newTask == 6) { // FASTEST
			duty1 = 2000;
			duty2 = 0;
		} else if( newTask == 7) { // FASTEST
			duty1 = 2000;
			duty2 = 0;
		} else if( newTask == 8) { // FASTEST
			printf("Task 8\n");
			duty1 = 7000;
			duty2 = 8000;
		} else if( newTask == 9) { // FASTEST
			printf("Task 9\n");
			duty1 = 8000;
			duty2 = 7000;
		}

		setPwmDutyPercent1(duty1);
		setPwmDutyPercent2(duty2);

//	}
}


void setDuty1(int newDuty1) {
	duty1 = newDuty1;
}
void setDuty2(int newDuty2) {
	duty2 = newDuty2;
}
void setModeVal1(int newModeVal1) {
	printf("setModeVal1 = %d\n", newModeVal1);
	modeVal1 = newModeVal1;
}
void setModeVal2(int newModeVal2) {
	printf("setModeVal2 = %d\n", newModeVal2);
	modeVal2 = newModeVal2;
}

int getDuty1() {
	return duty1;
}
int getDuty2() {
	return duty2;
}
int getModeVal1() {
	return modeVal1;
}
int getModeVal2() {
	return modeVal2;
}

*/
