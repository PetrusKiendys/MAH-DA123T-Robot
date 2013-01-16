/*
 * proc2.c
 *
 *  Created on: 31 okt 2011
 *      Author: Tommy
 */

/*****************************************************************************
 * Process 2
 ****************************************************************************/
#include "pre_emptive_os/api/osapi.h"
#include "general.h"
#include "startup/lpc2xxx.h"
#include "startup/printf_P.h"
#include "startup/ea_init.h"
#include "startup/consol.h"
#include "startup/config.h"
#include "startup/framework.h"

#include "utils/utils.c"
#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna

extern tCntSem mutexLCD;


void
procEx2(void* arg)
{
  tU8 error;

  for(;;)
  {
    osSemTake(&mutexLCD, 0, &error);
    osSemGive(&mutexLCD, &error);

    osSleep(5000);		// this process is very sleepy and sleeps for a long time
  }
}

