/**********************************************************************************************
 *
 *	LCD.c
 *	111031 TA
 *
 * 	Jonas LCD-rutiner med vissa justeringar
 *
 *
 *  Makefilen i mappen LCD generar lib_LDC.a i ARM-format, beh�vs pga asm instruktionerna i delay
 *
 ************************************************************************************************/
#include "../startup/lpc2xxx.h"

/* Med CCLK=60 MHz ger denna rutin en delay p� pulses*66,666 ns + anropstiden */

void delay(long pulses)
{
	asm("wait:   SUBS R0, R0, #1");
	asm("BNE wait");
}


#define	data_pins	0x00FF0000		//P1.16-P1.23
#define	RS	0x01000000		//P1.24
#define	RW	0x00400000		//P0.22
#define	E	0x02000000		//P1.25
#define	backlight	0x40000000		//P0.30
#define	BF	0x00800000

void send_instruction(long instruction)
{
	delay(100);
	IOCLR1 = RS;		//send instruction
	delay(100);
	IOSET1 = E;			//start a write pulse
	delay(100);
	IOCLR1 = data_pins;		//set p1.16-P1.23 to 0
	delay(100);
	IOSET1 = instruction<<16;
	delay(100);
	IOCLR1 = E;			//end write pulse
}

void send_character(long character)
{
	IOSET1 = RS;		//send character
	IOSET1 = E;			//start a write pulse
	IOCLR1 = data_pins;		//set p1.16-P1.23 to 0
	IOSET1 = character<<16;
	IOCLR1 = E;			//end write pulse
}

void wait_BF(void)
{
//	IODIR1 |= BF;  alternativ lsg p� kapacitansproblemet nedan
//	IOSET1 = BF;

	IOSET0 = RW;		//read from display
	IODIR1 &= ~data_pins;		//make data_pins input
	IOCLR1 = RS;		//read status
	IOSET1 = E;
	delay(2);			//beh�vs pga kapacitanser i flatkabel
	while(IOPIN1 & BF);		//wait until BF == 0
	IOCLR1 = E;
	IODIR1 |= data_pins;		//make data_pins output
//	delay(1);  //beh�vs inte om optimering O0, beh�vs om O2
	IOCLR0 = RW;		//restore write to display
}


void init_LCD(void)
{
	IODIR0 |= RW|backlight;
	IODIR1 |= data_pins|E|RS;
	IOCLR1 = E;
	IOCLR0 = RW;		//write to the display
	IOSET0 = backlight;		//turn on the backlight
	send_instruction(0x38);		//function set
	delay(1200);			//12*60/48 * 80 (80�s)
	send_instruction(0x38);
	delay(1200);
	send_instruction(0x0F);		//display on/off
	delay(1200);
	send_instruction(0x01);		//display clear
	delay(49250);		//12* 60/48 * 3283 (3,283 ms)
	send_instruction(0x06);		//entry mode set
	delay(1200);
}


