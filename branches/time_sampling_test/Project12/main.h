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
#include "utils/utils.c"

#include "LCD/LCD.h"  //  Funktionsprototyper för LCD-rutinerna
#include "startup/custom_settings.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/
// Dessa definitioner används av en del av EA's rutiner
#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD


// Stackstorlekar för processerna. Kan behöva justeras. Se beskrivning av procStackUsage nedan
#define PROC_Ex1_STACK_SIZE 1024
#define PROC_Ex2_STACK_SIZE 1024
#define PROC_Ex3_STACK_SIZE 1024

#define INIT_STACK_SIZE			1024
#define PROC_ST_US_STACK_SIZE	1024

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

long const delayshort = 1200;
long const delaylong = 49250;

tCntSem mutexLCD;	//	Semaforer för att skydda gemensamma resurser,
					//	används här som binära semaforer dvs antar bara värden 0 och 1 (mutex)

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void procEx1(void* arg);
void procEx2(void* arg);
void procEx3(void* arg);
static void initProc(void* arg);
static void procStackUsage(void* arg);

//Exempel på avbrott (ljudsampling)
void Timer1ISRirq (void);  // skall inte ha något attribut när RTOS används

void main_init(void);
void main_initProcesses(void);
void main_initInterrupts(void);
void main_initTimer1(void);
/****************************************************************************/
