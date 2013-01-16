/*
 * proc1.c
 *
 *  Created on: 31 okt 2011
 *      Author: Tommy
 */

/*****************************************************************************
 * Process 1
 ****************************************************************************/
#include "pre_emptive_os/api/osapi.h"
#include "general.h"
#include "startup/lpc2xxx.h"
#include "startup/printf_P.h"
#include "startup/ea_init.h"
#include "startup/consol.h"
#include "startup/config.h"
#include "startup/framework.h"

#include "utils/utils.h"
#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna
#include "motor/motor.h"
#include "motor/motor_test.h"

extern long const delayshort;
extern long const delaylong;

extern tCntSem mutexLCD;

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void LCD_clearDisplay(void);

/****************************************************************************/

void
procEx1(void* arg)
{
  tU8 error;

	for (;;) {	// QUESTION: why use for-loop?

		osSemTake(&mutexLCD, 0, &error);

		// REMARK: doesn't work at the moment... issues including runPwm()
		// runPwm();

		osSemGive(&mutexLCD, &error);

		osSleep(100);		// this process is not so sleepy, it's a very active process!
	}

}

void LCD_clearDisplay(void) {
	delay(delayshort);
	send_instruction(1);	//cleara displayen
	delay(delaylong);
	send_instruction(2);  	//cursorn till första positionen
}
