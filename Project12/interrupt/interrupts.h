/*************************************************************
 *  Filename: interrupts.h
 *  Created on: 2013-01-14
 *  Author: Petrus K. & Ardiana O.
 *  Description: Header file for interrupts.
 *************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
#include "../startup/lpc2xxx.h"
#include "../startup/framework.h"
#include "../startup/config.h"
#include "../startup/printf_P.h"


/******************************************************************************
 * Prototypes
 *****************************************************************************/
void interruptTacho_Tacho1ISR (void);
void interruptTacho_Tacho2ISR (void);
void interruptTimer_Timer1ISR (void);
void interruptTimer_readAIN1();

/******************************************************************************
 * Global variables
 *****************************************************************************/
static tU32 tacho1_counter = 0;
static tU32 tacho2_counter = 0;
