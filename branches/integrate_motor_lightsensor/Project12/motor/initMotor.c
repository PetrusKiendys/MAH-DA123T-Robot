
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
//#include <lpc2xxx.h>
#include "../startup/lpc2xxx.h"
#include "../startup/config.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define P07 (1 << 7)
#define P12	(1 << 12)
#define P16 (1 << 16)
#define P20 (1 << 20)
#define P21 (1 << 21)
#define	P23 (1 << 23)
#define	P25	(1 << 25)
#define	P31 (1 << 31)

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
//void runPwm();
//void initPwm(tU32 initialFreqValue);
//void initPins();
//void init_io();



void runPwm()
{

  tU32 freq;

  //set frequency to 1000 Hz (1 kHz)
  freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (VPBDIV_FACTOR * 1000));

  //initialize PWM unit
  initPwm(freq);

  initPins();

  init_io();

}

/*****************************************************************************
 *
 * Description:
 *    Initialize the PWM unit to generate a variable duty cycle signal on
 *    PWM2. Connect signal PWM2 to pin P0.7.
 *    The function sets initial frequency. Initial duty cucle is set to 0%.
 *
 * Params:
 *    [in] initialFreqValue - the initial frequency value. Value calculated as:
 *
 *                     (crystal frequency * PLL multiplication factor)
 *                     -----------------------------------------------
 *                           (VPBDIV factor * desired frequency)
 *
 ****************************************************************************/
void initPwm(tU32 initialFreqValue)
{
  /*
   * initialize PWM
   */
  PWM_PR  = 0x00000000;             //set prescale to 0
  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
  PWM_MR2 = 0;      				//MR2 = duty cycle control, initial = 0%
  PWM_MR5 = 0;  					//MR5 = duty cycle control, initial = 0%
  PWM_LER = 0x25;                   //latch new values for MR0 and MR2 and MR5
  PWM_PCR = 0x2400;                 //enable PWM2 and in single edge control mode
  PWM_TCR = 0x09;                   //enable PWM and Counter


}

/*****************************************************************************
 *
 * Description:
 *    The main-function.
 *
 * Returns:
 *    Always 0, since return value is not used.
 *
 ****************************************************************************/
void initPins() {

	PINSEL0 &= ~0x0000c000;
	PINSEL0 |= 0x00008000;

	// connect GPIO to P0.12 - Motor 1 PHASE
	PINSEL0 &= ~0x03000000;


	// connect PWM5 to P0.21 - Motor 2 ENABLE (PWM5)
	PINSEL1 &= ~0x00000c00;
	PINSEL1 |= 0x00000400;

	// connect GPIO to P0.23 - Motor 2 PHASE
	PINSEL1 &= ~0x0000c000;

	// connect GPIO to P0.25 - Motor 1 BRAKE
	PINSEL1 &= ~0x000c0000;

	// connect GPIO to P0.31 - Motor 2 BRAKE
	PINSEL1 &= ~0xc0000000;

}

void init_io() {
	// OUTPUT INITIALIZATION
	// initializes UT1
	IODIR |= P07;	// ENABLE (P0.7)
	IODIR |= P12;	// PHASE (P0.12)
	IODIR |= P25;	// BRAKE (P0.25)

	// initializes UT2
	IODIR |= P21;	// ENABLE (P0.21)
	IODIR |= P23;	// PHASE (P0.23)
	IODIR |= P31;	// BRAKE (P0.31)


	// INPUT INITIALIZATION
	//IODIR &= ~(P16);
	//IODIR &= ~(P20);	// makes EINT0 and EINT3 inputs
}

