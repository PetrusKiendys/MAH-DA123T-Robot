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

extern tCntSem mutexLCD;



void
procEx1(void* arg)
{
  tU8 error;

  for(;;)
  {
// Exempelkod för att visa hur flera processer kan dela på LCD

   osSemTake(&mutexLCD, 0, &error);

//	delay(delayshort);
//	send_instruction(1);	//cleara displayen
//	delay(delaylong);
//	send_instruction(2);  //cursorn till första positionen
//	delay(delaylong);
//    send_character('E');
//    delay(delayshort);
//    send_character('x');
//    delay(delayshort);
//
//    send_character('1');
//
//    osSleep(300);		// för att infon säkert skall hinna synas

    osSemGive(&mutexLCD, &error);


    //osSleep(1000);
    osSleep(5000);
  }
}
