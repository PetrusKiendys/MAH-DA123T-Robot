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

//#include "../motor/motor.c"



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
 static tU16 lastSend = 0;
 //int mode1 = 0;
 //int mode2 = 0;



 /* STYR MOTOR MED SWITCH */
 /*
 static int mode1 = -1;
 static int mode2 = -1;
 static int testFlag = 0;
 static int pulsFlag = 0;

 	 // Tryck på switch och text skrivs ut
 	 if(IOPIN & P028){
 		//printf("Klickat på switch ON\n");
 	 		//printf("tempMask = %h. P028 = %h", tempMask, P028);
 	 		 //IOSET = led;	//P0.8 = 1, led off
 			//printf("IOPIN = %x\n", IOPIN);
 		 testFlag = 1;

 	 } else {
    	 //IOCLR = led;	//led on
 		 if( testFlag == 1 ) {
			 mode1 += 2;
			 mode2 += 2;

			 if( mode1 == 5)
				 mode1 = 1;
			 if (mode2 == 5 )
				 mode2 = 1;

			printf("mode1 = %d.\nmode2 = %d\n\n");
			setMode2( mode2 );
			setMode1( mode1 );
			testFlag = 0;
 		 }
 		//delay(1500);
 //		delay_millis(1000);
 		//printf("IOPIN = %d\n", IOPIN);
 		//printf("tempMask = %d. P028 = %d", tempMask, P028);
 	 }


 */
 /*S LUT STYR MOTOR MED SWITCH */



 // FÖR LJUSSENSOR
 //printf("TEST-Interrupt1\n");

/* AD-omvandling av AIN1 (P0.28)  */




 	  //get AIN1 (P0.28)or AIN2 (P0.29)
	//start conversion now (for selected channel)
 	 // Denna rad har vi lagt till.
	ADCR = (ADCR & 0xFFFFFF00) | (1 << 2) | (1 << 24);

	//printf("TEST-Interrupt2\n");
	while((ADDR & 0x80000000) == 0);  //vänta till AD-omvandlaren är klar
	// printf("TEST-Interrupt3\n");
// Ovanstående är egentligen lite farligt, om AD-omvandlaren lägger av hänger sig allt!

//Hämta 16-bitar, skifta till 8-bit integer
	ADvalue=(ADDR>>8) & 0xFF;
	// printf("TEST-Interrupt4\n");



//Test, vi gör inget. Lägger ut samma värde som kom in

	DAvalue = ADvalue;
	// printf("TEST-Interrupt5\n");
//



/* DA-omvandling     */
	 DAvalue=DAvalue & 0xFF;	//maska ut 8 bitar för säkerhets skull
	// printf("TEST-Interrupt6\n");
	DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode
	// printf("TEST-Interrupt7\n");

	//Skriv ut resultat i terminal
//	printf("DAvalue =  %d \nlastSend = %d\n\n", DAvalue, lastSend);
	//setIsValue(DAvalue);

		//printf("TEST-Interrupt8\n");
	//lastRead=DAvalue;

// SLUT LJUSSENSOR + ADOMVANDLING

	// STYR MOTORER MED OMVANDLINGEN
	if( DAvalue != lastSend) {
		if( DAvalue < 80 ) { // Go Forward
			setMode1(1);
			setMode2(1);
		} else if( DAvalue > 130 ) { // Go Backward
			setMode1(3);
			setMode2(3);
		} else { // Break
			setMode1(6);
			setMode2(6);
		}
		lastSend = DAvalue;
	}


//Återställ interruptflaggor
	TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan för MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt




}

