/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void setState(int newState);
int getState();
void switchState(int newState);
void pauseState();
void pauseState();

void stat1Function();
void state2Function();

/*****************************************************************************
 * Variables
 ****************************************************************************/
int state;
static int oldState;

void setState(int newState)
{
	state = newState;
}

int getState() {
	return state;
}

void switchState(int newState) {
	if( state != 5 ) {
		state = newState;
	} else {
		oldState = newState;
	}
/*	switch(newState){

	case 0:

		break;

	}
	*/
}

void pauseState() {
	if( state != 5 ) {
		oldState = state;
		setState(5);
	}
}

void stopPauseState() {
	setState(oldState);
}
/*
void state1Function()
{

	int motorPulseCount = 0 ;			//nollställ räknare
	int X;
	int Y;

	motor(DRIVE SLOW);			// få robot att köra sakta för att se till att den hittar en så rak väg som möjligt

	driveNbrOfPulses(X);		// får robot fortsätta köra längs linjen i X antal pulser

	fuckTheLine();				// Sluta följ pulser och kör bara rakt fram
	motorPulseCount = 0;		// Nollställ pulsräknade
	motor(DRIVE FORWARD);		// Kör bara rakt
	driveNbrOfPulses(Y);		// kör Y antal pulser (över vita ytan utan linje)

	TurnLeftUntilLineIsFound(); // Leta upp linje genom att svänga vänster (förutsatt att roboten kom på höger sida om linjen)
	FollowLine();				// Följ linjen igen

}
*/
/*
void state2Function()
{
	int X = 0;
	int Y;
	int Z;
	int A;

	motorPulseCount = 0; 		//nollställ räknare
	motor(DRIVE_SLOW); 			// få robot att köra sakta för att hålla koll på när den kommer till laborinten

	driveNbrOfPulses(X);		// får robot att köra X antal pulser tills den kommer till laborinten

	fuckTheLine();				// sluta följ linje
	motorPulseCount = 0;		// återställ pulsräknare

	driveNbrOfPulses(Y);		// kör Y antal pulser in i laborinten

	turn90degreesLeft();		// sväng 90 grader åt vänster
	motorPulseCount = 0;		// nollställ räknare
	driveNbrOfPulses(Z);		// kör Z antal pulser i laborinten

	turn90degreesRight();		// sväng 90 grader åt höger
	motorPulseCount = 0;		// nollställ räknare
	driveNbrOfPulses(A);		// Kör A antal pulser i laborinten

	turn90degreesRight();		// sväng 90 grader åt höger
	motorPulseCount = 0;		// nollställ räknare
	driveNbrOfPulses(B);		// kör B antal pulser i laborinten

	turn90degreesLeft();		// sväng 90 grader åt vänster
	motorPulseCount = 0;		// nollställ räknare
	driveNbrOfPulsese(C);		// kör C antal pulser ut laborinten

	findLine();					// find black line again
	followLine();				// start fallowing line again



}
*/

