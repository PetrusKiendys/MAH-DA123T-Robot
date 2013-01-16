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

#include "../LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna

//#include "../motor/motor.c"


/******************************************************************************
 * Defines
 *****************************************************************************/
#define P028   0x10000000			// Output Switch
/// Från buttonledc
#define led 0x00000800		//the led is connected to P0.11
#define button 0x00010000	//the button is connected to P0.16

#define P014   0x00004000			// Output Switch

//#define actualValue = 100;
//#define PCB_PINSEL1_P028_MASK     ((unsigned int) 0x03000000)
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD



extern long const delayshort;
extern long const delaylong;


void osISREnter(void);
void osISRExit(void);




/*****************************************************************************
 * Exempel på Avbrottsrutin (ljudsamling)
 *
 ****************************************************************************/

void Timer1ISRirq (void)
{




 osISREnter();




//Återställ interruptflaggor
    TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan för MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt

	osISRExit();

	}



