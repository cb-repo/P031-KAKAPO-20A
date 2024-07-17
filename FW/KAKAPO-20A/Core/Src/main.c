
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "main.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#define CONFIG_HASH_A					0x8ca86362
#define CONFIG_HASH_B					(CONFIG_HASH_A + 1)

#define CALIBRATE_STARTUP_TIMEOUT		10000
#define CALIBRATE_INPUT_WIGGLE			20
#define CALIBRATE_TEST_DELAY			1000
#define CALIBRATE_INPUT_DELAY			250
#define CALIBRATE_MOTOR_JERKLONG		1000
#define CALIBRATE_MOTOR_JERK			100
#define CALIBRATE_MOTORBRAKE_TIMEOUT	5000


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE TYPES										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE PROTOTYPES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static void 	SYSTEM_Init							( void );
static void 	SYSTEM_Update						( void );

static bool 	SYSTEM_initiateCalibration			( void );
static void 	SYSTEM_runCalibration 				( void );
static void 	SYSTEM_calibrationMotor_Same 		( void );
static void 	SYSTEM_calibrationMotor_Opposite	( void );
//static void 	SYSTEM_calibrateMotor_enBrake 		( void );
static void 	SYSTEM_wait_ActiveInput 			( void );
static void 	SYSTEM_wait_ResetInputs 			( void );
static uint32_t SYSTEM_count_ActiveInputs 			( void );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE VARIABLES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static bool configVerified = false;
static CONFIG_data config = {0};

static RADIO_Data* dataPtr = 0;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


