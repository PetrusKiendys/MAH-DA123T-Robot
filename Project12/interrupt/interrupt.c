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



 // FÖR LJUSSENSOR
 //printf("TEST-Interrupt1\n");

/* AD-omvandling av AIN1 (P0.28)  */



/*
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

*/

/* DA-omvandling     */
	/* DAvalue=DAvalue & 0xFF;	//maska ut 8 bitar för säkerhets skull
	// printf("TEST-Interrupt6\n");
	DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode
	// printf("TEST-Interrupt7\n");

	//Skriv ut resultat i terminal
	printf(" %d \n",DAvalue);
	setIsValue(DAvalue);

		//printf("TEST-Interrupt8\n");
	//lastRead=DAvalue;

// SLUT LJUSSENSOR + ADOMVANDLING
*/

//Återställ interruptflaggor
	TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan för MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt




}

