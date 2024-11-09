
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Motor.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define PULSE_WIDTH_CENTER_DEADBAND	50	// Deadzone around Center
#define PULSE_WIDTH_CENTER_US		MOTOR_CENTER
#define PULSE_WIDTH_HCENTER_US		( PULSE_WIDTH_CENTER_US + PULSE_WIDTH_CENTER_DEADBAND )
#define PULSE_WIDTH_LCENTER_US		( PULSE_WIDTH_CENTER_US - PULSE_WIDTH_CENTER_DEADBAND )

#define PULSE_WIDTH_FULLON			25
#define PULSE_WIDTH_ERROR_US		RADIO_CH_ERROR
#define PULSE_WIDTH_MIN_US			MOTOR_MIN
#define PULSE_WIDTH_MMIN_US			( PULSE_WIDTH_MIN_US + PULSE_WIDTH_FULLON )
#define PULSE_WIDTH_ABSMIN_US		( PULSE_WIDTH_MIN_US - PULSE_WIDTH_ERROR_US )
#define PULSE_WIDTH_MAX_US			MOTOR_MAX
#define PULSE_WIDTH_MMAX_US			( PULSE_WIDTH_MAX_US - PULSE_WIDTH_FULLON )
#define PULSE_WIDTH_ABSMAX_US		( PULSE_WIDTH_MAX_US + PULSE_WIDTH_ERROR_US )

#define SPEED_OFF					0
#define SPEED_MAX					MOTOR_HALFSCALE

#define SPEED_RAMP					2

#define TX_BUFF_SIZE				2


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE TYPES										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


typedef struct {
	uint32_t speed;
	uint32_t speed_p;
	bool dir;
	bool dir_p;
} MOTOR_chStats;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE PROTOTYPES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static uint32_t MOTOR_ReverseRadio 	( uint32_t );
static uint32_t	MOTOR_Truncate		( uint32_t );
static void		MOTOR_Process		( uint32_t, MOTOR_chStats* );

void 			MOTOR_ReloadM1_ISR 	( void );
void 			MOTOR_PulseM1_ISR 	( void );
void 			MOTOR_ReloadM2_ISR 	( void );
void 			MOTOR_PulseM2_ISR 	( void );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE VARIABLES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static volatile uint8_t	tx[TX_BUFF_SIZE] 			= {0};
static uint8_t 			tx_resetFault[TX_BUFF_SIZE]	= {0x08, 0x92};
static uint8_t 			tx_driveOFF[TX_BUFF_SIZE] 	= {0x09, 0x00};
static uint8_t 			tx_driveFWD[TX_BUFF_SIZE] 	= {0x09, 0x03};
static uint8_t 			tx_driveRVS[TX_BUFF_SIZE] 	= {0x09, 0x02};

static CONFIG_data 		config 						= {0};
static RADIO_Data* 		dataPtr 					= 0;

static MOTOR_chStats 	M1 = {
						.speed = 0,
						.speed_p = 0,
						.dir = false,
						.dir_p = false, };

