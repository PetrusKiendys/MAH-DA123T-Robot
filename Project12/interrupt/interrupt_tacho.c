/*************************************************************
 *  Filename: interrupt_tacho.c
 *  Created on: 2013-01-14
 *  Author: Petrus K
 *************************************************************/

/******************************************************************************
 * Includes, defines, variable declarations and prototypes
 *****************************************************************************/
#include "../general.h"
#include "../startup/lpc2xxx.h"
#include "../startup/framework.h"
#include "../startup/config.h"
#include "../startup/custom_settings.h"

void Tacho1_ISR(void);
void Tacho2_ISR(void);

static tU32 tacho1_counter = 0;
static tU32 tacho2_counter = 0;

#define DEBUG				CUSTOMSETTINGS_DEBUG


/******************************************************************************
 * ISR
 *****************************************************************************/
void Tacho1_ISR(void) {
	// perform this when Tacho1 is triggered
	if (DEBUG) {
		printf("\n\nTacho1 ISR: %d", tacho1_counter);
		tacho1_counter++;
	}

	EXTINT = 0x00000009;			// (re)activates EINT0 and EINT3
	VICVectAddr = 0x00;				//dummy write to VIC to signal end of interrupt
									// QUESTION: have to use this statement?
									// EDIT: apparently so, see p. 58 4.12 Vector Address register of manualLPC2148.pdf for more info
									//		 "Writing to this register does not set the value for future reads from it."
									// 		 "Rather, this register should be written near the end of an ISR, to update the priority hardware."
}

void Tacho2_ISR(void) {
	// perform this when Tacho2 is triggered
	if (DEBUG) {
		printf("\nTacho2 ISR: %d", tacho2_counter);
		tacho2_counter++;
	}

	EXTINT = 0x00000009;			// (re)activates EINT0 and EINT3
	VICVectAddr = 0x00;				//dummy write to VIC to signal end of interrupt
									// QUESTION: have to use this statement?
									// EDIT: apparently so, see p. 58 4.12 Vector Address register of manualLPC2148.pdf for more info
									//		 "Writing to this register does not set the value for future reads from it."
									// 		 "Rather, this register should be written near the end of an ISR, to update the priority hardware."
}
