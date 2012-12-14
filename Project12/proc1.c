/*
 * proc1.c
 *
 *  Created on: 31 okt 2011
 *      Author: Tommy
 */

/*****************************************************************************
 *
 *    Exempel 1
 *
 *
 ****************************************************************************/
#include "pre_emptive_os/api/osapi.h"
#include "general.h"
#include "startup/lpc2xxx.h"
#include "startup/printf_P.h"
#include "startup/ea_init.h"
#include "startup/consol.h"
#include "startup/config.h"
#include "startup/framework.h"

#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna

extern long const delayshort;
extern long const delaylong;

// TODO: temporary extern declarations.. (make these local later if possible)
extern tU32 freq;
extern tU32 duty;

extern tCntSem mutexLCD;

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
static void setPwmDutyPercent(tU32 dutyValue);
static void setPwmDuty(tU32 dutyValue);

/****************************************************************************/

void
procEx1(void* arg)
{
  tU8 error;

  for(;;)
  {
// Exempelkod för att visa hur flera processer kan dela på LCD

   osSemTake(&mutexLCD, 0, &error);

	delay(delayshort);
	send_instruction(1);	//cleara displayen
	delay(delaylong);
	send_instruction(2);  //cursorn till första positionen

	//vary duty cycle
	  duty = 0;

	  while(1)
	  {
	    //set frequency value
	    setPwmDutyPercent(duty);

	    //wait 10 ms
	    delay_millis(10);

	    //update duty cycle (0.00 - 100.00%, in steps of 0.10%)
	    duty += 10;
	    if (duty > 10000)
	      duty = 0;
	  }

    osSemGive(&mutexLCD, &error);


    osSleep(1000);
  }
}

/*****************************************************************************
 *
 * Description:
 *    Update the duty cycle value of the PWM signal.
 *
 * Params:
 *    [in] dutyValue - the new duty cycle value in percent in increments
 *                     of 0.01% (i.e., 10% = 1000).
 *
 ****************************************************************************/
static void
setPwmDutyPercent(tU32 dutyValue)
{
  PWM_MR2 = (PWM_MR0 * dutyValue) / 10000;  //update duty cycle
  PWM_LER = 0x04;                           //latch new values for MR2
}

/*****************************************************************************
 *
 * Description:
 *    Update the duty cycle value of the PWM signal.
 *
 * Params:
 *    [in] dutyValue - the new duty cycle value. Value calculated as:
 *    (crystal frequency * PLL multiplication factor)
 *    ----------------------------------------------- * duty cycle in percent
 *           (VPBDIV factor * PWM frequency)
 *
 *                    or
 *
 *    value in MR0 register * duty cycle in percent
 *
 ****************************************************************************/
static void
setPwmDuty(tU32 dutyValue)
{
  PWM_MR2 = dutyValue;    //update duty cycle
  PWM_LER = 0x04;         //latch new values for MR2
}
