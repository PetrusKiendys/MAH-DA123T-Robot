#include "../general.h"
#include <lpc2xxx.h>
#include "../startup/config.h"

#define CRYSTAL_FREQUENCY FOSC
#define PLL_FACTOR        PLL_MUL
#define VPBDIV_FACTOR     PBSD

#define TASK 2
#define MODE 3
#define MODE2 3

static void initPwm(tU32 initialFreqValue);
static void setPwmDutyPercent1(tU32 dutyValue1);
static void setPwmDutyPercent2(tU32 dutyValue2);
static void delayMs(tU16 delayInMs);
void setMode1();
void setMode2();
void drive(tU32 duty1, tU32 duty2);

static void initPwm(tU32 initialFreqValue) {
	/*
	 * initialize PWM
	 */
	PWM_PR = 0x00000000; //set prescale to 0
	PWM_MCR = 0x0002; //counter resets on MR0 match (period time)
	PWM_MR0 = initialFreqValue; //MR0 = period cycle time
	PWM_MR2 = 0; //MR2 = duty cycle control, initial = 0%
	PWM_MR5 = 0; //MR5 = duty cycle control, initial = 0%
	PWM_LER = 0x25; //latch new values for MR0 and MR2 and MR5
	PWM_PCR = 0x2400; //enable PWM2 and in single edge control mode
	PWM_TCR = 0x09; //enable PWM and Counter
}

static void setPwmDutyPercent1(tU32 dutyValue1) {
	PWM_MR2 = (PWM_MR0 * dutyValue1) / 10000; //update duty cycle
	PWM_LER = 0x04; //latch new values for MR2
}

static void setPwmDutyPercent2(tU32 dutyValue2) {
	PWM_MR5 = (PWM_MR0 * dutyValue2) / 10000; //update duty cycle
	PWM_LER = 0x20; //latch new values for MR5
}

static void delayMs(tU16 delayInMs) {
	/*
	 * setup timer #1 for delay
	 */
	TIMER1_TCR = 0x02; //stop and reset timer
	TIMER1_PR = 0x00; //set prescaler to zero
	TIMER1_MR0 = delayInMs * ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (1000
			* VPBDIV_FACTOR));
	TIMER1_IR = 0xff; //reset all interrrupt flags
	TIMER1_MCR = 0x04; //stop timer on match
	TIMER1_TCR = 0x01; //start timer

	//wait until delay time has elapsed
	while (TIMER1_TCR & 0x01)
		;
}

void initPins() {
	// PWM2 - P0.7
	PINSEL0 &= ~0x0000c000;
	PINSEL0 |= 0x00008000;

	PINSEL0 &= ~0xC0000000;
	PINSEL0 |= 0x80000000;

	PINSEL1 &= ~0x000C0000;

	//PWM5 - P0.21
	PINSEL1 &= ~0x00000c00;
	PINSEL1 |= 0x00000400;

	PINSEL1 &= ~0x0000c000;

	PINSEL1 &= ~0xc0000000;
	//PINSEL1 |= 0x80000000;
}

void setMode1() {
	if (MODE == 1) { // Framåt
		IODIR |= (1 << 7);
		IOCLR = (1 << 7);

		IODIR |= (1 << 12);
		IOSET = (1 << 12);

		IODIR |= (1 << 25);
		IOSET = (1 << 25);
	} else if (MODE == 2) { // Bakåt
		IODIR |= (1 << 7);
		IOCLR = (1 << 7);

		IODIR |= (1 << 12);
		IOCLR = (1 << 12);

		IODIR |= (1 << 25);
		IOSET = (1 << 25);
	} else if (MODE == 3) { // Broms
		IODIR |= (1 << 7);
		IOCLR = (1 << 7);

		IODIR |= (1 << 12);
		IOCLR = (1 << 12);

		IODIR |= (1 << 25);
		IOCLR = (1 << 25);
	}
}
void setMode2() {
	if (MODE2 == 1) { // Framåt
		IODIR |= (1 << 21);
		IOCLR = (1 << 21);

		IODIR |= (1 << 23);
		IOSET = (1 << 23);

		IODIR |= (1 << 31);
		IOSET = (1 << 31);
	} else if (MODE2 == 2) { // Bakåt
		IODIR |= (1 << 21);
		IOCLR = (1 << 21);

		IODIR |= (1 << 23);
		IOCLR = (1 << 23);

		IODIR |= (1 << 31);
		IOSET = (1 << 31);
	} else if (MODE2 == 3) { // Broms
		IODIR |= (1 << 21);
		IOCLR = (1 << 21);

		IODIR |= (1 << 23);
		IOCLR = (1 << 23);

		IODIR |= (1 << 31);
		IOCLR = (1 << 31);
	}
}

int main() {
	tU32 duty1;
	tU32 duty2;
	tU32 freq;

	freq = ((CRYSTAL_FREQUENCY * PLL_FACTOR) / (VPBDIV_FACTOR * 1000));
	initPwm(freq);
	initPins();

	duty1 = 0;
	duty2 = 0;

	setMode1();
	setMode2();

	drive(duty1, duty2);

	return 0;
}

void drive(tU32 duty1, tU32 duty2) {
	while (1) {
		setPwmDutyPercent1(duty1);
		setPwmDutyPercent2(duty2);

		delayMs(10);

		if (TASK == 1) {
			duty1 += 10;
			if (duty1 > 10000)
				duty1 = 0;
			// ursprungliga
		} else if (TASK == 2) {
			duty1 = 0;
			duty2 = 0;
			// Framåt, maxfart
		} else if (TASK == 3) {
			// Svänga
		} else if (TASK == 4) {
			// Svänga
		} else if (TASK == 5) {
			// Bakåt
		}
	}
}