static MOTOR_chStats 	M2 = {
						.speed = 0,
						.speed_p = 0,
						.dir = false,
						.dir_p = false, };


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_Init ( void )
{
	// FUNCTION VARAIBLES
	uint8_t tx[2] = {0};

	// ENABLE SPI MODULE FUNCTIONALITY
	SPI_Init( SPI_1, 16000000, SPI_Mode_1 );

	// ENABLE GPIO FOR MOTOR A
	GPIO_EnableOutput(	MOTORA_Select,	GPIO_PIN_SET );
	GPIO_Init( 			MOTORA_Sense, 	GPIO_Mode_Analog );
	GPIO_EnableInput(	MOTORA_Fault, 	GPIO_Pull_Up);

	// ENABLE GPIO FOR MOTOR B
	GPIO_EnableOutput(	MOTORB_Select,	GPIO_PIN_SET );
	GPIO_Init( 			MOTORB_Sense, 	GPIO_Mode_Analog );
	GPIO_EnableInput(	MOTORB_Fault, 	GPIO_Pull_Up );

	// RETRIEVE CONFIG FROM FLASH
	FPROM_Read( CONFIG_FPROM_OFFSET, &config, sizeof(config) );

	// RETRIEVE POINTER TO RADIO DATA
	dataPtr = RADIO_getDataPtr();

	// WRITE COMMAND REGISTER
	//	- Reset nFAULT Pin From Startup
	// 	- SET SPI_IN_LOCK
	// 	- RESET REG_LOCK
	tx[0] = 0x08;							// address = 0x08
	tx[1] = 0x80;							// default = 0x09
	GPIO_Reset( MOTORA_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORA_Select );
	GPIO_Reset( MOTORB_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORB_Select );

	// TODO: Actually implement EN_OLA
	// TODO: Actually implement VMOV_SEL

	// WRITE CONFIG1 REGISTER
	// 	- Set EN_OLA -- NOT YET IMPLEMENTED
	//	- Set VMOV_SEL -- NOT YET IMPLEMENTED
	//	- Set OCP_RETRY
	//	- Set TSD_RETRY
	//	- Set VMOV_RETRY (Includes VMUV Retry)
	// 	- Set OLA_RETRY
	tx[0] = 0x0A;							// address = 0x0A
	tx[1] = 0x10; 							// default = 0x10
	GPIO_Reset( MOTORA_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORA_Select );
	GPIO_Reset( MOTORB_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORB_Select );

	// TODO: Actually implement S_DIAG
	// TODO: Actually implement S_TRIP. Implement Check here because this isnt notified in FAULT Register

	// WRITE CONFIG2 REGISTER
	//	- Set S_DIAG
	//	- Set S_ITRIP
	tx[0] = 0x0B;							// address = 0x0B
	tx[1] = 0x00; 							// default = 0x00
	GPIO_Reset( MOTORA_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORA_Select );
	GPIO_Reset( MOTORB_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORB_Select );

	// WRITE CONFIG3 REGISTER
	//	- Set S_MODE as PH/EN mode
	tx[0] = 0x0C;							// address = 0x0C
	tx[1] = 0x40; 							// default = 0x40
	GPIO_Reset( MOTORA_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORA_Select );
	GPIO_Reset( MOTORB_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORB_Select );

	// WRITE CONFIG4 REGISTER
	//	- Set DRVOFF_SEL, EN_IN_SEL, and PH_IN2_SEL registers as logical OR with I/P
	tx[0] = 0x0D;							// address = 0x0D
	tx[1] = 0x00; 							// default = 0x04
	GPIO_Reset( MOTORA_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORA_Select );
	GPIO_Reset( MOTORB_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORB_Select );

	// WRITE COMMAND REGISTER
	// 	- RESET SPI_IN_LOCK
	// 	- SET REG_LOCK
	tx[0] = 0x08;							// address = 0x08
	tx[1] = 0x12;							// default = 0x09
	GPIO_Reset( MOTORA_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORA_Select );
	GPIO_Reset( MOTORB_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORB_Select );

	// WRITE TO SPI_IN REGISTRER
	// 	- SPEED TO 0
	tx[0] = 0x09;							// address = 0x09
	tx[1] = 0x00;							// default = 0x00
	GPIO_Reset( MOTORA_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORA_Select );
	GPIO_Reset( MOTORB_Select );
	SPI_Write( SPI_1, tx, TX_BUFF_SIZE );
	GPIO_Set( MOTORB_Select );

	// START TIMER TO ...
	TIM_Init( 		TIM_MOTORA, TIM_MOTORA_FREQ, TIM_MOTORA_RELOAD );
	TIM_OnReload( 	TIM_MOTORA, MOTOR_ReloadM1_ISR );
	TIM_OnPulse( 	TIM_MOTORA, TIM_MOTORA_CH, MOTOR_PulseM1_ISR );
	TIM_SetPulse(	TIM_MOTORA, TIM_MOTORA_CH, M1.speed );
	TIM_Start(		TIM_MOTORA );
	TIM_Init( 		TIM_MOTORB, TIM_MOTORB_FREQ, TIM_MOTORB_RELOAD );
	TIM_OnReload( 	TIM_MOTORB, MOTOR_ReloadM2_ISR );
	TIM_OnPulse( 	TIM_MOTORB, TIM_MOTORB_CH, MOTOR_PulseM2_ISR );
	TIM_SetPulse(	TIM_MOTORB, TIM_MOTORB_CH, M2.speed );
	TIM_Start(		TIM_MOTORB );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_Deinit ( void )
{

}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_Update ( void )
{
	uint32_t driveA;
	uint32_t driveB;

	//
	if ( TEMP_inFaultState() || BATT_inFaultState() || dataPtr->inputLostCh[ config.motorA_ch ] || dataPtr->inputLostCh[ config.motorB_ch ]  )
	{
		driveA = PULSE_WIDTH_CENTER_US;
		driveB = PULSE_WIDTH_CENTER_US;
	}

	//
	else
	{
		// Extract appropriate data for motors
		driveA = MOTOR_Truncate( dataPtr->ch[ config.motorA_ch ] );
		driveB = MOTOR_Truncate( dataPtr->ch[ config.motorB_ch ] );

		// Check For Valid Motor Data
		if ( !driveA || !driveB )
		{
			driveA = PULSE_WIDTH_CENTER_US;
			driveB = PULSE_WIDTH_CENTER_US;
		}
		else
		{
			// Check for channel reverse
			if ( config.motorA_rev ) { driveA = MOTOR_ReverseRadio( driveA ); }
			if ( config.motorB_rev ) { driveB = MOTOR_ReverseRadio( driveB ); }

			// Mix channels for Arcade drive mode
			if (config.mode == CONFIG_ARCADE)
			{
				// Driving Straight
				if ( driveB == PULSE_WIDTH_CENTER_US ) {
					driveB = driveA;
				}
				// Driving Left
				else if ( driveB < PULSE_WIDTH_CENTER_US ) {
					uint32_t steer = PULSE_WIDTH_CENTER_US - driveB;
					driveB = driveA + steer;
					driveA = driveA - steer;
				}
				// Driving Right
				else if ( driveB > PULSE_WIDTH_CENTER_US ) {
					uint32_t steer = driveB - PULSE_WIDTH_CENTER_US;
					driveB = driveA - steer;
					driveA = driveA + steer;
				}
			}

			driveA = MOTOR_Truncate( driveA );
			driveB = MOTOR_Truncate( driveB );
		}
	}

	//
	MOTOR_Process( driveA, &M1 );
	MOTOR_Process( driveB, &M2 );

	//
	TIM_SetPulse( TIM_MOTORA, TIM_MOTORA_CH, M1.speed );
	TIM_SetPulse( TIM_MOTORB, TIM_MOTORB_CH, M2.speed );

	//
	M1.speed_p = M1.speed;
	M2.speed_p = M2.speed;
	M1.dir_p = M1.dir;
	M2.dir_p = M2.dir;
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_getConfig ( void )
{
	FPROM_Read( CONFIG_FPROM_OFFSET, &config, sizeof(config) );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
bool MOTOR_M1_isActive ( void )
{
	return M1.speed;
}


 /*
  * TEXT
  *
  * INPUTS:
  * OUTPUTS:
  */
bool MOTOR_M2_isActive ( void )
{
	return M2.speed;
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_StartupBuzz ( void )
{

}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_calibASSERT ( void )
{
	MOTOR_calibOFF();
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_calibRELEASE( void )
{
	MOTOR_calibOFF();
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_calibOFF ( void )
{
	M1.speed = SPEED_OFF;
	M2.speed = SPEED_OFF;

	TIM_SetPulse( TIM_MOTORA, TIM_MOTORA_CH, M1.speed );
	TIM_SetPulse( TIM_MOTORB, TIM_MOTORB_CH, M2.speed );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_calibTwitchFWD ( uint32_t delay )
{
	M1.dir = true;
	M2.dir = true;

	M1.speed = ( SPEED_MAX/4 );
	M2.speed = ( SPEED_MAX/4 );

	TIM_SetPulse( TIM_MOTORA, TIM_MOTORA_CH, M1.speed );
	TIM_SetPulse( TIM_MOTORB, TIM_MOTORB_CH, M2.speed );

	CORE_Delay(delay);

	M1.speed = SPEED_OFF;
	M2.speed = SPEED_OFF;

	TIM_SetPulse( TIM_MOTORA, TIM_MOTORA_CH, M1.speed );
	TIM_SetPulse( TIM_MOTORB, TIM_MOTORB_CH, M2.speed );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_calibTwitchLEFT ( uint32_t delay )
{
	M1.dir = false;
	M2.dir = true;

	M1.speed = ( SPEED_MAX/4 );
	M2.speed = ( SPEED_MAX/4 );

	TIM_SetPulse( TIM_MOTORA, TIM_MOTORA_CH, M1.speed );
	TIM_SetPulse( TIM_MOTORB, TIM_MOTORB_CH, M2.speed );

	CORE_Delay(delay);

	M1.speed = SPEED_OFF;
	M2.speed = SPEED_OFF;

	TIM_SetPulse( TIM_MOTORA, TIM_MOTORA_CH, M1.speed );
	TIM_SetPulse( TIM_MOTORB, TIM_MOTORB_CH, M2.speed );
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
static uint32_t MOTOR_ReverseRadio ( uint32_t radio )
{
	return ((PULSE_WIDTH_MIN_US + PULSE_WIDTH_MAX_US) - radio);
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static uint32_t MOTOR_Truncate ( uint32_t input )
{
	if ( input == 0 ) {
		return 0;
	} else if ( input < PULSE_WIDTH_MMIN_US ) {
		return PULSE_WIDTH_MIN_US;
	} else if ( input < PULSE_WIDTH_LCENTER_US ) {
		return input;
	} else if ( input <= PULSE_WIDTH_HCENTER_US ) {
		return PULSE_WIDTH_CENTER_US;
	} else if ( input <= PULSE_WIDTH_MMAX_US ) {
		return input;
	} else {
		return PULSE_WIDTH_MAX_US;
	}
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static void MOTOR_Process ( uint32_t input, MOTOR_chStats* m )
{
	uint32_t radio = 0;
	uint32_t speedInt = 0;
	bool directionInt = false;

	if ( input == 0 ) {
		radio = 0;
	} else if ( input < PULSE_WIDTH_ABSMIN_US ) {
		radio = 0;
	} else if ( input < PULSE_WIDTH_MMIN_US ) {
		radio = PULSE_WIDTH_MIN_US;
	} else if ( input < PULSE_WIDTH_LCENTER_US ) {
		radio = input;
	} else if ( input <= PULSE_WIDTH_HCENTER_US ) {
		radio = PULSE_WIDTH_CENTER_US;
	} else if ( input <= PULSE_WIDTH_MMAX_US ) {
		radio = input;
	} else if ( input <= PULSE_WIDTH_ABSMAX_US ) {
		radio = PULSE_WIDTH_MAX_US;
	} else {
		radio = 0;
	}

	if ( radio > PULSE_WIDTH_CENTER_US )
	{
		speedInt = radio - PULSE_WIDTH_CENTER_US;
		directionInt = true;
	}
	else if ( radio < PULSE_WIDTH_CENTER_US )
	{
		speedInt = PULSE_WIDTH_CENTER_US - radio;
		directionInt = false;
	}
	else
	{
		speedInt = SPEED_OFF;
	}

	// Ignore Ramp in Immediate Cutout for Safety
	if ( speedInt == SPEED_OFF ) {
		// Do nothing
	}
	// Going Same Direction to Prev Loop
	else if ( directionInt == m->dir_p  ) {
		// Accelerating
		if ( speedInt > m->speed_p  ) {
			// Exceed Limit
			if ( (speedInt - m->speed_p ) > SPEED_RAMP ) {
				speedInt = m->speed_p  + SPEED_RAMP;
			}
		}
		// Decelerating
		else if ( speedInt < m->speed_p  ) {
			if ( (m->speed_p  - speedInt) > SPEED_RAMP ) {
				speedInt = m->speed_p  - SPEED_RAMP;
			}
		}
	}
	// Going Different Direction To Prev Loop
	else {
		// Exceeding Limit
		if ( (speedInt + m->speed_p ) > SPEED_RAMP ) {
			//
			if ( m->speed_p  == SPEED_RAMP ) {
				speedInt = 0;
			}
			else if ( m->speed_p  > SPEED_RAMP ) {
				speedInt = m->speed_p  - SPEED_RAMP;
				directionInt = m->dir_p ;
			}
			else {
				speedInt = SPEED_RAMP - m->speed_p ;
			}
		}

	}

	m->speed = speedInt;
	m->dir = directionInt;
}


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
void MOTOR_ReloadM1_ISR ( void )
{
	//
	GPIO_Reset( MOTORA_Select );
	//
	if ( !GPIO_Read( MOTORA_Fault ) )
	{
		SPI_Write( SPI_1, (uint8_t*)tx_resetFault, TX_BUFF_SIZE );
	}
	//
	else if ( M1.speed == SPEED_OFF )
	{
		SPI_Write( SPI_1, (uint8_t*)tx_driveOFF, TX_BUFF_SIZE );
	}
	//
	else  // M1.speed != SPEED_OFF
	{
		if ( M1.dir ) {
			SPI_Write( SPI_1, (uint8_t*)tx_driveFWD, TX_BUFF_SIZE );
		} else {
			SPI_Write( SPI_1, (uint8_t*)tx_driveRVS, TX_BUFF_SIZE );
		}
	}
	//
	GPIO_Set( MOTORA_Select );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_PulseM1_ISR ( void )
{
	//
	GPIO_Reset( MOTORA_Select );
	//
	if ( !GPIO_Read( MOTORA_Fault ) )
	{
		SPI_Write( SPI_1, (uint8_t*)tx_resetFault, TX_BUFF_SIZE );
	}
	//
	else if ( M1.speed != SPEED_MAX )
	{
		SPI_Write( SPI_1, (uint8_t*)tx_driveOFF, TX_BUFF_SIZE );
	}
	//
	else // M1.speed == SPEED_MAX
	{
		if ( M1.dir ) {
			SPI_Write( SPI_1, (uint8_t*)tx_driveFWD, TX_BUFF_SIZE );
		} else {
			SPI_Write( SPI_1, (uint8_t*)tx_driveRVS, TX_BUFF_SIZE );
		}
	}
	//
	GPIO_Set( MOTORA_Select );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_ReloadM2_ISR ( void )
{
	GPIO_Reset( MOTORB_Select );
	if ( !GPIO_Read( MOTORB_Fault ) ) {
		SPI_Write( SPI_1, (uint8_t*)tx_resetFault, TX_BUFF_SIZE );
	} else if ( M2.speed == SPEED_OFF ) {
		SPI_Write( SPI_1, (uint8_t*)tx_driveOFF, TX_BUFF_SIZE );
	} else {
		if ( M2.dir ) {
			SPI_Write( SPI_1, (uint8_t*)tx_driveFWD, TX_BUFF_SIZE );
		} else {
			SPI_Write( SPI_1, (uint8_t*)tx_driveRVS, TX_BUFF_SIZE );
		}
	}
	GPIO_Set( MOTORB_Select );
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void MOTOR_PulseM2_ISR ( void )
{
	GPIO_Reset( MOTORB_Select );
	if ( !GPIO_Read( MOTORB_Fault ) ) {
		SPI_Write( SPI_1, (uint8_t*)tx_resetFault, TX_BUFF_SIZE );
	} else if ( M2.speed != SPEED_MAX ) {
		SPI_Write( SPI_1, (uint8_t*)tx_driveOFF, TX_BUFF_SIZE );
	} else {
		if ( M2.dir ) {
			SPI_Write( SPI_1, (uint8_t*)tx_driveFWD, TX_BUFF_SIZE );
		} else {
			SPI_Write( SPI_1, (uint8_t*)tx_driveRVS, TX_BUFF_SIZE );
		}
	}
	GPIO_Set( MOTORB_Select );
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
