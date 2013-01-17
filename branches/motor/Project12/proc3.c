/*
 * proc3.c
 *
 *  Created on: 31 okt 2011
 *      Author: Tommy
 */

/*****************************************************************************
 * Process 3
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

extern tCntSem mutexLCD;

/*****************************************************
 * This process is not tasked with any routines
 *****************************************************/
void
procEx3(void* arg)
{
  tU8 error;

  for(;;)
  {
    osSemTake(&mutexLCD, 0, &error);
    osSemGive(&mutexLCD, &error);

    osSleep(5000);
  }
}
