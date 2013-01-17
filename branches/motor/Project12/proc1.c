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
#include "LCD/LCD.h"
#include "motor/motor_test.h"



extern tCntSem mutexLCD;

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/


/****************************************************************************/

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


