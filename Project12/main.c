/*************************************************************
 *  Filename: main.c
 *  Author: Tommy
 *  Created on: 2012-11-08
 *  Description:	Bas f�r Project12
 *
 *  				RTOS kompilerat f�r max 10 processer, 5 prioritetsniv�er
 *  				link_32k_512k_rom.ld �ndrad s� att ramadressen b�rjar p� 0x40000200.
 *  				Newlib anv�nds inte.
 *
 *  				Alla �ndringar n�r det g�ller processortyp mm �r gjorda.
 *  				�ven f�rberett f�r on chip debugging.
 *
 *  				Projektet visar hur man kan ha processerna i olika filer, vilket �r bra
 *  				n�r man �r m�nga som jobbar med samma projekt.
 *  				Namnen p� k�llkodsfilerna m�ste anges i den yttre makefilen.
 *
 *  				Pga av problem med BF_wait f�r vissa kort anv�nds i st�llet delay() i exemplen.
 *
 *  				I projektet visas �ven hur man hanterar avbrott. Exemplet samplar en analog signal
 *  				8000 g�nger/sekund och l�gger sen ut v�rdena p� DA-omvandlaren.
 *
 *  Modified by: Petrus K. & Ardiana O. (2012-12-03)
 *	Description:	Main file for the application from which everything else is invoked.
 *					Serves as the base for the application.
 *************************************************************/
// TODO: integrate main.h and other help files from /branches/time_sampling_test into this project

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "pre_emptive_os/api/osapi.h"
#include "general.h"
#include "startup/lpc2xxx.h"
#include "startup/printf_P.h"
#include "startup/ea_init.h"
#include "startup/consol.h"
#include "startup/config.h"
#include "startup/framework.h"
#include "LCD/LCD.h"

#include "utils/utils.h"


/******************************************************************************
 * Defines
 *****************************************************************************/
// Dessa definitioner anv�nds av en del av EA's rutiner
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD


// Stackstorlekar f�r processerna. Kan beh�va justeras. Se beskrivning av procStackUsage nedan
#define PROC_Ex1_STACK_SIZE 1024
#define PROC_Ex2_STACK_SIZE 1024
#define PROC_Ex3_STACK_SIZE 1024

#define INIT_STACK_SIZE  1024
#define PROC_ST_US_STACK_SIZE  1024

/*****************************************************************************
 * Global variables
 ****************************************************************************/
static tU8 procEx1Stack[PROC_Ex1_STACK_SIZE];
static tU8 procEx2Stack[PROC_Ex2_STACK_SIZE];
static tU8 procEx3Stack[PROC_Ex3_STACK_SIZE];

static tU8 initStack[INIT_STACK_SIZE];
static tU8 procStUsStack[PROC_ST_US_STACK_SIZE];

static tU8 pidEx1;
static tU8 pidEx2;
static tU8 pidEx3;

static tU8 pidStUs;

long const delayshort =	1200;
long const delaylong  =	49250;

tCntSem mutexLCD;	//	Semaforer f�r att skydda gemensamma resurser,
					//	anv�nds h�r som bin�ra semaforer dvs antar bara v�rden 0 och 1 (mutex)

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void procEx1(void* arg);
void procEx2(void* arg);
void procEx3(void* arg);
static void initProc(void* arg);
static void procStackUsage(void* arg);

//Exempel p� avbrott (ljudsampling)
void interruptTimer_Timer1ISR (void);  // skall inte ha n�got attribut n�r RTOS anv�nds
/****************************************************************************/


/*****************************************************************************
 * Main function
 ****************************************************************************/
int
main(void)
{
  tU8 error;
  tU8 pid;

  osInit();

  LCD_initLCD();
  delay(delaylong);
  LCD_sendInstruction(0xC);  //sl�ck cursorn

  // TODO: do inits here, make sure you enable interrupts last

  osCreateProcess(initProc, initStack, INIT_STACK_SIZE, &pid, 1, NULL, &error);
  osStartProcess(pid, &error);

  osStart();
  return 0;
}


/*****************************************************************************
 * Functions
 ****************************************************************************/

/*****************************************************************************
 *
 *  Denna process kan anv�ndas f�r att kolla hur mycket stort utrymme
 *  som stacken anv�nder
 *  Ta bort kommateringen av printf-raden och s�tt in r�tt pid f�r processen som
 *  skall kollas
 *
 ****************************************************************************/
