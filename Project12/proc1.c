/*************************************************************
 *  Filename: proc1.c
 *  Created on: 2011-10-31
 *  Author: Tommy
 *
 *  Modified by: Petrus K. & Ardiana O. (2012-12-10)
 *  Description: Manages process 1.
 *************************************************************/

/*****************************************************************************
 * Includes
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
#include "LCD/LCD.h"
#include "motor/motor_test.h"

extern tCntSem mutexLCD;


/*****************************************************************************
 * Functions
 ****************************************************************************/
void
procEx1(void* arg)
{
  tU8 error;

	for (;;) {	// QUESTION: why use for-loop?

		osSemTake(&mutexLCD, 0, &error);
		motorTest_test();
		osSemGive(&mutexLCD, &error);

		osSleep(100);
	}

}
