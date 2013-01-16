
// Egna defines
#define CUSTOMSETTINGS_SAMPLE_FREQ								8000		// sample frequency in Hertz
#define CUSTOMSETTINGS_DEBUG									TRUE		// sets debug mode
#define CUSTOMSETTINGS_PERFORM_AD_READ							FALSE		// perform AD read (lightsensor)
#define CUSTOMSETTINGS_TMR1_ISR_COUNTER_RESET_MULTIPLIER		9			// if counter_timer1 is of the datatype tU16: 9 seems to be the most optimal setting (deduced heuristically)
																			// if counter_timer1 is of the datatype tU32: ---
																			// REMARK: doesn't seem to be any correlation between setting CUSTOMSETTINGS_TMR1_ISR_COUNTER_RESET_MULTIPLIER to ((2^bits_of_datatype) / CUSTOMSETTINGS_SAMPLE_FREQ)
																			//		   to reduce the amount of code that is compiled
																			// REMARK: compiling excessive amounts of code will lead to a quadrupled amount of TEXT and ROM
