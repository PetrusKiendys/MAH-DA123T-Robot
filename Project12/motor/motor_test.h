/*************************************************************
 *  Filename: motor_test.h
 *  Created on: 2013-01-03
 *  Author: Petrus K. & Ardiana O.
 *  Description: Header file for motor_test.c.
 *************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "motor.h"


/******************************************************************************
 * Defines
 *****************************************************************************/
#define TASK				9		// task 1 - use circulary loop
									// task 2 - use constant PWM signal
									// ...
#define RUN_SETPWM_IN_LOOP	1		// dictates whether setPwmDutyPercentx(tU32) should be run outside the "TASK conditional statement"


/******************************************************************************
 * Prototypes
 *****************************************************************************/
void motorTest_test();
void motorTest_enterLoop(tU32 duty1, tU32 duty2);
void motorTest_pwmMotorInit();
void motorTest_pwmMotorRun(tU32 duty1, tU32 duty2);
void motorTest_changeMode(short mode);