int main ( void ) {
	// STARTUP PROCEDURE
	CORE_Init();
	SYSTEM_Init();
	ADC_Init();
	LED_Init();
	TEMP_Init();

	RADIO_Init();
	MOTOR_Init();

//	CORE_Delay(900); // Not noticible to people but should allow voltage recovery. Could update this to only happen under brownout condition

//	BATT_Init();
//
//	uint32_t tick = CORE_GetTick();
//	while ( (CORE_GetTick() - tick) < 100 ) {
//		BATT_Update();
//		TEMP_Update();
//		RADIO_Update();
//		CORE_Idle();
//	}


//	MOTOR_StartupBuzz();

	// Loop To Infinity and Beyond
	while(1)
	{
//		// Update Input Modules
//		BATT_Update();
//		TEMP_Update();
//		RADIO_Update();
//		LED_Update();
//
//		// Run System Updates
//		SYSTEM_Update();
//		MOTOR_Update();

		// Loop Pacing
		CORE_Idle();
	}
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE FUNCTIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
 *
 */
void SYSTEM_Init ( void )
{
	// READ IN CONFIGURATION FROM FPROM
	FPROM_Read( CONFIG_FPROM_OFFSET, &config, sizeof(config) );

	// CHECK FOR VALID CONFIG
	// TODO: Actually check all calibration values are valid
	// TODO: Do a second read following the write
	// CHECK FOR CORRECT CONFIG HASH VALUES
	if ((config.hashA != CONFIG_HASH_A) || (config.hashB != CONFIG_HASH_B))
	{
		// FRESH DEVICE USED, OF FAILED WRITE, SO WRITE DEFAULT CONFIG TO EEPROM
		config.hashA = 				CONFIG_HASH_A;
		config.mode = 				CONFIG_ARCADE;
		config.motorBraking_EN =	true;
		config.motorA_ch = 			0;
		config.motorA_rev = 		false;
		config.motorB_ch = 			1;
		config.motorB_rev = 		false;
		config.hashB = 				CONFIG_HASH_B;

		FPROM_Write(CONFIG_FPROM_OFFSET, &config, sizeof(config));
	}

	//
	configVerified = true;

	//
	dataPtr = RADIO_getDataPtr();
}


/*
 *
 */
static void SYSTEM_Update ( void )
{
	// FUNCTION VARIABLES
	static bool calibrateWindow = true;

	// ARE WE STILL WITHIN THE BOOT CALIBRATION WINDOW
	if ( calibrateWindow )
	{
		// HAS THE USER INDICATED THEY WANT TO CALIBRATE DEVICE
		if ( SYSTEM_initiateCalibration() ) {
			// RUN CALIBRATION SEQUENCE
			SYSTEM_runCalibration();
			calibrateWindow = false;
		}
		// HAS THE WINDOW ELAPSED
		else if ( CORE_GetTick() >= CALIBRATE_STARTUP_TIMEOUT ) {
			calibrateWindow = false;
		}
	}
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE FUNCTIONS - CONFIG 							*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 *
 */
static bool SYSTEM_initiateCalibration ( void )
{
	// INIT LOOP VARIABLES
	static RADIO_chActiveFlags	chActive_p[RADIO_NUM_CHANNELS]	= {chActive_False};
	static uint8_t 				chCounter[RADIO_NUM_CHANNELS] 	= {0};
	bool 						retVal 							= false;

	// ONLY PROCEED IF THERE IS A RADIO DETECTED
	if ( !RADIO_inFaultStateANY() )
	{
		// ITTERATE THROUGH EACH AVALIBLE RADIO CHANNEL
		for ( uint8_t ch = 0; ch < RADIO_NUM_CHANNELS; ch++ )
		{
			// IF THE CHANNEL IS NOW INACTIVE BUT WAS PREVIOUSLY ACTIVE
			if ( (dataPtr->chActive[ch] == chActive_False) &&
				 ((chActive_p[ch] == chActive_True) || (chActive_p[ch] == chActive_TrueRev )) ) {
				// INCREMENT THE COUNTER
				chCounter[ch] += 1;
			}

			// CHECK IF ENOUGH INPUT WIGGLES HAVE BEEN DONE
			if ( chCounter[ch] > CALIBRATE_INPUT_WIGGLE ) {
				retVal = true;
				break;
			}

			// UPDATE VARIABLES FOR NEXT LOOP
			chActive_p[ch] = dataPtr->chActive[ch];
		}
	}

	return retVal;
}


/*
 * TEXT
 */
static void SYSTEM_runCalibration (void)
{
	//
	MOTOR_calibASSERT();

	// TURN ON LEDS TO TELL OPERATOR CALIBRATION IS STARTING
	LED_pulseOutputs(10);
	LED_calibUpdate(true, true);

	// WAIT FOR USER TO PRESS A STICK TO INITATE CALIBRATION SEQUENCE
	SYSTEM_wait_ActiveInput();
	SYSTEM_wait_ResetInputs();

	// START TEST
	LED_calibUpdate(false, false);
	CORE_Delay(CALIBRATE_TEST_DELAY);
	MOTOR_calibTwitchFWD( 500 );
	CORE_Delay(CALIBRATE_MOTOR_JERK);
	LED_calibUpdate(true, false);
	SYSTEM_calibrationMotor_Same();
	LED_calibUpdate(true, true);
	SYSTEM_wait_ResetInputs();
	LED_pulseOutputs(3);

	LED_calibUpdate(false, false);
	CORE_Delay(CALIBRATE_TEST_DELAY);
	MOTOR_calibTwitchLEFT( 500 );
	CORE_Delay(CALIBRATE_MOTOR_JERK);
	LED_calibUpdate(true, false);
	SYSTEM_calibrationMotor_Opposite();
	LED_calibUpdate(true, true);
	SYSTEM_wait_ResetInputs();
	LED_pulseOutputs(3);

//	LED_calibUpdate(false, false);
//	CORE_Delay(CALIBRATE_TEST_DELAY);
//	MOTOR_calibTwitchFWD( CALIBRATE_MOTOR_JERK );
//	CORE_Delay(CALIBRATE_MOTOR_JERK);
//	LED_calibUpdate(true, false);
//	SYSTEM_calibrateMotor_enBrake();
//	LED_calibUpdate(true, true);
//	SYSTEM_wait_ResetInputs();
//	if ( config.motorBraking_EN ) {
//		CORE_Delay(CALIBRATE_TEST_DELAY);
//		MOTOR_calibTwitchFWD( CALIBRATE_MOTOR_JERK );
//	}

	FPROM_Write(CONFIG_FPROM_OFFSET, &config, sizeof(config));

	MOTOR_getConfig();

	// PULSE LED TO LET USER KNOW SUCCESSFUL
	LED_pulseOutputs(10);
	LED_calibUpdate(false, false);

	// Reinitialize Outputs
	MOTOR_calibRELEASE();
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static void SYSTEM_calibrationMotor_Same ( void )
{
	// LOOP UNTIL BREAK CONDITION REACHED
	while (1)
	{
		// INIT LOOP VARIABLES
		uint32_t tick = CORE_GetTick();

		// UPDATE RADIO DATA AND COUNT ACTIVE INPUTS
		RADIO_Update();
		uint32_t chActive = SYSTEM_count_ActiveInputs();

		// START BREAK SEQUENCE FOR ARCADE INPUT DRIVE STYLE
		if (chActive == 1)
		{
			config.mode = CONFIG_ARCADE;
			break;
		}
		// START BREAK SEQUENCE FOR TANK INPUT DRIVE STYLE
		else if (chActive == 2)
		{
			config.mode = CONFIG_TANK;
			break;
		}

		// LOOP PACING
		while (CALIBRATE_INPUT_DELAY >= (CORE_GetTick() - tick))
		{
			RADIO_Update();
			CORE_Idle();
		}
	}

	// ITTERATE THROUGH EACH CHANNEL TO FIND THE PUSHED STICK
	for (uint8_t ch = 0; ch < RADIO_NUM_CHANNELS; ch++)
	{
		// IF STICK IS ACTIVE IN POSITIVE DIRECTION
		if (dataPtr->chActive[ch] == chActive_True)
		{
			config.motorA_ch = ch;
			config.motorA_rev = false;
			break;
		}
		// IF STICK IS ACTIVE IN NEGATIVE DIRECTION
		else if (dataPtr->chActive[ch] == chActive_TrueRev)
		{
			config.motorA_ch = ch;
			config.motorA_rev = true;
			break;
		}
	}

	// IF TANK MODE WAS SELECTED
	if (config.mode == CONFIG_TANK)
	{
		// CONTINUE ITTERATING THROUGH EACH CHANNEL TO FIND THE SECOND PUSHED STICK
		for (uint8_t ch = config.motorA_ch; ch < RADIO_NUM_CHANNELS; ch++)
		{
			// CHECK STICK DOESNT MATCH DRIVEA
			if ( ch != config.motorA_ch )
			{
				// IF STICK IS ACTIVE IN POSITIVE DIRECTION
				if (dataPtr->chActive[ch] == chActive_True)
				{
					config.motorB_ch = ch;
					config.motorB_rev = false;
					break;
				}
				// IF STICK IS ACTIVE IN NEGATIVE DIRECTION
				else if (dataPtr->chActive[ch] == chActive_TrueRev)
				{
					config.motorB_ch = ch;
					config.motorB_rev = true;
					break;
				}
			}
		}
	}
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static void SYSTEM_calibrationMotor_Opposite ( void )
{
	// LOOP UNTIL BREAK CONDITION REACHED
	while (1)
	{
		// INIT LOOP VARIABLES
		uint32_t tick = CORE_GetTick();

		// UPDATE RADIO DATA AND COUNT ACTIVE INPUTS
		RADIO_Update();
		uint32_t chActive = SYSTEM_count_ActiveInputs();

		// START BREAK SEQUENCE FOR ARCADE INPUT DRIVE STYLE
		if (config.mode == CONFIG_ARCADE && chActive == 1 && dataPtr->chActive[config.motorA_ch] == chActive_False )
		{
			// ITTERATE THROUGH EVERY INPUT
			for (uint8_t ch = 0; ch < RADIO_NUM_CHANNELS; ch++)
			{
				// IF STICK IS ACTIVE IN POSITIVE DIRECTION
				if (dataPtr->chActive[ch] == chActive_True)
				{
					config.motorB_ch = ch;
					config.motorB_rev = true;
					break;
				}
				// IF STICK IS ACTIVE IN NEGATIVE DIRECTION
				else if (dataPtr->chActive[ch] == chActive_TrueRev)
				{
					config.motorB_ch = ch;
					config.motorB_rev = false;
					break;
				}
			}
			break;
		}
		// START BREAK SEQUENCE FOR TANK INPUT DRIVE STYLE
		else if (config.mode == CONFIG_TANK && chActive == 2 )
		{
			// CHECK FOR REVERSE CONDITION
			bool chRevDriveA = false;
			bool chRevDriveB = false;
			if (dataPtr->chActive[config.motorA_ch] == chActive_TrueRev) {
				chRevDriveA = true;
			}
			if (dataPtr->chActive[config.motorB_ch] == chActive_TrueRev) {
				chRevDriveB = true;
			}

			if ( dataPtr->chActive[config.motorA_ch] != chActive_False &&				// DriveA channel was one of the two active channels
					dataPtr->chActive[config.motorB_ch] != chActive_False &&				// DriveB channel was one of the two active channels
				 ((chRevDriveA == config.motorA_rev) != (chRevDriveB == config.motorB_rev)) )  // Check only one of the changed directions )
			{
				// IF DRIVEB HAS BEEN REVERSED THEN CHANNELS A AND B NEED TO BE SWAPPED
				if (chRevDriveB != config.motorB_rev) {
					CONFIG_data cInt;
					cInt.motorA_ch 		= config.motorA_ch;
					cInt.motorA_rev 	= config.motorA_rev;
					config.motorA_ch 		= config.motorB_ch;
					config.motorA_rev 		= config.motorB_rev;
					config.motorB_ch 		= cInt.motorA_ch;
					config.motorB_rev 		= cInt.motorA_rev;
				}
				break;
			}
		}

		// LOOP PACING
		while (CALIBRATE_INPUT_DELAY >= (CORE_GetTick() - tick))
		{
			RADIO_Update();
			CORE_Idle();
		}
	}
}


///*
// *
// */
//static void SYSTEM_calibrateMotor_enBrake ( void )
//{
//	// INIT FUNCTION VARIABLES
//	uint32_t tick = CORE_GetTick();
//
//	// LOOP UNTIL BREAK CONDITION REACHED
//	while (1)
//	{
//		// UPDATE INPUTS
//		RADIO_Update();
//
//		// CHECK IF MOTOR INPUT PRESSED - ENABLE MOTOR BRAKING
//		if ( SYSTEM_count_ActiveInputs() ) { //(ptrRadioData->chActive[config.motorA_ch] != chActive_False) || (ptrRadioData->chActive[config.motorB_ch] != chActive_False) ) {
//			config.motorBraking_EN = true;
//			break;
//		}
//		// CHECK IF ANY OTHER INPUT IT PRESSED OR TIMEOUT REACHED - DISABLE MOTOR BRAKING
//		else if ( CALIBRATE_MOTORBRAKE_TIMEOUT <= (CORE_GetTick() - tick) ) { //SYSTEM_count_ActiveInputs() || (CALIBRATE_MOTORBRAKE_TIMEOUT >= (CORE_GetTick() - tick)) ) {
//			config.motorBraking_EN = false;
//			break;
//		}
//
//		// LOOP PACING
//		CORE_Idle();
//	}
//}


/*
 * WAITS UNTIL ANY OF THE RADIO INPUTS ARE ACTIVE
 *
 * INPUTS: 	N/A
 * OUTPUTS: N/A
 */
static void SYSTEM_wait_ActiveInput ( void )
{
	// LOOP UNTIL BREAK CONDITION REACHED
	while (1)
	{
		// UPDATE RADIO DATA
		RADIO_Update();
		// BREAK IF ANY STICKS ARE PUSHED
		if ( SYSTEM_count_ActiveInputs() > 0 ) {
			break;
		}
		// LOOP PACING
		CORE_Idle();
	}
}


/*
 * WAITS UNTIL ALL RADIO INPUTS ARE BACK TO NEUTRAL POSITION
 *
 * INPUTS: 	N/A
 * OUTPUTS: N/A
 */
static void SYSTEM_wait_ResetInputs ( void )
{
	// LOOP UNTIL BREAK CONDITION REACHED
	while (1)
	{
		// UPDATE RADIO DATA
		RADIO_Update();
		// BREAK WHEN ALL STICKS HAVE RETURNED BACK TO ZERO POSITION
		if ( SYSTEM_count_ActiveInputs() == 0 ) {
			break;
		}
		// LOOP PACING
		CORE_Idle();
	}
}


/*
 * COUNTS AND RETURNS NUMBER OF THE RADIO INPUTS ARE ACTIVE
 *
 * INPUTS: 	N/A
 * OUTPUTS: NUMBER OF ACTIVE RADIO CHANNELS
 */
static uint32_t SYSTEM_count_ActiveInputs ( void )
{
	// INIT FUNCTION VARIABLES
	uint32_t retVal = 0;

	// ITTERATE THROUGH EACH RADIO INPUT
	for (uint8_t ch = 0; ch < RADIO_NUM_CHANNELS; ch++)
	{
		if ( (dataPtr->chActive[ch] == chActive_True) ||
			 (dataPtr->chActive[ch] == chActive_TrueRev) )
		{
			retVal += 1;
		}
	}

	// RETURN NUMBER OF ACTIVE INPUTS
	return retVal;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EVENT HANDLERS   									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* INTERRUPT ROUTINES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
