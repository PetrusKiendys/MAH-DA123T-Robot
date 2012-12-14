/*
 * proc2.c
 *
 *  Created on: 31 okt 2011
 *      Author: Tommy
 */

/*****************************************************************************
 *
 * Exempel 2
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
#include "utils/utils.c"

#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna

extern long const delayshort;
extern long const delaylong;

extern tCntSem mutexLCD;


void
procEx2(void* arg)
{
  tU8 error;

  for(;;)
  {
// Exempelkod för att visa hur flera processer kan dela på LCD
    osSemTake(&mutexLCD, 0, &error);

    delay(delayshort);
	send_instruction(1);	//cleara displayn
	delay(delaylong);
	send_instruction(2);  //cursorn till första positionen
	delay(delaylong);
    send_character('E');
    delay(delayshort);
    send_character('x');
    delay(delayshort);
    send_character('e');
    delay(delayshort);
    send_character('m');
    delay(delayshort);
    send_character('p');
    delay(delayshort);
    send_character('e');
    delay(delayshort);
    send_character('l');
    delay(delayshort);
    send_character('2');

    osSleep(300);		// för att infon säkert skall hinna synas

    osSemGive(&mutexLCD, &error);

    osSleep(2000);
  }
}

