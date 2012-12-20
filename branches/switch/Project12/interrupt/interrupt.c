/*
 * interrupt.c
 *
 *  Created on: 2011
 *      Author: Tommy
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
#include "../startup/lpc2xxx.h"
#include "../startup/framework.h"
#include "../startup/config.h"
#include "../startup/printf_P.h"
//#include "../lightSensorSwitch/initLightSwitch.c"
#include  "../regulation/regulation.c"


#define P028   0x10000000			// Output Switch
/// Från buttonledc
#define led 0x00000800		//the led is connected to P0.11
#define button 0x00010000	//the button is connected to P0.16

//#define PCB_PINSEL1_P028_MASK     ((unsigned int) 0x03000000)




/*****************************************************************************
 * Exempel på Avbrottsrutin (ljudsamling)
 *
 ****************************************************************************/




void Timer1ISRirq (void)
{
 tU16 ADvalue, DAvalue;
 static int switchCounter = 0; // sets to 0 first time

 	 // Tryck på switch och text skrivs ut
 	 if(IOPIN & P028){
 		printf("Klickat på switch ON\n");
 	 		//printf("tempMask = %h. P028 = %h", tempMask, P028);
 	 		 //IOSET = led;	//P0.8 = 1, led off
 			printf("IOPIN = %x\n", IOPIN);

 	 } else {
    	 //IOCLR = led;	//led on
 		printf("a\na\na\na\na\na\na\na\na\na\na\na\na\na\n");
 		//printf("IOPIN = %d\n", IOPIN);
 		//printf("tempMask = %d. P028 = %d", tempMask, P028);
 	 }


 // SLUT TEST AV SWITCH



//Återställ interruptflaggor
	TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan för MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt




}

