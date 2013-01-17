
/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../general.h"
#include <lpc2xxx.h>
#include "../startup/config.h"
#include "../utils/lpc214x.h"

/******************************************************************************
 * Defines
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

#define MODE_FORWARD	1
#define MODE_REVERSE	2
#define MODE_BRAKE		3

/*****************************************************************************
 * Global variables
 ****************************************************************************/
static tU32 duty1;
static tU32 duty2;

/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void init();
void initPwm(tU32 initialFreqValue);
void initPins();
void init_EINT0();
void init_EINT3();
void init_vic();
void init_io();

void setMode1(short mode);
void setMode2(short mode);
void setPwmDutyPercent1(tU32 dutyValue1);
void setPwmDutyPercent2(tU32 dutyValue2);	// QUESTION: should these functions be set as static? (only accessible from within this translation unit)
											// EDIT: they have been set to implicitly extern (they are accessible from other translation units)


