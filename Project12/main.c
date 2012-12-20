/******************************************************************************
 *
 * Bas för Project12
 *
 * RTOS kompilerat för max 10 processer, 5 prioritetsnivåer
 * link_32k_512k_rom.ld ändrad så att ramadressen börjar på 0x40000200
 * Newlib används inte
 *
 * Alla ändringar när det gäller processortyp mm är gjorda.
 * Även förberett för on chip debugging.
 *
 * Projektet visar hur man kan ha processerna i olika filer, vilket är bra
 * när man är många som jobbar med samma projekt.
 * Namnen på källkodsfilerna måste anges i den yttre makefilen
 *
 * Pga av problem med BF_wait för vissa kort används i stället delay()i exemplen.
 *
 * I projektet visas även hur man hanterar avbrott. Exemplet samplar en analog signal
 * 8000 gånger/sekund och lägger sen ut värdena på DA-omvandlaren
 *
 * Tommy 121108
 *
 *****************************************************************************/

// QUESTION: how many ms is a tick (RTOS)?

#include "pre_emptive_os/api/osapi.h"
#include "general.h"
#include "startup/lpc2xxx.h"
#include "startup/printf_P.h"
#include "startup/ea_init.h"
#include "startup/consol.h"
#include "startup/config.h"
#include "startup/framework.h"
#include "lightSensor/initLightSensor.c"

#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna

// Dessa definitioner används av en del av EA's rutiner
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD


// Stackstorlekar för processerna. Kan behöva justeras. Se beskrivning av procStackUsage nedan
#define PROC_Ex1_STACK_SIZE 1024
#define PROC_Ex2_STACK_SIZE 1024
#define PROC_Ex3_STACK_SIZE 1024

#define INIT_STACK_SIZE  1024
#define PROC_ST_US_STACK_SIZE  1024

#define P06 0x00000040	// Output Ljussensor
#define P029 0x20000000 // Input Ljussensor
#define P028 0x10000000 // Output Switch

static tU8 procEx1Stack[PROC_Ex1_STACK_SIZE];
static tU8 procEx2Stack[PROC_Ex2_STACK_SIZE];
static tU8 procEx3Stack[PROC_Ex3_STACK_SIZE];

static tU8 initStack[INIT_STACK_SIZE];
static tU8 procStUsStack[PROC_ST_US_STACK_SIZE];

static tU8 pidEx1;
static tU8 pidEx2;
static tU8 pidEx3;

static tU8 pidStUs;


/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void procEx1(void* arg);
void procEx2(void* arg);
void procEx3(void* arg);

//int runPwm(void);

static void initProc(void* arg);
static void procStackUsage(void* arg);
//static tU16 getAnalogueInput(tU8 channel);

//static void initPwm(tU32 initialFreqValue);


//Exempel på avbrott (ljudsampling)
void Timer1ISRirq (void);  // skall inte ha något attribut när RTOS används
/****************************************************************************/



/****************************************************************************
 *
 * Globala konstanter och variabler
 *
 ****************************************************************************/

long const delayshort = 1200;
long const delaylong = 49250;

// TODO: temporary global variables, make local later if possible..
//tU32 duty;
//set frequency to 1000 Hz (1 kHz)
//tU32 const freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR)/ (VPBDIV_FACTOR * 1000));



/****************************************************************************
 *
 * Semaforer för att skydda gemensamma resurser
 * Används här som binära semaforer dvs antar bara värden 0 och 1 (mutex)
 *
 ****************************************************************************/

tCntSem mutexLCD;



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
//static void
//initPwm(tU32 initialFreqValue)
//{
//  /*
//   * initialize PWM
//   */
//  PWM_PR  = 0x00000000;             //set prescale to 0
//  PWM_MCR = 0x0002;                 //counter resets on MR0 match (period time)
//  PWM_MR0 = initialFreqValue;       //MR0 = period cycle time
//  PWM_MR2 = 0;                      //MR2 = duty cycle control, initial = 0%
//  PWM_LER = 0x05;                   //latch new values for MR0 and MR2
//  PWM_PCR = 0x0400;                 //enable PWM2 in single edge control mode
//  PWM_TCR = 0x09;                   //enable PWM and Counter
//
//  /*
//   * connect signal PWM2 to pin P0.7
//   */
//  PINSEL0 &= ~0x0000c000;  //clear bits related to P0.7
//  PINSEL0 |=  0x00008000;  //connect signal PWM2 to P0.7 (second alternative function)
//}


/*****************************************************************************
 *
 * Main
 *
 ****************************************************************************/
