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
// Exempel p� avbrottsrutin (ljudsampling)
void interruptTimer_readAIN1() {

	 tU16 ADvalue, DAvalue;

	/* AD-omvandling av AIN1 (P0.28)  */

	 //start conversion av AIN1
		ADCR = (ADCR & 0xFFFFFF02)| (1 << 1) |(1 << 24);
		while((ADDR & 0x80000000) == 0);  //v�nta till AD-omvandlaren �r klar
	// Ovanst�ende �r egentligen lite farligt, om AD-omvandlaren l�gger av h�nger sig allt!

	//H�mta 16-bitar, skifta till 8-bit integer
		ADvalue=(ADDR>>8) & 0xFF;

	//Test, vi g�r inget. L�gger ut samma v�rde som kom in
		DAvalue=ADvalue;

	/* DA-omvandling     */
		DAvalue=DAvalue & 0xFF;	    //maska ut 8 bitar f�r s�kerhets skull
		DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode

	//�terst�ll interruptflaggor
		TIMER1_IR |= 0x00000001;            //reseta interrrupt flaggan f�r MR0
		VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt
}
