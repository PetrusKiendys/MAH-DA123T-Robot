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



/*****************************************************************************
 * Exempel p� avbrottsrutin (ljudsamling)
 *
 ****************************************************************************/


void Timer1ISRirq (void)
{
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
//


/* DA-omvandling     */
	DAvalue=DAvalue & 0xFF;	//maska ut 8 bitar f�r s�kerhets skull
	DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode


//�terst�ll interruptflaggor
	TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan f�r MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt



}

