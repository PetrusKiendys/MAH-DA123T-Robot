
#include "motor_test.h"

/******************************************************************************
 * Functions
 *****************************************************************************/
void runPwm() {

	init();
	dev_run(duty1, duty2);

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
				setMode1(MODE_REVERSE);
				setMode2(MODE_REVERSE);
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


			setMode1(MODE_REVERSE);
			setMode2(MODE_REVERSE);

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
		change_mode(MODE_REVERSE);
		change_mode(MODE_BRAKE);
	}
}

void change_mode(short mode) {
	setMode1(mode);
	setMode2(mode);
	delay_millis(10);
}
