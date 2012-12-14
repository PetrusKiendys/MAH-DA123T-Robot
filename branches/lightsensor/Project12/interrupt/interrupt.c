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















/*****************************************************************************
 * Exempel p� Avbrottsrutin (ljudsamling)
 *
 ****************************************************************************/





void Timer1ISRirq (void)
{
 tU16 ADvalue, DAvalue, lastRead;

// printf("TEST-Interrupt1\n");

/* AD-omvandling av AIN1 (P0.28)  */




 	  //get AIN1 (P0.28)or AIN2 (P0.29)
	//start conversion now (for selected channel)
 	 // Denna rad har vi lagt till.
	ADCR = (ADCR & 0xFFFFFF00) | (1 << 1) | (1 << 24);

	// printf("TEST-Interrupt2\n");
	while((ADDR & 0x80000000) == 0);  //v�nta till AD-omvandlaren �r klar
	// printf("TEST-Interrupt3\n");
// Ovanst�ende �r egentligen lite farligt, om AD-omvandlaren l�gger av h�nger sig allt!

//H�mta 16-bitar, skifta till 8-bit integer
	ADvalue=(ADDR>>8) & 0xFF;
	// printf("TEST-Interrupt4\n");



//Test, vi g�r inget. L�gger ut samma v�rde som kom in

	lastRead = DAvalue;
	DAvalue = ADvalue;
	// printf("TEST-Interrupt5\n");
//



/* DA-omvandling     */
	DAvalue=DAvalue & 0xFF;	//maska ut 8 bitar f�r s�kerhets skull
	// printf("TEST-Interrupt6\n");
	DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode
//	 printf("TEST-Interrupt7\n");

	//Skriv ut resultat i terminal
	if( DAvalue != lastRead )
		printf(" %d \n",DAvalue);

//	 printf("TEST-Interrupt8\n");
	//lastRead=DAvalue;


//�terst�ll interruptflaggor
	TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan f�r MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt




}

