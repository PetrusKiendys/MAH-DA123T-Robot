/**********************************************************************************
 *
 *
 *
 *	Funktionsprototyper för LCD-rutinerna
 *  Beskrivning finns i LCD.c
 *  Denna fil skall inkluderas i huvudfilen
 *
 ***********************************************************************************/

#include "../startup/lpc2xxx.h"

void send_instruction(long instruction);
void send_character(long character);
void wait_BF(void);
void init_LCD(void);

// *********************************
// CUSTOM CODE
// *********************************
void delay(long pulses);
void delay_mikros(long us);
void delay_millis(long ms);
void delay_secs(long secs);
