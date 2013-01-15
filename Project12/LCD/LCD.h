/**********************************************************************************
 *
 *
 *
 *	Funktionsprototyper för LCD-rutinerna
 *  Beskrivning finns i LCD.c
 *  Denna fil skall inkluderas i huvudfilen
 *
 ***********************************************************************************/

void delay(long pulses);
void send_instruction(long instruction);
void send_character(long character);
void wait_BF(void);
void init_LCD(void);

