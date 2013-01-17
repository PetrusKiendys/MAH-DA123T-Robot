/*************************************************************
 *  Filename: interrupt_tacho.c
 *  Created on: 2013-01-14
 *  Author: Petrus K. & Ardiana O.
 *	Description: Manages the tachometer ISR.
 *************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "interrupts.h"


/******************************************************************************
 * ISR
 *****************************************************************************/
void interruptTacho_Tacho1ISR (void) {
	printf("\n\nTacho1 ISR: %d", tacho1_counter);	// prints the tacho1 counter value everytime the interrupt is trigger
													// note that this causes an overhead and is not preferable in performance critical applications (when not testing code)
	tacho1_counter++;								// increments the tacho1 counter

	EXTINT |= 0x00000009;			// (re)activates EINT0 and EINT3
	VICVectAddr = 0x00;				//dummy write to VIC to signal end of interrupt
									// QUESTION: have to use this statement?
									// EDIT: apparently so, see p. 58 4.12 Vector Address register of manualLPC2148.pdf for more info
									//		 "Writing to this register does not set the value for future reads from it."
									// 		 "Rather, this register should be written near the end of an ISR, to update the priority hardware."
}

void interruptTacho_Tacho2ISR (void) {
	printf("\nTacho2 ISR: %d", tacho2_counter);		// prints the tacho2 counter value everytime the interrupt is trigger
													// note that this causes an overhead and is not preferable in performance critical applications (when not testing code)
	tacho2_counter++;								// increments the tacho2 counter

	EXTINT |= 0x00000009;			// (re)activates EINT0 and EINT3
	VICVectAddr = 0x00;				//dummy write to VIC to signal end of interrupt
									// QUESTION: have to use this statement?
									// EDIT: apparently so, see p. 58 4.12 Vector Address register of manualLPC2148.pdf for more info
									//		 "Writing to this register does not set the value for future reads from it."
									// 		 "Rather, this register should be written near the end of an ISR, to update the priority hardware."
}
