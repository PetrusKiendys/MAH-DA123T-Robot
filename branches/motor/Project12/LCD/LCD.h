/*************************************************************
 *  Filename: LCD.h
 *  Created on: ?
 *  Author: ?
 *  Description:	Funktionsprototyper för LCD-rutinerna
 *  				Beskrivning finns i LCD.c
 *  				Denna fil skall inkluderas i huvudfilen
 *
 *  Modified by: Petrus K. & Ardiana O. (2013-01-10)
 *  Description: Header file for LCD.c.
 *************************************************************/

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
long const delayshort;
long const delaylong;
// REMARK: removed extern modifier!


/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void LCD_sendInstruction(long instruction);
void LCD_sendCharacter(long character);
void LCD_waitBF(void);
void LCD_initLCD(void);

void LCD_clearDisplay(void);

// REMARK: the functions below should be moved to another source file
// EDIT:   tried moving to utils/utils.c and compiling but didn't work...
void delay(long pulses);
void delay_mikros(long us);
void delay_millis(long ms);
void delay_secs(long secs);
