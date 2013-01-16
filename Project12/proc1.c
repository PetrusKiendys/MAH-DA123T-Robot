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
#include "startup/printf_P.h"

#include "LCD/LCD.h"  //  Funktionsprototyper f�r LCD-rutinerna
#include "motor/motor.c"

/******************************************************************************
 * Defines
 *****************************************************************************/
#define P028   0x10000000			// Output Switch
/// Fr�n buttonledc
#define led 0x00000800		//the led is connected to P0.11
#define button 0x00010000	//the button is connected to P0.16

#define P014   0x00004000			// Output Switch


extern long const delayshort;
extern long const delaylong;

extern tCntSem mutexLCD;

int actual, diff; // desired
static int desired = 160;		// b�rv�rde

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
			mode1Counter = 0;
			flag = 0;

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

/*		first = DAvalue / 100;			// 100-talssiffran fr�n DAValue
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
*/
//		printf("DAvalue = %d\n", DAvalue); // printa DAValue i terminalen


		if( getState() == 5 ) {
				setMode1(2);
				setMode2(2);
				setPwmDutyPercent1(10000);		// h�ger hjul.
				setPwmDutyPercent2(10000);

			} else if( getState() == 0 || flag == 1 ) {
					//setMode1(2);	// H�ger hjul k�r fram�t
					//setMode2(2);	// V�nster hjul k�r fram�t
						if( diff < 0 ) { // Om differensen mellan �r-b�rv�rde �r negativt (TURN LEFT)
							diff = diff * (-1);	// g�r differens positivt
							//printf("NEG diff = %d\n", diff);			// skriv ut storleken p� differensen
							setMode1(2);	// H�ger hjul k�r fram�t
							setMode2(2);	// V�nster hjul k�r fram�t
							setPwmDutyPercent1(1000);					// h�ger hjul. Skicka h�g hastighet
							setPwmDutyPercent2((1500+(diff*450)));		// v�nster hjul. skicka l�gre hastighet 350

						} else if ( diff > 0 ) { // Om differensen mellan �r-b�rv�rde �r positiv (TURN RIGHT)
							//printf("POS diff = %d\n", diff);		// skriv ut storleken p� differensen
							setMode1(2);	// H�ger hjul k�r fram�t
							setMode2(2);	// V�nster hjul k�r fram�t
							setPwmDutyPercent1((1500+(diff*500)));	// h�ger hjul. skicka l�gre hastighet
							setPwmDutyPercent2(1000);				// v�nster hjul. Skicka h�g hastighet

						} else { // Om differensen mellan �r-b�rv�rde �r 0 (TURN LITTLE RIGHT)
							// Borde k�ra rakt, men vi k�r hellre lite �t h�ger s� vi hellre kommer mot vitt �n svart
							//printf("ZERO diff = %d\n", diff); // skriv ut storleken p� differensen (0)
							setMode1(2);	// H�ger hjul k�r fram�t
							setMode2(2);	// V�nster hjul k�r fram�t
							setPwmDutyPercent1(800);				// h�ger hjul. k�r lite l�ngsammare �n v�nster
							setPwmDutyPercent2(600);				// v�nster hjul. k�r lite snabbar �n h�ger
						}

						setMode1(2);	// H�ger hjul k�r fram�t
						setMode2(2);	// V�nster hjul k�r fram�t


				} else if(getState() == 1 && flag == 0) {
						//printf("mode1Counter = %d\n", mode1Counter);

						if( mode1Counter <= 6 ) {

							/*setMode1(2);
							setMode2(2);
							setPwmDutyPercent1(0);			// h�ger hjul. snabb
							setPwmDutyPercent2(235);		// v�nster hjul. snabbaste*/

							if( diff < 0 ) { // Om differensen mellan �r-b�rv�rde �r negativt (TURN LEFT)
														diff = diff * (-1);	// g�r differens positivt
														//printf("NEG diff = %d\n", diff);			// skriv ut storleken p� differensen
														setMode1(2);	// H�ger hjul k�r fram�t
														setMode2(2);	// V�nster hjul k�r fram�t
														setPwmDutyPercent1(1000);					// h�ger hjul. Skicka h�g hastighet
														setPwmDutyPercent2((1500+(diff*450)));		// v�nster hjul. skicka l�gre hastighet 350

													} else if ( diff > 0 ) { // Om differensen mellan �r-b�rv�rde �r positiv (TURN RIGHT)
														//printf("POS diff = %d\n", diff);		// skriv ut storleken p� differensen
														setMode1(2);	// H�ger hjul k�r fram�t
														setMode2(2);	// V�nster hjul k�r fram�t
														setPwmDutyPercent1((1500+(diff*500)));	// h�ger hjul. skicka l�gre hastighet
														setPwmDutyPercent2(1000);				// v�nster hjul. Skicka h�g hastighet

													} else { // Om differensen mellan �r-b�rv�rde �r 0 (TURN LITTLE RIGHT)
														// Borde k�ra rakt, men vi k�r hellre lite �t h�ger s� vi hellre kommer mot vitt �n svart
														//printf("ZERO diff = %d\n", diff); // skriv ut storleken p� differensen (0)
														setMode1(2);	// H�ger hjul k�r fram�t
														setMode2(2);	// V�nster hjul k�r fram�t
														setPwmDutyPercent1(800);				// h�ger hjul. k�r lite l�ngsammare �n v�nster
														setPwmDutyPercent2(600);				// v�nster hjul. k�r lite snabbar �n h�ger
													}

							mode1Counter++;
						}else if( mode1Counter > 6 && mode1Counter < 50 ) { // v�nster
							setMode1(2);
							setMode2(2);
							setPwmDutyPercent1(65);			// h�ger hjul. snabb
							setPwmDutyPercent2(135);		// v�nster hjul. snabbaste
							mode1Counter++;
						} else if( mode1Counter >= 50 && mode1Counter < 60 ) {
							setMode1(2);
							setMode2(2);
							setPwmDutyPercent1(5000);			// h�ger hjul. snabbaste
							setPwmDutyPercent2(0);			// v�nster hjul. snabb
		/*				} else if( mode1Counter >= 45 && mode1Counter < 45) { // h�ger
							setMode1(2);
							setMode2(2);
							setPwmDutyPercent1(5000);			// h�ger hjul. snabbaste
							setPwmDutyPercent2(0);			// v�nster hjul. snabb
							mode1Counter++;*/
//						} else if( diff < 0 ){
//							setMode1(2);	// H�ger hjul k�r fram�t
//							setMode2(2);	// V�nster hjul k�r fram�t
//							setPwmDutyPercent1(1000);		// h�ger hjul. Skicka h�g hastighet
//							setPwmDutyPercent2(3000);		// v�nster hjul. skicka l�gre hastighet 350
//							//setPwmDutyPercent1(10000);		// h�ger hjul.
//							//setPwmDutyPercent2(10000);		// v�nster hjul.
//						}
						} else if( diff < 0 ){	// k�r rakt tills linjen �terfinns
							setMode1(2);	// H�ger hjul k�r fram�t
							setMode2(2);	// V�nster hjul k�r fram�t
							setPwmDutyPercent1(0);		// h�ger hjul. Skicka h�g hastighet
							setPwmDutyPercent2(0);		// v�nster hjul. skicka l�gre hastighet 350
						} else {
							flag = 1;

		/*		} else if( getState() == 5 ) {
					setMode1(3);
					setMode2(3);
					//setPwmDutyPercent1(10000);			// h�ger hjul. snabbaste
					//setPwmDutyPercent2(10000);			// v�nster hjul. snabbaste
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
	send_instruction(2);  	//cursorn till f�rsta positionen


}
