/*************************************************************
 *  Filename: proc2.c
 *  Created on: 2011-10-31
 *  Author: Tommy
 *
 *  Modified by: Petrus K. & Ardiana O. (2012-12-10)
 *  Description: Manages process 2.
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

extern tCntSem mutexLCD;

/*****************************************************************************
 * Functions
 ****************************************************************************/
// This process is not tasked with any routines
void
procEx2(void* arg)
{
  tU8 error;

  for(;;)
  {
    osSemTake(&mutexLCD, 0, &error);
    osSemGive(&mutexLCD, &error);

    osSleep(5000);
  }
}
