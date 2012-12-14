
//#include "../pre_emptive_os/api/osapi.h"
#include "../general.h"
#include "../startup/lpc2xxx.h"
#include "../startup/printf_P.h"
#include "../startup/ea_init.h"
#include "../startup/consol.h"
#include "../startup/config.h"
#include "../startup/framework.h"

#include "../LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna


#define P06 0x00000040	// Output Ljussensor
#define P029 0x20000000 // Input Ljussensor
#define P028 0x10000000 // Output Switch

//void initLightSwitchPins(void);

void initLightSwitchPins()
{

  //FÖR LJUSSENSORN:
  IODIR |= P06; // Sätter P0.6 till output
  IODIR &= ~P029; // Sätter P029 till input
  IOSET = P06; //Sätter p06 hög

 //För LJUSSENSOR+Digital Switch
  PINSEL1 |= 0x05080000;	 //set AIN1 = P0.28, set AIN2 = P0.29,set Aout (DAC) = P0.25

  // FÖR DIGITALSWITCH:
  IODIR &= ~P028; // sätter P0.28 till input

}

