/******************************************************************************
 *
 * Bas för Project12
 *
 * RTOS kompilerat för max 10 processer, 5 prioritetsnivåer
 * link_32k_512k_rom.ld ändrad så att ramadressen börjar på 0x40000200.
 * Newlib används inte.
 *
 * Alla ändringar när det gäller processortyp mm är gjorda.
 * Även förberett för on chip debugging.
 *
 * Projektet visar hur man kan ha processerna i olika filer, vilket är bra
 * när man är många som jobbar med samma projekt.
 * Namnen på källkodsfilerna måste anges i den yttre makefilen.
 *
 * Pga av problem med BF_wait för vissa kort används i stället delay() i exemplen.
 *
 * I projektet visas även hur man hanterar avbrott. Exemplet samplar en analog signal
 * 8000 gånger/sekund och lägger sen ut värdena på DA-omvandlaren.
 *
 * Tommy 121108
 *
 *****************************************************************************/

// QUESTION: how many ms is a tick (RTOS)?

#include	"main.h"

#define 	DEBUG			CUSTOMSETTINGS_DEBUG
#define		SAMPLE_FREQ		CUSTOMSETTINGS_SAMPLE_FREQ


/*****************************************************************************
 * Main
 ****************************************************************************/
int main(void) {

	main_init();	// initiering

	osStart();
	return 0;
}

void main_init() {
	tU8 error;
	tU8 pid;

	osInit();

	init_LCD();
	delay(delaylong);
	send_instruction(0xC);  //släck cursorn

	// Här kan diverse initeringar läggas
	// Alternativt gör detta i initieringprocessen
	// Lägg märke till att avbrott inte bör enablas för än när alla processer startats

	osCreateProcess(initProc, initStack, INIT_STACK_SIZE, &pid, 1, NULL,
			&error);
	osStartProcess(pid, &error);
}





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
    printf("Stack usage proc1: %d %%\n", osStackUsage(pidEx1));
    printf("Stack usage proc2: %d %%\n", osStackUsage(pidEx2));
    printf("Stack usage proc3: %d %%\n", osStackUsage(pidEx3));
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
	main_initProcesses();		// init processes
	main_initInterrupts();		// init interrupts

	osDeleteProcess();			// deletes this process
								// QUESTION: why is this a process? could implement the functionality of this process in a function..
}



void main_initProcesses() {
	tU8 error;

	eaInit();
	if (DEBUG)
		printf("RTOS startar \n");

	// Initiering av semaforerna. Alla ges värdet 1 från början
	osSemInit(&mutexLCD, 1);
	osCreateProcess(procEx1, procEx1Stack, PROC_Ex1_STACK_SIZE, &pidEx1, 3,
			NULL, &error);
	osStartProcess(pidEx1, &error);
	osCreateProcess(procEx2, procEx2Stack, PROC_Ex2_STACK_SIZE, &pidEx2, 3,
			NULL, &error);
	osStartProcess(pidEx2, &error);
	osCreateProcess(procEx3, procEx3Stack, PROC_Ex3_STACK_SIZE, &pidEx3, 3,
			NULL, &error);
	osStartProcess(pidEx3, &error);
	osCreateProcess(procStackUsage, procStUsStack, PROC_ST_US_STACK_SIZE,
			&pidStUs, 4, NULL, &error);
	osStartProcess(pidStUs, &error);
}

void main_initInterrupts() {

	main_initTimer1();
	// TODO: init other interrupts here

}

void main_initTimer1() {


	/****************************************************************************************
	 * Detta är ett exempel på avbrottshantering
	 * Kod för initiering av AD och DA-omvandlare och Timer 1 som skall ge avbrott (8 KHz)
	 ****************************************************************************************/

	//set AIN1 = P0.28
	PINSEL1 |= 0x01080000;

	//initialize ADC (fel i beräkningen av divisionsfaktorn i EA-kod, rättat här)
	ADCR = (1 << 0) |	//SEL = 1, dummy channel #1
			((CRYSTAL_FREQUENCY * PLL_FACTOR / VPBDIV_FACTOR) / 4500000) << 8 | //set clock division factor, so ADC clock is 4.5MHz
			(0 << 16) |	//BURST = 0, conversions are SW controlled
			(0 << 17) |	//CLKS  = 0, 11 clocks = 10-bit result
			(1 << 21) |	//PDN   = 1, ADC is active
			(1 << 24) |	//START = 1, start a conversion now
			(0 << 27);					//EDGE  = 0, not relevant when start=1

	while ((ADDR & 0x80000000) == 0)

		// REMARK: this is where you set the sampling frequency of lightsensor input reading and processing?
		// Initiera timer 1 (som skall sköta avbrottet)
		TIMER1_TCR = 0x02;          //stop and reset timer
	TIMER1_PR = 0x00;          //set prescaler to zero
	TIMER1_MCR = 0x03; //ställ up matchregister 0 för avbrott och reset av timer 0
	TIMER1_MR0 = (60000000 / SAMPLE_FREQ);	//Detta värde avgör sampletiden,
											//exempelvis ger ett värde på 7500 en sampeltid på 0.125 ms (8000 Hz)
	TIMER1_IR = 0xff;          //reset all interrupt flags
	TIMER1_TCR = 0x01;          //start timer

	// Initiera avbrott vid match på MR0
	// Numret på VICVectCntl och VICVectAddr bestämmer prioriteten
	// TEMPLATE:	VICVectCntl["slot"] = 0x20 | ["VIC_channel"]
	//			where "slot" is the priority slot of the VIC Control registers (from 0 to 15, 0 has highest priority)
	//			and "VIC_channel" is the VIC channel for the specified flag/function (as specified by Table 5-57 in manualLPC2148.pdf)
	VICIntSelect &= ~0x000000020;		// Timer1 sätts till IRQ
	VICVectCntl7 = 0x00000025;// tilldela interuptchannel 5 (Timer1) till slot 7
	VICVectAddr7 = (tU32) Timer1ISRirq;	// adressen till avbrottsrutinen på slot 7
	VICIntEnable |= 0x000000020;		// enabla Timer1 som avbrott

	//*************slut ljudavbrottsinitering****************

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
// REMARK:	2.5 ms per osTick is hardcoded/compiled for this project (the OS reference of time is generated by TIMER0 of the LPC2148)
// EDIT:	have confirmed by timing printf-outputs that 2.5 ms is a decent approximation of the time it takes before each osTick is triggered
void
appTick(tU32 elapsedTime)
{
	static tU16 counter_appTick = 0;

	if (DEBUG) {
		counter_appTick++;
		if (counter_appTick == 400) {	// 2.5ms/tick hardcoded
			printf("\ncounter_appTick: %d", counter_appTick);
			counter_appTick = 0;
		}
	}

}
