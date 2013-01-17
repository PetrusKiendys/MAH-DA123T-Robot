// DEPRECATED: this file doesn't work, can be removed from the project

/*
 * utils.c
 *
 *  Created on: 14 dec 2012
 *      Author: Petrus K. & Ardiana O.
 */
#include "utils.h"

// ********************************************
// Delay function written in assembly code
// ********************************************

//	ERROR: the delay functions below generate the following error:
//	Error: instruction not supported in Thumb16 mode -- `subs R0,R0,#1'
//	make: *** [main.o] Error 1

//										// Med CCLK=60 MHz ger denna rutin en delay på pulses*66,666 ns + anropstiden
//void delay(long pulses) {               // pulses is stored in R0 when this function is called
//	asm("wait:   SUBS R0, R0, #1");     // the label "wait" is declared, decrements the contents of R0 by 1
//	asm("BNE wait");                    // branches to the label "wait" unless the contents of R0 equals 0
//}
//
//void delay_mikros(long us) {            // delays by the provided parameter (in mikroseconds)
//    delay(us*15);
//}
//
//void delay_millis(long ms) {            // delays by the provided parameter (in milliseconds)
//    delay(ms*15000);
//}
//
//void delay_secs(long secs) {              // delays by the provided parameter (in seconds)
//    delay(secs*15000000);
//}
