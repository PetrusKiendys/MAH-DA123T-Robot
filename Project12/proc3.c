

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

  for(;;)
  {
// Exempelkod för att visa hur flera processer kan dela på LCD
    osSemTake(&mutexLCD, 0, &error);


    static int switchCounter = 0; // sets to 0 first time
 //   delay(delayshort);
 //   send_instruction(1);	//cleara displayn



    // Tryck på switch och text skrivs ut
    	if(IOPIN & P028){
    		//printf("Klickat på switch ON\n");
    		//printf("counter = %d. \n", switchCounter);
    	    //printf("tempMask = %h. P028 = %h", tempMask, P028);
    	    //IOSET = led;	//P0.8 = 1, led off
    	    //printf("IOPIN = %x\n", IOPIN);
    		//delay(delaylong);
    		//send_character('E');
 //   		delay(delaylong);
 //   		send_character('U');

   // 		delay_millis(500);

    	} else {
    	   	//IOCLR = led;	//led on
    		switchCounter++;

    	   //	printf("a\na\na\na\na\na\na\na\na\na\na\na\na\na\n");
    	   	//printf("counterKLICKAD = %d\n", switchCounter);

    		for( i=0; i<switchCounter; i++ ) {
//				delay(delaylong);
//				send_character('I');
    		}
    	   	// Kalla på funktion som ändrar state eftersom countern ökat!

    	   //	delay(delaylong);
    	   //	send_character('X');

    	   	osSleep(100);
    	   	//printf("IOPIN = %d\n", IOPIN);
    	   	//printf("tempMask = %d. P028 = %d", tempMask, P028);
    	}


   	//osSleep(300);



/*
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
    send_character('3');

    osSleep(300);		// för att infon säkert skall hinna synas
*/
    osSemGive(&mutexLCD, &error);

    osSleep(50);
  }
}
