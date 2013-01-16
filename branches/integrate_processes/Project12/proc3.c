

/*
 * proc3.c
 *
 *  Created on: 31 okt 2011
 *      Author: Tommy
 */

/*****************************************************************************
 *
 * Exempel 3
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
#include "state/state.c"

#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna


#define P028   0x10000000			// Output Switch

extern long const delayshort;
extern long const delaylong;

extern tCntSem mutexLCD;


void
procEx3(void* arg)
{
  tU8 error;
  int i;
  static int enterFlag = 0;

  for(;;)
  {
// Exempelkod för att visa hur flera processer kan dela på LCD
    osSemTake(&mutexLCD, 0, &error);


    static int switchCounter = 0; // sets to 0 first time
 //   delay(delayshort);
 //   send_instruction(1);	//cleara displayn



    // Tryck på switch och text skrivs ut
    	if(IOPIN & P028){
    		enterFlag = 0;

    	} else if ( enterFlag == 0 ){
    	   	//IOCLR = led;	//led on
    		enterFlag = 1;
    		switchCounter++;
    		if( switchCounter >= 2) {
    			switchCounter = 0;
    		}

    	   //	printf("a\na\na\na\na\na\na\na\na\na\na\na\na\na\n");
    	   	//printf("counterKLICKAD = %d\n", switchCounter);
    		printf("switchCounter = %d.\n", switchCounter);

    		switchState( switchCounter ); // sets new state

    		delay(delayshort);
    		send_instruction(1);	//cleara displayn
    		for( i=0; i<switchCounter; i++ ) {
				delay(delaylong);
				send_character('I');
    		}


    	   	osSleep(600);
    	}


    osSemGive(&mutexLCD, &error);

    osSleep(100);
  }
}
