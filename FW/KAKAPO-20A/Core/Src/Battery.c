
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Battery.h"

#if !defined(BATT_Pin)
#error "BATT_Pin must be defined"
#endif

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define BATT_CELLH		4350 // Assume someone may use a LiHv, 4.35V per cell
#define BATT_CELLL 		3000

#define BATT_AUTOH_6S	(BATT_CELLH * 6 + 200)
#define BATT_AUTOH_5S	(BATT_CELLH * 5 + 200)
#define BATT_AUTOH_4S	(BATT_CELLH * 4 + 200)
#define BATT_AUTOH_3S	(BATT_CELLH * 3 + 200)
#define BATT_AUTOH_2S	(BATT_CELLH * 2 + 200)

#define FAULT_SET_MS	100
#define FAULT_RESET_MS	100

#define FAULT_SET_6S	(BATT_CELLL * 6)
#define FAULT_RESET_6S	(FAULT_SET_6S + 100)
#define FAULT_SET_5S	(BATT_CELLL * 5)
#define FAULT_RESET_5S	(FAULT_SET_5S + 100)
#define FAULT_SET_4S	(BATT_CELLL * 4)
#define FAULT_RESET_4S	(FAULT_SET_4S + 100)
#define FAULT_SET_3S	(BATT_CELLL * 3)
#define FAULT_RESET_3S	(FAULT_SET_3S + 100)
#define FAULT_SET_2S	(BATT_CELLL * 2)
#define FAULT_RESET_2S	(FAULT_SET_2S + 100)

#define BATT_ARRAY_LEN	3


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE TYPES										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE PROTOTYPES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static uint32_t BATT_calcStartup 	( void );
static uint32_t BATT_calc 			( void );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE VARIABLES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static bool		faultVolt = false;
static uint32_t faultVolt_set;
static uint32_t faultVolt_reset;

static uint32_t battArray[ BATT_ARRAY_LEN ] = {0};
static uint32_t battSum = 0;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * Initialize battery module, including associated GPIO and measuring/setting fault thresholds
 * ADC_Init() MUST be called prior to BATTERY_Init()
 */
void BATT_Init ( void )
{
	// INITIALISE BATTERY VOLTAGE PIN
	GPIO_Init( BATT_Pin, GPIO_Mode_Analog );

	// TAKE INITIAL BATTERY READING TO DETEMINE NUMBER OF CELLS
	uint32_t voltInt = BATT_calcStartup();

	// CALCULATE LOW VOLTAGE FAULT THRESHOLD
	if ( voltInt <= BATT_AUTOH_6S && voltInt > BATT_AUTOH_5S ) {
		faultVolt_set = FAULT_SET_6S;
		faultVolt_reset = FAULT_RESET_6S;
	}
	else if ( voltInt <= BATT_AUTOH_5S && voltInt > BATT_AUTOH_4S ) {
		faultVolt_set = FAULT_SET_5S;
		faultVolt_reset = FAULT_RESET_5S;
	}
	else if ( voltInt <= BATT_AUTOH_4S && voltInt > BATT_AUTOH_3S ) {
		faultVolt_set = FAULT_SET_4S;
		faultVolt_reset = FAULT_RESET_4S;
	}
	else if ( voltInt <= BATT_AUTOH_3S && voltInt > BATT_AUTOH_2S ) {
		faultVolt_set = FAULT_SET_3S;
		faultVolt_reset = FAULT_RESET_3S;
	}
	else {
		faultVolt_set = FAULT_SET_2S;
		faultVolt_reset = FAULT_RESET_2S;
	}

	// SET RELEVANT FLAGS
	faultVolt = false;
}


/*
 * De-initialsie battery module
 */
void BATT_Deinit ( void )
{
	// DEINITIALISE BATTERY VOLTAGE DETECT PIN
	GPIO_Deinit( BATT_Pin );
	// RESET RELEVANT FLAGS
	faultVolt = false;
}


/*
 * Handle battery voltage readings and fault state checks
 */
void BATT_Update ( void )
{
	// FUNCTION VARIABLES
	static bool intFault = false;
	static bool setTick = false;
	static uint32_t tick = 0;

	uint32_t now = CORE_GetTick();
	uint32_t volt = BATT_calc();

	// NORMAL OPERATION
	if ( !faultVolt && !intFault )
	{
		// CHECK FOR UNDERVOLTAGE CONDITION
		if ( volt <= faultVolt_set ) {
			// TRIGGER AND RECORD FAULT CONDITION
			intFault = true;
			tick = now;
			setTick = true;
		}
	}

	// POTENTIAL FAULT FLAGGED
	else if ( intFault )
	{
		// CHECK FOR VOLTAGE RECOVERY
		if ( volt >= faultVolt_reset ) {
			// RESET TO NORMAL OPERATION
			intFault = false;
			setTick = false;
		}
		// STILL IN UNDERVOLT CONDITION
		else {
			// HAS FAULT TIMER ELAPSED
			if ( (now - tick) >= FAULT_SET_MS ) {
				// ESCALLATE TO FULL FAULT CONDITION
				intFault = false;
				faultVolt = true;
				setTick = false;
			}
		}
	}

	// FULL FAULT CONDITION
	else
	{
		// CHECK IF VOLTAGE HAS RECOVERED
		if ( volt >= faultVolt_reset ) {
			// FIRST LOOP SINCE RECOVERY
			if ( !setTick ) {
				tick = now;
				setTick = true;
			}
			// CHECK IF RECOVERED FOR LONG ENOUGH
			else if ( ( now - tick ) >= FAULT_RESET_MS ) {
				// RESET FAULT FLAGS
				faultVolt = false;
				setTick = false;
			}
		}
		// STILL IN UNDERVOLD CONDITION
		else {
			/// RESET RECOVERY FLAG
			if ( setTick ) {
				setTick = false;
			}
		}
	}
}


/*
 * CALLED TO GET CURRENT FAULT STATE OF MODULE
 */
bool BATT_inFaultState ( void )
{
	return faultVolt;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE FUNCTIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * Handle startup condition to prime bettery data array prior to normal operation
 */
static uint32_t BATT_calcStartup ( void )
{
	// ADD VOLT DATA TO ENTIRE ARRAY
	for ( uint8_t i = 0; i < BATT_ARRAY_LEN; i++ )
	{
		// READ AND CALC VOLTAGE
		battArray[i] = AIN_AinToDivider( ADC_Read( BATT_Ch ), BATT_Rlow, BATT_Rhigh );
		battSum += battArray[i];
		// DELAY TO ALLOW VOLTAGE TO STABILISE
		CORE_Idle();
	}

	// CALC AND RETURN VOLTAGE
	return (battSum / BATT_ARRAY_LEN);
}



/*
 * Handle normal operation for the battery voltage measurement and running average filter
 */
static uint32_t BATT_calc ( void )
{
	// FUNCTION VARIABLES
	static uint8_t index = 0;

	// CALCULATE NEW BATT VOLTAGE SUM
	battSum -= battArray[index];
	battArray[index] = AIN_AinToDivider( ADC_Read( BATT_Ch ), BATT_Rlow, BATT_Rhigh );
	battSum += battArray[index];

	// INCREMENET INDEX AND CHECK FOR RESET CONDITION
	if ( index >= (BATT_ARRAY_LEN - 1) ) {
		index = 0;
	} else {
		index += 1;
	}

	// CALC AND RETURN VOLTAGE
	return (battSum / BATT_ARRAY_LEN);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EVENT HANDLERS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* INTERRUPT ROUTINES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
