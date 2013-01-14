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
	int ADvalue, DAvalue;
 int actual, diff; // desired
 static int desired = 160;		// börvärde

 int first, second, third;
 char aaa;
 char bbb;
 char ccc;
 static int LCDCounter = 0;


 osISREnter();


 // FÖR LJUSSENSOR
 //printf("TEST-Interrupt1\n");

/* AD-omvandling av AIN1 (P0.28)  */

/* EA5
   ADvalue = ADDR;
   ADCR = (ADCR & 0xFFFFFF00) | (1 << 2) | (1 << 24);
   while ((ADDR & 0x80000000) == 0);
   ADvalue = (ADDR >> 6) & 0x3FF;
 */



 	  //get AIN1 (P0.28)or AIN2 (P0.29)
	//start conversion now (for selected channel)
 	// Denna rad har vi lagt till.
	ADCR = (ADCR & 0xFFFFFF00) | (1 << 2) | (1 << 24);

	while((ADDR & 0x80000000) == 0);  //vänta till AD-omvandlaren är klar
// Ovanstående är egentligen lite farligt, om AD-omvandlaren lägger av hänger sig allt!

	//Hämta 16-bitar, skifta till 8-bit integer
	ADvalue=(ADDR>>8) & 0xFF;

//Test, vi gör inget. Lägger ut samma värde som kom in
	DAvalue = ADvalue;

/* DA-omvandling     */
	 DAvalue=DAvalue & 0xFF;	//maska ut 8 bitar för säkerhets skull
	 DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode

// SLUT LJUSSENSOR + ADOMVANDLING



	if(IOPIN & P014){		// ej intryckt

	} else {				// P014 intryckt
		setPwmDutyPercent1(10000);	// sätter hastighet till 0
		setPwmDutyPercent1(10000);	// sätter hastighet till 0
		desired = DAvalue;			// desired (börvärde) blir avläsningen

		// Printar meddelande på displayen så vi vet att initiering gjorts
		delay(delayshort);
		send_instruction(1);	//cleara displayn
		delay(delaylong);
		send_character('N');
		delay(delaylong);
		send_character('E');
		delay(delaylong);
		send_character('W');
		delay(delaylong);
		send_character(' ');
		delay(delaylong);
		send_character('D');
		delay(delaylong);
		send_character('E');
		delay(delaylong);
		send_character('S');
	}



	actual = DAvalue;		 // actual (ärvärde) blir det ljussensorn läser av
	diff = actual - desired; // differens mellan är- och börvärde (pos: svart, neg: vit)

	first = DAvalue / 100;			// 100-talssiffran från DAValue
	second = (DAvalue % 100) / 10; 	// 10-talssiffran från DAValue
	third = DAvalue % 10;			// 1-talssiffran från DAValue

	LCDCounter++; 					// ökar bara en räknare med 1

	aaa = '0' + first;				// gör 100-talssifran till en char
	bbb = '0' + second;				// gör 10-talssifran till en char
	ccc = '0' + third;				// gör 1-talssifran till en char

	if( (LCDCounter == 100) ) {	// om räknaren gått till 100 (skriver ut var 100:e värde)
		delay(delayshort);
		send_instruction(1);	//cleara displayn
		delay(delaylong);
		send_character(aaa);	//skriv ut 100-talssiffran
		delay(delaylong);
		send_character(bbb);	//skriv ut 10-talssiffran
		delay(delaylong);
		send_character(ccc);	//skriv ut 1-talssiffran
		LCDCounter = 0;			// nollställ räknare
	}

	printf("DAvalue = %d\n", DAvalue); // printa DAValue i terminalen

	if( diff < 0 ) { // Om differensen mellan är-börvärde är negativt (TURN LEFT)
		diff = diff * (-1);	// gör differens positivt
		//printf("NEG diff = %d\n", diff);			// skriv ut storleken på differensen
		setPwmDutyPercent1(1000);					// höger hjul. Skicka hög hastighet
		setPwmDutyPercent2(1500+(diff*350));		// vänster hjul. skicka lägre hastighet

	} else if ( diff > 0 ) { // Om differensen mellan är-börvärde är positiv (TURN RIGHT)
		//printf("POS diff = %d\n", diff);		// skriv ut storleken på differensen
		setPwmDutyPercent1(1500+(diff*500));	// höger hjul. skicka lägre hastighet
		setPwmDutyPercent2(1000);				// vänster hjul. Skicka hög hastighet

	} else { // Om differensen mellan är-börvärde är 0 (TURN LITTLE RIGHT)
		// Borde köra rakt, men vi kör hellre lite åt höger så vi hellre kommer mot vitt än svart
		//printf("ZERO diff = %d\n", diff); // skriv ut storleken på differensen (0)
		setPwmDutyPercent1(700);				// höger hjul. kör lite långsammare än vänster
		setPwmDutyPercent2(500);				// vänster hjul. kör lite snabbar än höger
	}

	setMode1(2);	// Höger hjul kör framåt
	setMode2(2);	// Vänster hjul kör framåt







//Återställ interruptflaggor
    TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan för MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt

	osISRExit();

	}



