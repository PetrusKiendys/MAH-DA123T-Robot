/*****************************************************************************
 * Function prototypes
 ****************************************************************************/
void setState(int newState);
void switchState(int newState);
void stat1Function();
void state2Function();

/*****************************************************************************
 * Variables
 ****************************************************************************/
/*int state;

void setState(int newState)
{
	state = newState;
}

void switchState(int newState) {
	state = newState;
	switch(newState){

	case 0:

		break;
	case 1:
		state1Function();
		break;
	case 2:
		state2Function();
		break;
	case 3:
		break;
	}
}

void state1Function()
{

	int motorPulseCount = 0 ;			//nollst�ll r�knare
	int X;
	int Y;

	motor(DRIVE SLOW);			// f� robot att k�ra sakta f�r att se till att den hittar en s� rak v�g som m�jligt

	driveNbrOfPulses(X);		// f�r robot forts�tta k�ra l�ngs linjen i X antal pulser

	fuckTheLine();				// Sluta f�lj pulser och k�r bara rakt fram
	motorPulseCount = 0;		// Nollst�ll pulsr�knade
	motor(DRIVE FORWARD);		// K�r bara rakt
	driveNbrOfPulses(Y);		// k�r Y antal pulser (�ver vita ytan utan linje)

	TurnLeftUntilLineIsFound(); // Leta upp linje genom att sv�nga v�nster (f�rutsatt att roboten kom p� h�ger sida om linjen)
	FollowLine();				// F�lj linjen igen

}

void state2Function()
{
	int X = 0;
	int Y;
	int Z;
	int A;

	motorPulseCount = 0; 		//nollst�ll r�knare
	motor(DRIVE_SLOW); 			// f� robot att k�ra sakta f�r att h�lla koll p� n�r den kommer till laborinten

	driveNbrOfPulses(X);		// f�r robot att k�ra X antal pulser tills den kommer till laborinten

	fuckTheLine();				// sluta f�lj linje
	motorPulseCount = 0;		// �terst�ll pulsr�knare

	driveNbrOfPulses(Y);		// k�r Y antal pulser in i laborinten

	turn90degreesLeft();		// sv�ng 90 grader �t v�nster
	motorPulseCount = 0;		// nollst�ll r�knare
	driveNbrOfPulses(Z);		// k�r Z antal pulser i laborinten

	turn90degreesRight();		// sv�ng 90 grader �t h�ger
	motorPulseCount = 0;		// nollst�ll r�knare
	driveNbrOfPulses(A);		// K�r A antal pulser i laborinten

	turn90degreesRight();		// sv�ng 90 grader �t h�ger
	motorPulseCount = 0;		// nollst�ll r�knare
	driveNbrOfPulses(B);		// k�r B antal pulser i laborinten

	turn90degreesLeft();		// sv�ng 90 grader �t v�nster
	motorPulseCount = 0;		// nollst�ll r�knare
	driveNbrOfPulsese(C);		// k�r C antal pulser ut laborinten

	findLine();					// find black line again
	followLine();				// start fallowing line again



}

*/
