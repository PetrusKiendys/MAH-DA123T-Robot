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
 tU16 ADvalue, DAvalue, min, max, maxx, minn;
 int left, right, actual, diff; // desired
 static tU32 timeCounter = 0;
 static int desired = 170;

 osISREnter();

 //timeCounter++;
 //printf("\ncounter: %d\n", timeCounter);
 /*static tU16 lastSend = 0;
 static int driving = 0; // is not driving if driving = 0;
 tU32 duty1;
 tU32 duty2;
 tU32 freq;

*/

 //int mode1 = 0;
 //int mode2 = 0;





 // F�R LJUSSENSOR
 //printf("TEST-Interrupt1\n");

/* AD-omvandling av AIN1 (P0.28)  */

/* EA5
 lightValue = ADDR;
   ADCR = (ADCR & 0xFFFFFF00) | (1 << 2) | (1 << 24);
   //wait till done
   while ((ADDR & 0x80000000) == 0);
   lightValue = (ADDR >> 6) & 0x3FF;
 */



 	  //get AIN1 (P0.28)or AIN2 (P0.29)
	//start conversion now (for selected channel)
 	// Denna rad har vi lagt till.
	ADCR = (ADCR & 0xFFFFFF00) | (1 << 2) | (1 << 24);

	while((ADDR & 0x80000000) == 0);  //v�nta till AD-omvandlaren �r klar
// Ovanst�ende �r egentligen lite farligt, om AD-omvandlaren l�gger av h�nger sig allt!

	//H�mta 16-bitar, skifta till 8-bit integer
	ADvalue=(ADDR>>8) & 0xFF;		//VAD G�R ADDR
//	ADvalue=(ADDR>>6) & 0x3FF;

//Test, vi g�r inget. L�gger ut samma v�rde som kom in
	DAvalue = ADvalue;
//	printf("DAvalue = %d\n", DAvalue);

/* DA-omvandling     */
	 DAvalue=DAvalue & 0xFF;	//maska ut 8 bitar f�r s�kerhets skull
	 DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode	// VAD G�R DACR

//	DAvalue=DAvalue & 0x3FF;	//maska ut 10 bitar f�r s�kerhets skull
//	DACR=DAvalue<<8;			//skifta 8 bitar, nolla i bit 16 -> snabb mode


	//Skriv ut resultat i terminal
//	printf("DAvalue =  %d \nlastSend = %d\n\n", DAvalue, lastSend);
	//setIsValue(DAvalue);

	//lastRead=DAvalue;

// SLUT LJUSSENSOR + ADOMVANDLING

	// STYR MOTORER MED OMVANDLINGEN
/*	if( DAvalue != lastSend) {
		if( DAvalue < 80 ) { // Go Forward
			setMode1(1);
			setMode2(1);
		} else if( DAvalue > 130 ) { // Go Backward
			setMode1(3);
			setMode2(3);
		} else { // Break
			setMode1(6);
			setMode2(6);
		}
		lastSend = DAvalue;
	}
*/







	// K�r Fram  om det �r ljus bakgrund. Stanna vid svart!
/*	if( DAvalue < 180){ // Go Forward(3), Go Backward(1)
		setMode1(2);
		setMode2(3);
		printf("<180 DAvalue = %d\n", DAvalue);
	}else if(DAvalue > 200) {
		setMode1(3);
		setMode2(2);
		printf(">200 DAvalue = %d\n", DAvalue);
	} else { // Break
		printf("Else: DAvalue = %d\n", DAvalue);
		setMode1(3);
		setMode2(3);
	}
*/


	//duty1 = 0;
	//duty2 = 0;

	 // Tryck p� P0.14 och starta motorer att k�ra rakt

		//set frequency value
		//setPwmDutyPercent1(duty1);
		//setPwmDutyPercent2(duty2);

		//wait 10 ms


		//delayMs(10);


	  //set frequency to 1000 Hz (1 kHz)
/**	  freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (VPBDIV_FACTOR * 1000));

	  //initialize PWM unit
	  initPwm(freq);

	  initPins();

	  //vary duty cycle
	//  duty1 = 0;
	  //duty2 = 0;

	  init_io();
*/
	 //dev_run2(driving);

