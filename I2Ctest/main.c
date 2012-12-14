/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to use the i2c library.
 *    Omarbetad för att testa hur man läser temp.sensorn /Tommy
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <ea_init.h>
#include <framework.h>
#include "startup/config.h"
#include "i2c.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD



/*****************************************************************************
 * Public function prototypes
 ****************************************************************************/
int main(void);


/*****************************************************************************
 * Local function prototypes
 ****************************************************************************/
static void delayMs(tU16 delayInMs);


/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    Delay execution by a specified number of milliseconds by using
 *    timer #1. A polled implementation.
 *
 * Params:
 *    [in] delayInMs - the number of milliseconds to delay.
 *
 ****************************************************************************/
static void
delayMs(tU16 delayInMs)
{
  /*
   * setup timer #1 for delay
   */
  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MR0 = delayInMs * ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (1000 * VPBDIV_FACTOR));
  TIMER1_IR  = 0xff;          //reset all interrrupt flags
  TIMER1_MCR = 0x04;          //stop timer on match
  TIMER1_TCR = 0x01;          //start timer

  //wait until delay time has elapsed
  while (TIMER1_TCR & 0x01)
    ;
}


/*****************************************************************************
 * Implementation of public functions
 ****************************************************************************/

/*****************************************************************************
 *
 * Description:
 *    The main-function.
 *
 * Returns:
 *    Always 0, since return value is not used.
 *
 ****************************************************************************/
int
main(void)
{


  //initialize printf()-functionality
  eaInit();

  //print welcome message
  printf("Enkel avläsning av temperatursensorn/Tommy");

  //initialice i2c communication
  i2cInit(10000);

  //enter forever loop...
  while(1)
  {

    //Läs temperatur
    //Enklast tänkbara, vi antar att pointern står defaultläge, vi struntar i retCode och vi printar bara de 8 översta bitarna
    //resultatet blir då i grader C

    tU8 temperatur[2];		//två bytes hämtas från LM75

    temperatur[0]=0;		//8 mest signifikanta bitarna, lägsta biten svarar mot 1 grad C
    temperatur[1]=0;			//högsta biten svarar mot 0,5 grad C, övriga bitar utan info;



    i2cStart();
    i2cRead(0x91,temperatur,2);  //de 7 mest signifikanta bitarna (bit 7-1) är adressen. Bit 0 = 1 betyder "Read data"
    i2cStop();

    printf("\nTemperaturen: %d grader C",temperatur[0]);




	//wait 1000 ms
    delayMs(1000);
  }

  return 0;
}
