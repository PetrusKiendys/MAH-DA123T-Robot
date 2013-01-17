/*************************************************************
 *  Filename: interrupt_timer.c
 *  Created on: 2011
 *  Author: Tommy
 *
 *  Modified by: Petrus K. & Ardiana O. (2013-01-17)
 *  Description: Manages the Timer1 ISR.
 *************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "interrupts.h"


/******************************************************************************
 * ISR
 *****************************************************************************/
void interruptTimer_Timer1ISR (void)
{
	if (TRUE) {	// turning on/off the AD-conversion
		interruptTimer_readAIN1();
	}
}


/******************************************************************************
 * Functions
 *****************************************************************************/
// Exempel på avbrottsrutin (ljudsampling)
void interruptTimer_readAIN1() {

	 tU16 ADvalue, DAvalue;

	/* AD-omvandling av AIN1 (P0.28)  */

	 //start conversion av AIN1
		ADCR = (ADCR & 0xFFFFFF02)| (1 << 1) |(1 << 24);
		while((ADDR & 0x80000000) == 0);  //vänta till AD-omvandlaren är klar
	// Ovanstående är egentligen lite farligt, om AD-omvandlaren lägger av hänger sig allt!

	//Hämta 16-bitar, skifta till 8-bit integer
		ADvalue=(ADDR>>8) & 0xFF;

	//Test, vi gör inget. Lägger ut samma värde som kom in
		DAvalue=ADvalue;

	/* DA-omvandling     */
		DAvalue=DAvalue & 0xFF;	    //maska ut 8 bitar för säkerhets skull
		DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode

	//Återställ interruptflaggor
		TIMER1_IR |= 0x00000001;            //reseta interrrupt flaggan för MR0
		VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt
}
