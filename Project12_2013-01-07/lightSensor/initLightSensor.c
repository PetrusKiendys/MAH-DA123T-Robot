
//#include "../pre_emptive_os/api/osapi.h"
#include "../general.h"
#include "../startup/lpc2xxx.h"
#include "../startup/printf_P.h"
#include "../startup/ea_init.h"
#include "../startup/consol.h"
#include "../startup/config.h"
#include "../startup/framework.h"

#include "../LCD/LCD.h"  //  Funktionsprototyper f�r LCD-rutinerna


#define P06 0x00000040	// Output Ljussensor
#define P029 0x20000000 // Input Ljussensor
#define P028 0x10000000 // Output Switch

//void initLightSwitchPins(void);

void initLightPins()
{

	 //F�R LJUSSENSORN:
	  IODIR |= P06; // S�tter P0.6 till output
	  IODIR &= ~P029; // S�tter P029 till input
	   IOSET = P06; //S�tter p06 h�g

	  //F�r LJUSSENSOR+Digital Switch
	  // PINSEL1 |= 0x05080000;	 //set AIN1 = P0.28, set AIN2 = P0.29,set Aout (DAC) = P0.25
	  // PINSEL1 |= 0x01080000;	 //set AIN1 = P0.28, set AIN2 = P0.29,set Aout (DAC) = P0.25
	 //  PINSEL1 |=(1 << 24); // DONT SET THIS ONE, FUCKES UP THE SWITCH
	 //  PINSEL1 |=(1 << 26);
	  // PINSEL1 |=(1 << 19);
	   PINSEL1 |=(1 << 19) | (1 << 26); ////set AIN2 = P0.29,set Aout (DAC) = P0.25.
	   //PINSEL1 |= 0x04080000; //set AIN2 = P0.29,set Aout (DAC) = P0.25. This works so switch dosen't affects
	    //F�R DIGITALSWITCH:
	 //  IODIR &= ~P028; // s�tter P0.28 till input

}