//	  if(IOPIN & P014){		// ej intryckt
//		  //setMode1(3);
//		  //setMode2(3);
//	  } else {
//		  setMode1(2); // k�r fram
//		  setMode2(2); // k�r fram
//
//		  driving++;
//		  if( driving == 10 )
//			  driving = 0;
//
//		 dev_run2(driving);
//	  	 //setTask(driving);
//	  }


	// FUNGERAR
	/*	if(IOPIN & P014){		// ej intryckt
			//setMode1(3);
			//setMode2(3);
		} else {
			if( driving == 0 ) {

				printf("driving = 0. BUTTON CLICKED\n");
				//setMode1(1);
				//setMode2(1);
			   //dev_run2(8);

				setModeVal1(2);
				setModeVal2(2);

				setTask(8);

				driving = 1;

			} else {


				printf("driving = 1. BUTTON CLICKED\n");

				//setMode1(2);
				//setMode2(2);
				//dev_run2(9);

				setModeVal1(1);
				setModeVal2(1);
				setTask(9);

				driving = 0;

			}
		}
*/
/*
	if(IOPIN & P014){		// ej intryckt
		//setMode1(3);
		//setMode2(3);
	} else {
		if( driving == 0 ) {

			setMode1(1);
			setMode2(1);
			duty1 = 10000;
			duty2 = 10000;
			setPwmDutyPercent1(duty1);
			setPwmDutyPercent2(duty2);

			driving = 1;
		} else {


			setMode1(2);
			setMode2(2);

			duty1 = 10000;
			duty2 = 0;

			setPwmDutyPercent1(duty1);
			setPwmDutyPercent2(duty2);

			driving = 0;
		}
	}
*/

	//setModeVal2(2);
	//setModeVal1(2);

//	min = 180;
//	max = 190;

//	minn = 170;
//	maxx = 200;
	//DAvalue = DAvalue*100;

//	desired = 170;	// b�r-v�rde

	if(IOPIN & P014){		// ej intryckt
			//setMode1(3);
			//setMode2(3);
	} else {
		setPwmDutyPercent1(1000);
		setPwmDutyPercent1(1000);
		desired = DAvalue;
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

	printf("desired = %d\n", desired);

	actual = DAvalue;
	diff = actual - desired; // pos: svart, neg: vit


	printf("DAvalue = %d\n", DAvalue);

	if( diff < 0 ) { // TURN LEFT
		diff = diff * (-1);
		printf("NEG diff = %d\n", diff);
		setPwmDutyPercent1(500);	// h�ger hjul
		setPwmDutyPercent2(1000+(diff*100));		// v�nster hjul
	} else if ( diff > 0 ) {
		printf("POS diff = %d\n", diff);
		setPwmDutyPercent1(1000+(diff*100));	// h�ger hjul
		setPwmDutyPercent2(500);		// v�nster hjul
	} else {
		printf("ZERO diff = %d\n", diff);
		setPwmDutyPercent1(500);	// h�ger hjul
		setPwmDutyPercent2(500);		// v�nster hjul
	}

	setMode1(2);
	setMode2(2);



//	left = 0;
//	right = 0;
//	setMode1(1);
//	setMode2(2);
//	setPwmDutyPercent1(right);	// RIGHT
//	setPwmDutyPercent2(left);	// LEFT

/*
	if( DAvalue > max && DAvalue < maxx){ // Go Forward(3), Go Backward(1)
		//setMode1(2);
		//setMode2(3);
	//	printf(">180 DAvalue = %d\n", DAvalue);
		//setTask(9); // sv�ng v�nster
		setMode1(1);
		setMode2(2);
		left = 1000;
		right = 1000;
		setPwmDutyPercent1(right);	// RIGHT
		setPwmDutyPercent2(left);	// LEFT
		//setPwmDuty1(30000);
		//setPwmDuty2(6000);
	} else if(DAvalue < min && DAvalue > minn) {
		//setMode1(3);
		//setMode2(2);
	//	printf("<200 DAvalue = %d\n", DAvalue);
		//setTask(8); // sv�ng h�ger
		setMode1(2);
		setMode2(2);
		left = 4000;
		right = 1000;
		setPwmDutyPercent1(right);	// RIGHT
		setPwmDutyPercent2(left);	// LEFT
		//setPwmDuty1(6000);
		//setPwmDuty2(30000);

	} else if(DAvalue < minn) {
		setMode1(2);
		setMode2(2);
		left = 6500;
		right = 1000;
		setPwmDutyPercent1(right);	// RIGHT
		setPwmDutyPercent2(left);	// LEFT
	} else if(DAvalue > maxx) {
		setMode1(1);
		setMode2(2);
		left = 2000;
		right = 2000;
		setPwmDutyPercent1(right);	// RIGHT
		setPwmDutyPercent2(left);	// LEFT
	} else { // Break
	//	printf("Else: DAvalue = %d\n", DAvalue);
		//setMode1(3);
		//setMode2(3);
		//setTask(1);
		setMode1(2);
		setMode2(2);
		left = 100;
		right = 1000;
		setPwmDutyPercent1(right);
		setPwmDutyPercent2(left);
		//setPwmDuty1(6000);
		//setPwmDuty2(6000);
	}
*/
	//runPwm();
	// SKRIV UT V�RDE:
	//printf("DAvalue = %d\n", DAvalue);






//�terst�ll interruptflaggor
    TIMER1_IR |= 0x00000001;          //reseta interrrupt flaggan f�r MR0
	VICVectAddr = 0x00;        			//dummy write to VIC to signal end of interrupt

	osISRExit();

	}



