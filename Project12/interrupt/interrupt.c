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

#include "../LCD/LCD.h"  //  Funktionsprototyper f�r LCD-rutinerna

//#include "../motor/motor.c"


/******************************************************************************
 * Defines
 *****************************************************************************/
#define P028   0x10000000			// Output Switch
/// Fr�n buttonledc
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
 * Exempel p� Avbrottsrutin (ljudsamling)
 *
 ****************************************************************************/





void Timer1ISRirq (void)
{
	int ADvalue, DAvalue;
 int actual, diff; // desired
 static int desired = 160;		// b�rv�rde

 int first, second, third;
 char aaa;
 char bbb;
 char ccc;
 static int LCDCounter = 0;


 osISREnter();


 // F�R LJUSSENSOR
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

	while((ADDR & 0x80000000) == 0);  //v�nta till AD-omvandlaren �r klar
// Ovanst�ende �r egentligen lite farligt, om AD-omvandlaren l�gger av h�nger sig allt!

	//H�mta 16-bitar, skifta till 8-bit integer
	ADvalue=(ADDR>>8) & 0xFF;

//Test, vi g�r inget. L�gger ut samma v�rde som kom in
	DAvalue = ADvalue;

/* DA-omvandling     */
	 DAvalue=DAvalue & 0xFF;	//maska ut 8 bitar f�r s�kerhets skull
	 DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode

// SLUT LJUSSENSOR + ADOMVANDLING



	if(IOPIN & P014){		// ej intryckt

	} else {				// P014 intryckt
		setPwmDutyPercent1(10000);	// s�tter hastighet till 0
		setPwmDutyPercent1(10000);	// s�tter hastighet till 0
		desired = DAvalue;			// desired (b�rv�rde) blir avl�sningen

		// Printar meddelande p� displayen s� vi vet att initiering gjorts
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



	actual = DAvalue;		 // actual (�rv�rde) blir det ljussensorn l�ser av
	diff = actual - desired; // differens mellan �r- och b�rv�rde (pos: svart, neg: vit)

	first = DAvalue / 100;			// 100-talssiffran fr�n DAValue
	second = (DAvalue % 100) / 10; 	// 10-talssiffran fr�n DAValue
	third = DAvalue % 10;			// 1-talssiffran fr�n DAValue

	LCDCounter++; 					// �kar bara en r�knare med 1

	aaa = '0' + first;				// g�r 100-talssifran till en char
	bbb = '0' + second;				// g�r 10-talssifran till en char
	ccc = '0' + third;				// g�r 1-talssifran till en char

	if( (LCDCounter == 100) ) {	// om r�knaren g�tt till 100 (skriver ut var 100:e v�rde)
		delay(delayshort);
		send_instruction(1);	//cleara displayn
		delay(delaylong);
		send_character(aaa);	//skriv ut 100-talssiffran
		delay(delaylong);
		send_character(bbb);	//skriv ut 10-talssiffran
		delay(delaylong);
		send_character(ccc);	//skriv ut 1-talssiffran
		LCDCounter = 0;			// nollst�ll r�knare
	}

	printf("DAvalue = %d\n", DAvalue); // printa DAValue i terminalen

	if( diff < 0 ) { // Om differensen mellan �r-b�rv�rde �r negativt (TURN LEFT)
		diff = diff * (-1);	// g�r differens positivt
		//printf("NEG diff = %d\n", diff);			// skriv ut storleken p� differensen
		setPwmDutyPercent1(1000);					// h�ger hjul. Skicka h�g hastighet
		setPwmDutyPercent2(1500+(diff*350));		// v�nster hjul. skicka l�gre hastighet

	} else if ( diff > 0 ) { // Om differensen mellan �r-b�rv�rde �r positiv (TURN RIGHT)
		//printf("POS diff = %d\n", diff);		// skriv ut storleken p� differensen
		setPwmDutyPercent1(1500+(diff*500));	// h�ger hjul. skicka l�gre hastighet
		setPwmDutyPercent2(1000);				// v�nster hjul. Skicka h�g hastighet

	} else { // Om differensen mellan �r-b�rv�rde �r 0 (TURN LITTLE RIGHT)
		// Borde k�ra rakt, men vi k�r hellre lite �t h�ger s� vi hellre kommer mot vitt �n svart
		//printf("ZERO diff = %d\n", diff); // skriv ut storleken p� differensen (0)
		setPwmDutyPercent1(700);				// h�ger hjul. k�r lite l�ngsammare �n v�nster
		setPwmDutyPercent2(500);				// v�nster hjul. k�r lite snabbar �n h�ger
	}

	setMode1(2);	// H�ger hjul k�r fram�t
	setMode2(2);	// V�nster hjul k�r fram�t







//�terst�ll interruptflaggor
    TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan f�r MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt

	osISRExit();

	}



