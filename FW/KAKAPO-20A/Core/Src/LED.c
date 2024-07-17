
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "LED.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define LED_PULSEON_MS 		200
#define LED_PULSEOFF_MS		300

#define FAULT_INPUT_MS		400
#define FAULT_VOLTAGE_MS	400
#define FAULT_TEMP_MS		100


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE TYPES										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE PROTOTYPES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


bool LED_getState_M1 		( void );
bool LED_getState_M2 		( void );

void LED_updateOutput_M1	( bool );
void LED_updateOutput_M2 	( bool );

void LED_toggleOutput_M1 	( void );
void LED_toggleOutput_M2 	( void );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE VARIABLES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * Handles Initialization of the LED Module
 */
void LED_Init (void)
{
	#if defined(LED_PWR_Pin)
	GPIO_EnableOutput(LED_PWR_Pin, LED_ON);
	#endif

	GPIO_EnableOutput(LED_M1_Pin, LED_OFF);
	GPIO_EnableOutput(LED_M2_Pin, LED_OFF);
}


/*
 * Handles De-Initialization of LED Module
 */
void LED_Deinit (void)
{
	GPIO_Deinit(LED_PWR_Pin);
	GPIO_Deinit(LED_M1_Pin);
	GPIO_Deinit(LED_M2_Pin);
}


/*
 * Handle...
 */
void LED_Update ( void )
{
	// INIT FUNCTION VARIABLES
	static uint32_t tick = 0;
	uint32_t now = CORE_GetTick();

	// FAULT CONDITION - OVERTEMPERATURE
	if ( TEMP_inFaultState() )
	{
		if ( (now - tick) >= (2*FAULT_TEMP_MS) ) {
			tick = now;
		}

		if ( (now - tick) >= FAULT_TEMP_MS ) {
			LED_updateOutput_M1( true );
			LED_updateOutput_M2( true );
		} else {
			LED_updateOutput_M1( false );
			LED_updateOutput_M2( false );
		}
	}

	// FAULT CONDITION - UNDERVOLTAGE
	else if ( BATT_inFaultState())
	{
		if ( (now - tick) >= (2*FAULT_VOLTAGE_MS) ) {
			tick = now;
		}

		if ( (now - tick) >= FAULT_VOLTAGE_MS ) {
			LED_updateOutput_M1( true );
			LED_updateOutput_M2( true );
		} else {
			LED_updateOutput_M1( false );
			LED_updateOutput_M2( false );
		}
	}

	// FAULT CONDITION - SIGNAL LOST
	else if ( RADIO_inFaultStateANY() )
	{
		if ( (now - tick) >= (2*FAULT_INPUT_MS) ) {
			tick = now;
		}

		if ( (now - tick) >= FAULT_INPUT_MS ) {
			LED_updateOutput_M1( true );
			LED_updateOutput_M2( false );
		} else {
			LED_updateOutput_M1( false );
			LED_updateOutput_M2( true );
		}
	}

	// NORMAL OPERATION
	else
	{
		if ( MOTOR_M1_isActive() ) {
			LED_updateOutput_M1( true );
		} else {
			LED_updateOutput_M1( false );
		}

		if ( MOTOR_M2_isActive() ) {
			LED_updateOutput_M2( true );
		} else {
			LED_updateOutput_M2( false );
		}
	}
}


/*
 *  Turn Both LED's ON then OFF (Pulse) A Defined Number of Time
 *
 *  n = Number of Pulses
 *  LED_PULSEOFF_MS = Time LED Pulses OFF
 *  LED_PULSEON_MS = Time LED Pulse ON
 */
void LED_pulseOutputs ( uint8_t n )
{
	// Repeat Process n Times
	for ( uint8_t i = 0; i < n; i++ )
	{
		// Turn LED's OFF
		LED_updateOutput_M1( LED_OFF );
		LED_updateOutput_M2( LED_OFF );
		// Wait 'OFF Time'
		CORE_Delay( LED_PULSEOFF_MS );
		// Turn LED's ON
		LED_updateOutput_M1( LED_ON );
		LED_updateOutput_M2( LED_ON );
		// Wait 'ON Time'
		CORE_Delay( LED_PULSEON_MS );
	}
}


/*
 *
 */
void LED_calibUpdate ( bool M1, bool M2 )
{
	LED_updateOutput_M1(M1);
	LED_updateOutput_M2(M2);
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE FUNCTIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * Retrieve the Current State of the M1 LED
 */
bool LED_getState_M1 ( void )
{
	return GPIO_Read( LED_M1_Pin );
}


/*
 * Retrieve the Current State of the M2 LED
 */
bool LED_getState_M2 ( void )
{
	return GPIO_Read( LED_M2_Pin );
}


/*
 * Turn the M1 LED ON/OFF
 */
void LED_updateOutput_M1 ( bool on )
{
	if ( on ) {
		GPIO_Set( LED_M1_Pin );
	} else {
		GPIO_Reset( LED_M1_Pin );
	}
}


/*
 * Turn the M2 LED ON/OFF
 */
void LED_updateOutput_M2 ( bool on )
{
	if ( on ) {
		GPIO_Set( LED_M2_Pin );
	} else {
		GPIO_Reset( LED_M2_Pin );
	}
}


/*
 * Toggle the State of M1 LED
 *
 * Calling the Function Will Turn M1 OFF if it was already on and vice-versa.
 */
void LED_toggleOutput_M1 ( void )
{
	if ( LED_getState_M1() ) {
		LED_updateOutput_M1(false);
	} else {
		LED_updateOutput_M1(true);
	}
}


/*
 * Toggle the State of M2 LED
 *
 * Calling the Function Will Turn M2 OFF if it was already on and vice-versa.
 */
void LED_toggleOutput_M2 ( void )
{
	if ( LED_getState_M2() ) {
		LED_updateOutput_M2(false);
	} else {
		LED_updateOutput_M2(true);
	}
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EVENT HANDLERS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* INTERRUPT ROUTINES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