int
main(void)
{
  tU8 error;
  tU8 pid;

  //initLightSwitchPins();

  //FÖR LJUSSENSORN:
  //IODIR |= P06; // Sätter P0.6 till output
  //ODIR &= ~P029; // Sätter P029 till input
   //IOSET = P06; //Sätter p06 hög

  //För LJUSSENSOR+Digital Switch
    //PINSEL1 |= 0x05080000;	 //set AIN1 = P0.28, set AIN2 = P0.29,set Aout (DAC) = P0.25

    // New PINSEL1. Set AIN2 = P0.29, set Aout (DAC) = P0.25. DON'T Set AIN1 = P0.28!
    PINSEL1 |= 0x04080000; //set AIN2 = P0.29,set Aout (DAC) = P0.25
    // NOTE: Switchen är kopplad till P0.28. P0.28 använder Bit 24 och 25 i PINSEL1.
    // Dessa bitar ska vara 0 för digital switch. Detta är de från början!



  osInit();

  init_LCD();
  delay(delaylong);
  send_instruction(0xC);  //släck cursorn

  // init PWM variables
  //initPwmVars();

  //initialize PWM unit
  //initPwm(freq);

 // Här kan diverse initeringar läggas
 // Alternativt gör detta i initieringprocessen
 // Lägg märke till att avbrott inte bör enablas för än när alla processer startats

//  osInit();

  osCreateProcess(initProc, initStack, INIT_STACK_SIZE, &pid, 1, NULL, &error);
  osStartProcess(pid, &error);

  osStart();
  return 0;
}

//void initPwmVars(void) {
//
//	  // REMARK: make other inits here (or remove this method)
//}





/*****************************************************************************
 *
 *  Denna process kan användas för att kolla hur mycket stort utrymme
 *  som stacken använder
 *  Ta bort kommateringen av printf-raden och sätt in rätt pid för processen som
 *  skall kollas
 *
 ****************************************************************************/
static void
procStackUsage(void* arg)
{
  for(;;)
  {
    osSleep(5500);
//    printf("Stack usage: %d %%\n", osStackUsage(pidEx1));
  }
}
/*****************************************************************************
 *
 * Initieringprocessen som startar upp de andra processerna
 * Här är lämpligt att tex initiera avbrott. Initeringar kan alternativt läggas i main()
 * Men enabla inte avbrotten förrän på slutet av denna process
 * när alla andra processerna startat
 *
 * Som exempel visas avbrottet för att sampla ljud
 *
 ****************************************************************************/
static void
initProc(void* arg)
{
  tU8 error;

  eaInit();

  printf("RTOS startar \n");

 // Initiering av semaforerna. Alla ges värdet 1 från början
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
  * Detta är ett exempel på avbrottshantering
  * Kod för initiering av AD och DA-omvandlare och Timer 1 som skall ge avbrott (8 KHz)
  */

  //set AIN1 = P0.28
 // PINSEL1 |= 0x01080000;

 //initialize ADC (fel i beräkningen av divisionsfaktorn i EA-kod, rättat här)
   ADCR = (1 << 0)                             |  //SEL = 1, dummy channel #1
          ((CRYSTAL_FREQUENCY *
            PLL_FACTOR /
            VPBDIV_FACTOR) / 4500000) << 8 |  //set clock division factor, so ADC clock is 4.5MHz
          (0 << 16)                            |  //BURST = 0, conversions are SW controlled
          (0 << 17)                            |  //CLKS  = 0, 11 clocks = 10-bit result
          (1 << 21)                            |  //PDN   = 1, ADC is active
          (1 << 24)                            |  //START = 1, start a conversion now
          (0 << 27);							                 //EDGE  = 0, not relevant when start=1

   while((ADDR & 0x80000000) == 0)



 // Initiera timer 1 (som skall sköta avbrottet)

  TIMER1_TCR = 0x02;          //stop and reset timer
  TIMER1_PR  = 0x00;          //set prescaler to zero
  TIMER1_MCR = 0x03;		  //ställ up matchregister 0 för avbrott och reset av timer 0
  TIMER1_MR0 = 7500;		  //Detta värde avgör sampletiden, här 0.125 ms  (8000 Hz)
  TIMER1_IR  = 0xff;          //reset all interrrupt flags
  TIMER1_TCR = 0x01;          //start timer


  //Initiera avbrott vid match på MR0
  // Numret på VICVectCntl och VICVectAddr bestämmer prioriteten

  VICIntSelect&=~0x000000020;		//IRQ, inte FIQ-avbrott
  VICVectCntl7  =  0x00000025; //Enabla interuptchannel 5 (Timer1)
  VICVectAddr7  =  (tU32)Timer1ISRirq; //adressen till avbrottsrutinen


// enabla interrupt

  VICIntEnable|=0x000000020;		//Enabla Timer1 som irqavbrott

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
void
appTick(tU32 elapsedTime)
{
}
