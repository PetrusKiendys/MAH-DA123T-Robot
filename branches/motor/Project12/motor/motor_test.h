
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
void dev_run(tU32 duty1, tU32 duty2);
void pwm_motor_init();
void pwm_motor_run(tU32 duty1, tU32 duty2);
void change_mode(short mode);
