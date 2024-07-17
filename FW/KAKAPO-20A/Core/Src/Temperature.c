
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Temperature.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#define FAULT_SET_oC	100
#define FAULT_RESET_oC 	90

#define FAULT_SET_MS	10
#define FAULT_RESET_MS	100

#define TEMP_ARRAY_LEN	3

#define TEMP_MIN(X, Y) 	(((X) < (Y)) ? (X) : (Y))
#define TEMP_MAX(X, Y) 	(((X) > (Y)) ? (X) : (Y))


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE TYPES										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE PROTOTYPES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static void 	TEMP_calcStartup ( void );
static uint32_t TEMP_calc 		( void );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE VARIABLES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static bool 	faultTemp = false;

static uint32_t tempArray[ TEMP_ARRAY_LEN ] = {0};
static uint32_t tempSum = 0;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * Initialize temperature module
 * ADC_Init() MUST be called prior to TEMP_Init()
 */
void TEMP_Init ( void )
{
	// TAKE INITIAL TEMPERATURE READING
	TEMP_calcStartup();

	// SET RELEVANT FLAGS
	if ( (tempSum / TEMP_ARRAY_LEN) >= FAULT_SET_oC ) {
		faultTemp = true;
	} else {
		faultTemp = false;
	}
}


/*
 * De-initialize temperature module
 */
void TEMP_Deinit ( void )
{
	// RESET RELEVANT FLAGS
	faultTemp = false;
}


/*
 * Handle temperature measurements and fault state readings/logic
 */
void TEMP_Update ( void )
{
	// FUNCTION VARIABLES
	static bool intFault = false;
	static bool setTick = false;
	static uint32_t tick = 0;

	uint32_t now = CORE_GetTick();
	uint32_t temp = TEMP_calc();

	// NORMAL OPERATION
	if ( !faultTemp && !intFault)
	{
		// CHECK FOR OVERTEMPERATURE CONDITION
		if ( temp >= FAULT_SET_oC ) {
			// TRIGGER AND RECORD FAULT CONDITION
			intFault = true;
			tick = now;
			setTick = true;
		}
	}

	// POTENTIAL FAULT FLAGGED
	else if ( intFault )
	{
		// CHECK FOR TEMP RECOVERY
		if ( temp <= FAULT_RESET_oC ) {
			// RESET TO NORMAL OPERATION
			intFault = false;
			setTick = false;
		}
		// STILL IN OVERTEMP CONDITION
		else {
			// HAS FAULT TIMER ELAPSED
			if ( (now - tick) >= FAULT_SET_MS ) {
				// ESCALLATE TO FULL FAULT CONDITION
				intFault = false;
				faultTemp = true;
				setTick = false;
			}
		}
	}

	// FULL FAULT CONDITION
	else
	{
		// CHECK FOR TEMP RECOVERY
		if ( temp <= FAULT_RESET_oC ) {
			// FIRST LOOP SINCE RECOVERY
			if ( !setTick ) {
				tick = now;
				setTick = true;
			}
			// CHECK IF RECOVERED FOR LONG ENOUGH
			if ( ( now - tick ) >= FAULT_RESET_MS ) {
				// RESET FAULT FLAGS
				faultTemp = false;
				setTick = false;
			}
		}
		// STILL IN OVERTEMP CONDITION
		else {
			// RESET RECOVERY FLAG
			if ( setTick ) {
				setTick = false;
			}
		}
	}
}


/*
 * Retrieves the current state of the temperature fault
 */
bool TEMP_inFaultState ( void )
{
	return faultTemp;
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE FUNCTIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/*
 * Handle startup condition to prime temperature data array prior to normal operation
 */
static void TEMP_calcStartup ( void )
{
	// ADD TEMP DATA TO ENTIRE ARRAY
	for ( uint8_t i = 0; i < TEMP_ARRAY_LEN; i++ )
	{
		// READ AND CALC TEMP
		tempArray[i] = ADC_ReadDieTemp();
		tempSum += tempArray[i];
		// DELAY TO ALLOW TEMP TO STABILISE
		CORE_Idle();
	}
}


/*
 * Handle normal operation for the temperature measurement and running average filter
 */
static uint32_t TEMP_calc ( void )
{
	// FUNCTION VARIABLES
	static uint8_t index = 0;

	// CALCULATE NEW TEMP SUM
	tempSum -= tempArray[index];
	tempArray[index] = ADC_ReadDieTemp();
	tempSum += tempArray[index];

	// INCREMENET INDEX AND CHECK FOR RESET CONDITION
	if ( index >= (TEMP_ARRAY_LEN - 1) ) {
		index = 0;
	} else {
		index += 1;
	}

	// CALC AND RETURN TEMP
	return (tempSum / TEMP_ARRAY_LEN);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EVENT HANDLERS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* INTERRUPT ROUTINES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
