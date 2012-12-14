/******************************************************************************
 *
 * Copyright:
 *    (C) 2005 Embedded Artists AB
 *
 * File:
 *    main.c
 *
 * Description:
 *    Sample application that demonstrates how to generate a PWM signal
 *    with a fixed frequency and varying duty cycle.
 *    This feature can for example be used to generate an analogue signal
 *    if the PWM signal is low pass filtered.
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "general.h"
#include <lpc2xxx.h>
#include "startup/config.h"

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
static void initPwm(tU32 initialFreqValue);
static void setPwmDutyPercent(tU32 dutyValue);
static void setPwmDuty(tU32 dutyValue);
static void delayMs(tU16 delayInMs);

/*****************************************************************************
 * Implementation of local functions
 ****************************************************************************/

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
static void
initPwm(tU32 initialFreqValue)
{
  /*
   * initialize PWM
   */
  PWM_PR  = 0x00000000;             //set prescale to 0
  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
  PWM_MR2 = 0;                      //MR2 = duty cycle control, initial = 0%
  PWM_LER = 0x05;                   //latch new values for MR0 and MR2
  PWM_PCR = 0x0400;                 //enable PWM2 in single edge control mode
  PWM_TCR = 0x09;                   //enable PWM and Counter

  /*
   * connect signal PWM2 to pin P0.7
   */
  PINSEL0 &= ~0x0000c000;  //clear bits related to P0.7
  PINSEL0 |=  0x00008000;  //connect signal PWM2 to P0.7 (second alternative function)
}

/*****************************************************************************
 *
 * Description:
 *    Update the duty cycle value of the PWM signal.
 *
 * Params:
 *    [in] dutyValue - the new duty cycle value in percent in increments
 *                     of 0.01% (i.e., 10% = 1000).
 *
 ****************************************************************************/
static void
setPwmDutyPercent(tU32 dutyValue)
{
  PWM_MR2 = (PWM_MR0 * dutyValue) / 10000;  //update duty cycle
  PWM_LER = 0x04;                           //latch new values for MR2
}

/*****************************************************************************
 *
 * Description:
 *    Update the duty cycle value of the PWM signal.
 *
 * Params:
 *    [in] dutyValue - the new duty cycle value. Value calculated as:
 *    (crystal frequency * PLL multiplication factor)
 *    ----------------------------------------------- * duty cycle in percent
 *           (VPBDIV factor * PWM frequency)
 *
 *                    or
 *
 *    value in MR0 register * duty cycle in percent
 *
 ****************************************************************************/
static void
setPwmDuty(tU32 dutyValue)
{
  PWM_MR2 = dutyValue;    //update duty cycle
  PWM_LER = 0x04;         //latch new values for MR2
}
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
  tU32 duty;
  tU32 freq;

  //set frequency to 1000 Hz (1 kHz)
  freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (VPBDIV_FACTOR * 1000));

  //initialize PWM unit
  initPwm(freq);
  
  //vary duty cycle
  duty = 0;

  while(1)
  {
    //set frequency value
    setPwmDutyPercent(duty);
    
    //wait 10 ms
    delayMs(10);
    
    //update duty cycle (0.00 - 100.00%, in steps of 0.10%)
    duty += 10;
    if (duty > 10000)
      duty = 0;
  }

  return 0;
}
