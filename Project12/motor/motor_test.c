/*************************************************************
 *  Filename: motor_test.c
 *  Created on: 2013-01-03
 *  Author: Petrus K. & Ardiana O.
 *	Description: Test file for trying out motor.c functionality.
 *	REMARK: This file is only used for testing, it should not be treated as an integral part of the application and primarily used for testing and debugging
 *************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "motor_test.h"


/******************************************************************************
 * Functions
 *****************************************************************************/
void motorTest_test() {
	motor_init();
	motorTest_enterLoop(duty1, duty2);		// REMARK: duty1 and duty2 have to be initialized, this is done in motor_init()
}



// dev function - not used in release
// COMMENT: PWM signal is continuously generated when the setPwmDutyPercent(tU32) function is called, no need to endlessly iterate the calls
// EDIT:  	seems like you need to iterate over the setPwmDutyPercent(tU32) functions afterall...
// COMMENT: the duty sets the speed of the motor driver
// 			the value that can be set is 0-10000
// 			a higher value means a lower speed, conversely a low value means a higher speed
void motorTest_enterLoop(tU32 duty1, tU32 duty2) {
	// COMMENT: try running without delays
	// EDIT:	seems to work fine..
	//delayMs(10);		//wait 10 ms
	//delayMs(100);		//wait 100 ms

	while (1) {

		//set frequency value
		if (RUN_SETPWM_IN_LOOP == 1) {
			motor_setPwmDutyPercent1(duty1);
			motor_setPwmDutyPercent2(duty2);
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
		case 3: {
			duty1 = 6000;
			duty2 = 8000;
			break;
		}
		case 4: {
			duty1 = 8000;
			duty2 = 6000;
			break;
		}
		case 5: {
			duty1 = 6000;
			duty2 = 6000;
			break;
		}

		// COMMENT: this "task" does not seem to work...
		case 6: {
			short delay = 1;
			short duty_vals = 6000;
			short section[3] = { 1, 0, 0 };

			if (section[0] == 1) {
				motor_setMode1(MODE_FORWARD);
				motor_setMode2(MODE_FORWARD);
				duty1 = duty_vals;
				duty2 = duty_vals;
				motor_setPwmDutyPercent1(duty1);
				motor_setPwmDutyPercent2(duty2);
				delay_millis(delay);
			}

			if (section[1] == 1) {
				motor_setMode1(MODE_BRAKE);
				motor_setMode2(MODE_BRAKE);
				motor_setPwmDutyPercent1(duty1);
				motor_setPwmDutyPercent2(duty2);
				delay_millis(delay);
			}

			if (section[2] == 1) {
				motor_setMode1(MODE_REVERSE);
				motor_setMode2(MODE_REVERSE);
				duty1 = duty_vals;
				duty2 = duty_vals;
				motor_setPwmDutyPercent1(duty1);
				motor_setPwmDutyPercent2(duty2);
				delay_millis(delay);
			}
			break;
		}
		case 7: {
			motorTest_pwmMotorInit();
			duty1 = 6000;
			duty2 = 6000;

			delay_millis(10);

			motorTest_pwmMotorRun(duty1, duty2);
			break;
		}
		case 8: {


			motor_setMode1(MODE_FORWARD);
			motor_setMode2(MODE_FORWARD);

			delay_millis(700);

			duty1 = 7000;
			duty2 = 7000;


			motor_setMode1(MODE_REVERSE);
			motor_setMode2(MODE_REVERSE);

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


/******************************************************************************
 * Functions (experimental)
 *****************************************************************************/
// trying an alternate way of initializing the pwm
void motorTest_pwmMotorInit() {
	motor_setMode1(MODE_FORWARD);
	motor_setMode2(MODE_FORWARD);

	delay_millis(10);
}

void motorTest_pwmMotorRun(tU32 duty1, tU32 duty2) {
	motor_setPwmDutyPercent1(duty1);
	motor_setPwmDutyPercent2(duty2);

	while(1) {
		motorTest_changeMode(MODE_FORWARD);
		motorTest_changeMode(MODE_BRAKE);
		motorTest_changeMode(MODE_REVERSE);
		motorTest_changeMode(MODE_BRAKE);
	}
}

void motorTest_changeMode(short mode) {
	motor_setMode1(mode);
	motor_setMode2(mode);
	delay_millis(10);
}
