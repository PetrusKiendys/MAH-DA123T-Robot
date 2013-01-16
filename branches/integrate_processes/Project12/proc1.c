/*
 * proc1.c
 *
 *  Created on: 31 okt 2011
 *      Author: Tommy
 */

/*****************************************************************************
 *
 *    Exempel 1
 *
 *
 ****************************************************************************/
#include "pre_emptive_os/api/osapi.h"
#include "general.h"
#include "startup/lpc2xxx.h"
#include "startup/printf_P.h"
#include "startup/ea_init.h"
#include "startup/consol.h"
#include "startup/config.h"
#include "startup/framework.h"
#include "motor/motor.c"
#include "startup/printf_P.h"

#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna

#include "interrupt/interrupt.c"

/******************************************************************************
 * Defines
 *****************************************************************************/
#define P028   0x10000000			// Output Switch
/// Från buttonledc
#define led 0x00000800		//the led is connected to P0.11
#define button 0x00010000	//the button is connected to P0.16

#define P014   0x00004000			// Output Switch


extern long const delayshort;
extern long const delaylong;

extern tCntSem mutexLCD;

int actual, diff; // desired
static int desired = 160;		// börvärde

int first, second, third;
char aaa;
char bbb;
char ccc;
static int LCDCounter = 0;
static int mode1Counter = 0;
static int flag = 0;

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void initLCD(void);

/****************************************************************************/

void
procEx1(void* arg)
{
  tU8 error;
  static int ADvalue, DAvalue;

	for (;;) {	// QUESTION: why use for-loop?

		osSemTake(&mutexLCD, 0, &error);



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
			mode1Counter = 0;
			flag = 0;

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

/*		first = DAvalue / 100;			// 100-talssiffran från DAValue
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
*/
//		printf("DAvalue = %d\n", DAvalue); // printa DAValue i terminalen


		if( getState() == 5 ) {
				setMode1(2);
				setMode2(2);
				setPwmDutyPercent1(10000);		// höger hjul.
				setPwmDutyPercent2(10000);

			} else if( getState() == 0 || flag == 1 ) {
					//setMode1(2);	// Höger hjul kör framåt
					//setMode2(2);	// Vänster hjul kör framåt
						if( diff < 0 ) { // Om differensen mellan är-börvärde är negativt (TURN LEFT)
							diff = diff * (-1);	// gör differens positivt
							//printf("NEG diff = %d\n", diff);			// skriv ut storleken på differensen
							setMode1(2);	// Höger hjul kör framåt
							setMode2(2);	// Vänster hjul kör framåt
							setPwmDutyPercent1(1000);					// höger hjul. Skicka hög hastighet
							setPwmDutyPercent2((1500+(diff*450)));		// vänster hjul. skicka lägre hastighet 350

						} else if ( diff > 0 ) { // Om differensen mellan är-börvärde är positiv (TURN RIGHT)
							//printf("POS diff = %d\n", diff);		// skriv ut storleken på differensen
							setMode1(2);	// Höger hjul kör framåt
							setMode2(2);	// Vänster hjul kör framåt
							setPwmDutyPercent1((1500+(diff*500)));	// höger hjul. skicka lägre hastighet
							setPwmDutyPercent2(1000);				// vänster hjul. Skicka hög hastighet

						} else { // Om differensen mellan är-börvärde är 0 (TURN LITTLE RIGHT)
							// Borde köra rakt, men vi kör hellre lite åt höger så vi hellre kommer mot vitt än svart
							//printf("ZERO diff = %d\n", diff); // skriv ut storleken på differensen (0)
							setMode1(2);	// Höger hjul kör framåt
							setMode2(2);	// Vänster hjul kör framåt
							setPwmDutyPercent1(800);				// höger hjul. kör lite långsammare än vänster
							setPwmDutyPercent2(600);				// vänster hjul. kör lite snabbar än höger
						}

						setMode1(2);	// Höger hjul kör framåt
						setMode2(2);	// Vänster hjul kör framåt


				} else if(getState() == 1 && flag == 0) {
						//printf("mode1Counter = %d\n", mode1Counter);

						if( mode1Counter < 60 ) { // vänster
							setMode1(2);
							setMode2(2);
							setPwmDutyPercent1(0);			// höger hjul. snabb
							setPwmDutyPercent2(100);		// vänster hjul. snabbaste
							mode1Counter++;
						} else if( mode1Counter >= 60 && mode1Counter < 130) { // höger
							setMode1(2);
							setMode2(2);
							setPwmDutyPercent1(100);			// höger hjul. snabbaste
							setPwmDutyPercent2(0);			// vänster hjul. snabb
							mode1Counter++;
						} else if( diff < 0 ){
							setMode1(2);	// Höger hjul kör framåt
							setMode2(2);	// Vänster hjul kör framåt
							setPwmDutyPercent1(1000);		// höger hjul. Skicka hög hastighet
							setPwmDutyPercent2(3000);		// vänster hjul. skicka lägre hastighet 350
							//setPwmDutyPercent1(10000);		// höger hjul.
							//setPwmDutyPercent2(10000);		// vänster hjul.
						} else {
							flag = 1;

		/*		} else if( getState() == 5 ) {
					setMode1(3);
					setMode2(3);
					//setPwmDutyPercent1(10000);			// höger hjul. snabbaste
					//setPwmDutyPercent2(10000);			// vänster hjul. snabbaste
		*/
						}
					}



//printf("\n\nBEFORE GIVE\n\n");
		osSemGive(&mutexLCD, &error);
//	printf("\n\nAFTER GIVE\n\n");
		osSleep(10);
	}

}

// TODO: rename this function
void initLCD(void) {
	delay(delayshort);
	send_instruction(1);	//cleara displayen
	delay(delaylong);
	send_instruction(2);  	//cursorn till första positionen


}
