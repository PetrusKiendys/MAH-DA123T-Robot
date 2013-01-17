/**********************************************************************************
 *
 *
 *
 *	Funktionsprototyper för LCD-rutinerna
 *  Beskrivning finns i LCD.c
 *  Denna fil skall inkluderas i huvudfilen
 *
 ***********************************************************************************/

/*****************************************************************************
 * Includes
 ****************************************************************************/
#include "../startup/lpc2xxx.h"

/*****************************************************************************
 * Defines
 ****************************************************************************/
#define	data_pins	0x00FF0000		//P1.16-P1.23
#define	RS			0x01000000		//P1.24
#define	RW			0x00400000		//P0.22
#define	E			0x02000000		//P1.25
#define	backlight	0x40000000		//P0.30
#define	BF			0x00800000

/*****************************************************************************
 * Global variables
 ****************************************************************************/
//long const delayshort =	1200;
//long const delaylong  =	49250;
long const delayshort;
long const delaylong;
// REMARK: removed extern modifier!

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void send_instruction(long instruction);
void send_character(long character);
void wait_BF(void);
void init_LCD(void);

void LCD_clearDisplay(void);

void delay(long pulses);
void delay_mikros(long us);
void delay_millis(long ms);
void delay_secs(long secs);
