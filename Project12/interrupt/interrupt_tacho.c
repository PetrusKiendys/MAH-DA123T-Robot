/*************************************************************
 *  Filename: interrupt_tacho.c
 *  Created on: 2013-01-14
 *  Author: Petrus K
 *************************************************************/

#include "interrupts.h"

static tU32 tacho1_counter;
static tU32 tacho2_counter;


/******************************************************************************
 * ISR
 *****************************************************************************/
void Tacho1_ISR (void) {
	// perform this when Tacho1 is triggered
	printf("\n\nTacho1 ISR: %d", tacho1_counter);
	tacho1_counter++;

	// don't forget to reset the interrupt flag!
	EXTINT |= 0x00000009;			// (re)activates EINT0 and EINT3

	VICVectAddr = 0x00;				//dummy write to VIC to signal end of interrupt
									// QUESTION: have to use this statement?
									// EDIT: apparently so, see p. 58 4.12 Vector Address register of manualLPC2148.pdf for more info
									//		 "Writing to this register does not set the value for future reads from it."
									// 		 "Rather, this register should be written near the end of an ISR, to update the priority hardware."
}

void Tacho2_ISR (void) {
	// perform this when Tacho2 is triggered
	printf("\nTacho2 ISR: %d", tacho2_counter);
	tacho2_counter++;

	// don't forget to reset the interrupt flag!
	EXTINT |= 0x00000009;			// (re)activates EINT0 and EINT3

	VICVectAddr = 0x00;				//dummy write to VIC to signal end of interrupt
									// QUESTION: have to use this statement?
									// EDIT: apparently so, see p. 58 4.12 Vector Address register of manualLPC2148.pdf for more info
									//		 "Writing to this register does not set the value for future reads from it."
									// 		 "Rather, this register should be written near the end of an ISR, to update the priority hardware."
}