static void
procStackUsage(void* arg)
{
  for(;;)
  {
    osSleep(5500);
    printf("Stack usage proc1: %d %%\n", osStackUsage(pidEx1));
    printf("Stack usage proc2: %d %%\n", osStackUsage(pidEx2));
    printf("Stack usage proc3: %d %%\n", osStackUsage(pidEx3));
  }
}
/*****************************************************************************
 *
 * Initieringprocessen som startar upp de andra processerna
 * H�r �r l�mpligt att tex initiera avbrott. Initeringar kan alternativt l�ggas i main()
 * Men enabla inte avbrotten f�rr�n p� slutet av denna process
 * n�r alla andra processerna startat
 *
 * Som exempel visas avbrottet f�r att sampla ljud
 *
 ****************************************************************************/
static void
initProc(void* arg)
{
  tU8 error;

  eaInit();

  printf("RTOS startar \n");

 // Initiering av semaforerna. Alla ges v�rdet 1 fr�n b�rjan
  osSemInit(&mutexLCD, 1);


  osCreateProcess(procEx1, procEx1Stack, PROC_Ex1_STACK_SIZE, &pidEx1, 3, NULL, &error);
  osStartProcess(pidEx1, &error);

  osCreateProcess(procEx2, procEx2Stack, PROC_Ex2_STACK_SIZE, &pidEx2, 3, NULL, &error);
  osStartProcess(pidEx2, &error);

  osCreateProcess(procEx3, procEx3Stack, PROC_Ex3_STACK_SIZE, &pidEx3, 3, NULL, &error);
  osStartProcess(pidEx3, &error);


  osCreateProcess(procStackUsage, procStUsStack, PROC_ST_US_STACK_SIZE, &pidStUs, 4, NULL, &error);
  osStartProcess(pidStUs, &error);




 /****************************************************************************************
  * Detta �r ett exempel p� avbrottshantering
  * Kod f�r initiering av AD och DA-omvandlare och Timer 1 som skall ge avbrott (8 KHz)
  ****************************************************************************************/

  //set AIN1 = P0.28
  PINSEL1 |= 0x01080000;

 //initialize ADC (fel i ber�kningen av divisionsfaktorn i EA-kod, r�ttat h�r)
   ADCR = (1 << 0)                             |	//SEL = 1, dummy channel #1
          ((CRYSTAL_FREQUENCY *
            PLL_FACTOR /
            VPBDIV_FACTOR) / 4500000) << 8 |  		//set clock division factor, so ADC clock is 4.5MHz
          (0 << 16)                            |	//BURST = 0, conversions are SW controlled
          (0 << 17)                            |	//CLKS  = 0, 11 clocks = 10-bit result
          (1 << 21)                            |	//PDN   = 1, ADC is active
          (1 << 24)                            |	//START = 1, start a conversion now
          (0 << 27);								//EDGE  = 0, not relevant when start=1

   while((ADDR & 0x80000000) == 0)



	// REMARK: this is where you set the sampling frequency of lightsensor input reading and processing?
	// Initiera timer 1 (som skall sk�ta avbrottet)
   TIMER1_TCR = 0x02;          //stop and reset timer
   TIMER1_PR = 0x00;          //set prescaler to zero
   TIMER1_MCR = 0x03; //st�ll up matchregister 0 f�r avbrott och reset av timer 0
   TIMER1_MR0 = 7500;	//Detta v�rde avg�r sampletiden, h�r 0.125 ms  (8000 Hz)
   TIMER1_IR = 0xff;          //reset all interrupt flags
   TIMER1_TCR = 0x01;          //start timer


  // Initiera avbrott vid match p� MR0
  // Numret p� VICVectCntl och VICVectAddr best�mmer prioriteten
  // TEMPLATE:	VICVectCntl["slot"] = 0x20 | ["VIC_channel"]
  //			where "slot" is the priority slot of the VIC Control registers (from 0 to 15, 0 has highest priority)
  //			and "VIC_channel" is the VIC channel for the specified flag/function (as specified by Table 5-57 in manualLPC2148.pdf)
  VICIntSelect	&=	~0x000000020;		// Timer1 s�tts till IRQ
  VICVectCntl7  =  	 0x00000025;		// tilldela interuptchannel 5 (Timer1) till slot 7
  VICVectAddr7	=  	(tU32)interruptTimer_Timer1ISR;	// adressen till avbrottsrutinen p� slot 7
  VICIntEnable	|=	 0x000000020;		// enabla Timer1 som avbrott

//*************slut ljudavbrottsinitering****************

  osDeleteProcess();
}

/*****************************************************************************
 *
 * Description:
 *    The timer tick entry function that is called once every timer tick
 *    interrupt in the RTOS. Observe that any processing in this
 *    function must be kept as short as possible since this function
 *    execute in interrupt context.
 *
 * Params:
 *    [in] elapsedTime - The number of elapsed milliseconds since last call.
 *
 ****************************************************************************/
// REMARK: 2.5 ms per osTick is hardcoded/compiled for this project (the OS reference of time is generated by TIMER0 of the LPC2148)
void
appTick(tU32 elapsedTime)
{
}
