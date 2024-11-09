
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Servo.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE TYPES										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE PROTOTYPES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static uint32_t	SERVO_ReverseRadio 		( uint32_t );
static void 	SERVO_InitS1			( void );
static void 	SERVO_DeinitS1			( void );
static void 	SERVO_InitS2			( void );
static void 	SERVO_DeinitS2			( void );

static void		SERVO_S1_TimerReloadISR	( void );
static void 	SERVO_S1_TimerPulseISR	( void );

#if SERVO_NUM_OUTPUTS >= 2
static void 	SERVO_S2_TimerReloadISR	( void );
static void 	SERVO_S2_TimerPulseISR	( void );
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE VARIABLES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static CONFIG_data 	config 	= {0};
static RADIO_Data* 	dataPtr	= 0;
static bool initS1 = false;
#if SERVO_NUM_OUTPUTS >= 2
static bool initS2 = false;
#endif

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void SERVO_Init(void)
{
	// RETRIEVE CONFIG FROM FLASH
	SERVO_getConfig();

	// RETRIEVE POINTER TO RADIO DATA
	dataPtr = RADIO_getDataPtr();

	//
	if ( config.servoS1_en ) {
		// ENABLE SERVO 1 OUTPUT
		SERVO_InitS1();
	}

	// ENABLE SERVO 2 OUTPUT
#if SERVO_NUM_OUTPUTS >= 2
	if ( config.servoS2_en ) {
		SERVO_InitS2();
	}
#endif
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void SERVO_Deinit(void)
{
	//
	SERVO_DeinitS1();

	//
#if SERVO_NUM_OUTPUTS >= 2
	SERVO_DeinitS2();
#endif
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void SERVO_Update ( void )
{
	//
	bool faultS1 = ( TEMP_inFaultState() || BATT_inFaultState() || dataPtr->inputLostCh[ config.servoS1_ch ] );
	static uint32_t servoS1_p;
	#if SERVO_NUM_OUTPUTS >= 2
	bool faultS2 = ( TEMP_inFaultState() || BATT_inFaultState() || dataPtr->inputLostCh[ config.servoS2_ch ] );
	static uint32_t servoS2_p;
	#endif

	// FAULT CONDITION - RISING EDGE
	if ( ( faultS1 || !config.servoS1_en ) && initS1 )
	{
		SERVO_DeinitS1();
	}

	// FAULT CONDITION - FALLING EDGE
	else if ( !faultS1 && config.servoS1_en && !initS1 )
	{
		SERVO_InitS1();
	}

	// NORMAL OPERATION - NO FAULT
	else if ( !faultS1 && initS1 )
	{
		// EXTRACT APPROPRIATE DATA FOR SERVO
		uint32_t servoS1 = dataPtr->ch[ config.servoS1_ch ];

		// CHECK FOR AND SERVICE CHANNEL REVERSE
		if ( config.servoS1_rev ) { servoS1 = SERVO_ReverseRadio( servoS1 ); }
		//
		if ( servoS1 >= (servoS1_p + 0) || servoS1 <= (servoS1_p - 0) )
		{
			// UPDATE SERVO
			TIM_SetPulse( TIM_SERVO_S1, 0, servoS1 );
			//
			servoS1_p = servoS1;
		}
	}


	#if SERVO_NUM_OUTPUTS >= 2
	if ( ( faultS2 || !config.servoS2_en ) && initS2 ) {
		SERVO_DeinitS2();
	} else if ( !faultS2 && config.servoS2_en && !initS2 ) {
		SERVO_InitS2();
	} else if ( !faultS2 && initS2 ) {
		uint32_t servoS2 = dataPtr->ch[ config.servoS2_ch ];
		if ( config.servoS2_rev ) {
			servoS2 = SERVO_ReverseRadio( servoS2 );
		}
		if ( servoS2 >= (servoS2_p + 0) || servoS2 <= (servoS2_p - 0) ) {
			TIM_SetPulse( TIM_SERVO_S2, 0, servoS2 );
			servoS2_p = servoS2;
		}
	}
	#endif
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void SERVO_getConfig ( void )
{
	FPROM_Read( CONFIG_FPROM_OFFSET, &config, sizeof(config) );
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE FUNCTIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static uint32_t SERVO_ReverseRadio ( uint32_t radio )
{
	return ( (SERVO_CH_MAX + SERVO_CH_MIN) - radio );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void SERVO_InitS1(void)
{
	// ENABLE SERVO 1 OUTPUT
	GPIO_EnableOutput(	SERVO_S1_Pin, GPIO_PIN_RESET );
	TIM_Init(			TIM_SERVO_S1, TIM_SERVO_S1_FREQ, TIM_SERVO_S1_RELOAD );
	TIM_OnReload(		TIM_SERVO_S1, SERVO_S1_TimerReloadISR );
	TIM_OnPulse(		TIM_SERVO_S1, 0, SERVO_S1_TimerPulseISR );
	TIM_SetPulse( 		TIM_SERVO_S1, 0, 0 );
	TIM_Start(			TIM_SERVO_S1 );
	//
	initS1 = true;
}

void SERVO_DeinitS1(void)
{
	//
	TIM_Stop(	TIM_SERVO_S1 );
	TIM_Deinit(	TIM_SERVO_S1 );
	GPIO_Write(	SERVO_S1_Pin, GPIO_PIN_RESET );
	//
	initS1 = false;
}

/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
#if SERVO_NUM_OUTPUTS >= 2
void SERVO_InitS2(void)
{
	GPIO_EnableOutput(	SERVO_S2_Pin, GPIO_PIN_RESET );
	TIM_Init(			TIM_SERVO_S2, TIM_SERVO_S2_FREQ, TIM_SERVO_S2_RELOAD );
	TIM_OnReload(		TIM_SERVO_S2, SERVO_S2_TimerReloadISR );
	TIM_OnPulse(		TIM_SERVO_S2, 0, SERVO_S2_TimerPulseISR );
	TIM_SetPulse( 		TIM_SERVO_S2, 0, 0 );
	TIM_Start(			TIM_SERVO_S2 );
	initS2 = true;
}

void SERVO_DeinitS2(void)
{
	TIM_Stop(	TIM_SERVO_S2 );
	TIM_Deinit(	TIM_SERVO_S2 );
	GPIO_Write(	SERVO_S2_Pin, GPIO_PIN_RESET );
	initS2 = false;
}
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EVENT HANDLERS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* INTERRUPT ROUTINES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static void SERVO_S1_TimerReloadISR ( void )
{
	GPIO_Set( SERVO_S1_Pin );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static void SERVO_S1_TimerPulseISR ( void )
{
	GPIO_Reset( SERVO_S1_Pin );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
#if SERVO_NUM_OUTPUTS >= 2
static void SERVO_S2_TimerReloadISR ( void )
{
	GPIO_Set( SERVO_S2_Pin );
}
#endif


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
#if SERVO_NUM_OUTPUTS >= 2
static void SERVO_S2_TimerPulseISR ( void )
{
	GPIO_Reset( SERVO_S2_Pin );
}
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
