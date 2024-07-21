
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "Radio.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define PWM_WIDTH_MIN_US			RADIO_CH_MIN
#define PWM_WIDTH_ABSMIN_US			(PWM_WIDTH_MIN_US - RADIO_CH_ERROR)

#define PWM_WIDTH_MAX_US			RADIO_CH_MAX
#define PWM_WIDTH_ABSMAX_US			(PWM_WIDTH_MAX_US + RADIO_CH_ERROR)

#define PWM_WIDTH_CENTER_DEADBAND	50
#define PWM_WIDTH_CENTER_US			RADIO_CH_CENTER
#define PWM_WIDTH_CENTERMIN_US		(PWM_WIDTH_CENTER_US - PWM_WIDTH_CENTER_DEADBAND)
#define PWM_WIDTH_CENTERMAX_US		(PWM_WIDTH_CENTER_US + PWM_WIDTH_CENTER_DEADBAND)

#define PWM_PERIOD_MS				20
#define PWM_PERIOD_US				(PWM_PERIOD_MS * 1000)
#define PWM_PERIOD_MAX_MS			(PWM_PERIOD_MS + 5)
#define PWM_PERIOD_MAX_US 			(PWM_PERIOD_MAX_MS * 1000)
#define PWM_PERIOD_MIN_MS			(PWM_PERIOD_MS - 5)
#define PWM_PERIOD_MIN_US 			(PWM_PERIOD_MIN_MS * 1000)

#define PWM_TIMEOUT_CYCLES			5
#define PWM_TIMEOUT_MS				(PWM_PERIOD_MS * PWM_TIMEOUT_CYCLES)

#define PWM_TIMEIN_COUNT			3


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE TYPES										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


typedef enum {
	radioCH1,
	radioCH2,
	radioCH3,
	radioCH4,
} RADIO_chArrayIndex;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE PROTOTYPES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


bool 		RADIO_inFaultStateCH1 	( void );
bool 		RADIO_inFaultStateCH2 	( void );
#if RADIO_NUM_CHANNELS >= 3
bool 		RADIO_inFaultStateCH3 	( void );
#if RADIO_NUM_CHANNELS >= 4
bool 		RADIO_inFaultStateCH4 	( void );
#endif
#endif

uint32_t	RADIO_truncate			( uint32_t );
void 		RADIO_processChannel	( uint8_t );

static void RADIO_CH1_IRQ			( void );
static void RADIO_CH2_IRQ			( void );
#if RADIO_NUM_CHANNELS >= 3
static void RADIO_CH3_IRQ			( void );
#if RADIO_NUM_CHANNELS >= 4
static void RADIO_CH4_IRQ			( void );
#endif
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PRIVATE VARIABLES									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


static bool 				initialised = false;
static volatile uint32_t	rx[RADIO_NUM_CHANNELS];
RADIO_Data 					data;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/*
 * HANDLES INITIALISATION OF RADIO MODULE
 * INCLUDES DATA STRUCTURES, TIMERS, AND CALIBRATION
 *
 * INPUTS:
 * OUTPUTS:
 */
void RADIO_Init(void) {
	// RESET RADIO DATA ARRAYS
	for (uint8_t ch = 0; ch < RADIO_NUM_CHANNELS; ch++) {
		rx[ch] = 0;
		data.ch[ch] = 0;
		data.chActive[ch] = chActive_False;
		data.inputLostCh[ch] = true;
	}

	// START TIMERS TO MEASURE PULSE WIDTHS
	TIM_Init(TIM_RADIO, TIM_RADIO_FREQ, TIM_RADIO_RELOAD);
	TIM_Start(TIM_RADIO);

	// SETUP GPIO AS INPUTS
	GPIO_EnableInput(RADIO_PWM1_Pin, GPIO_Pull_Down);
	GPIO_OnChange(RADIO_PWM1_Pin, GPIO_IT_Both, RADIO_CH1_IRQ);
	GPIO_EnableInput(RADIO_PWM2_Pin, GPIO_Pull_Down);
	GPIO_OnChange(RADIO_PWM2_Pin, GPIO_IT_Both, RADIO_CH2_IRQ);
	#if RADIO_NUM_CHANNELS >= 3
	GPIO_EnableInput(RADIO_PWM3_Pin, GPIO_Pull_Down);
	GPIO_OnChange(RADIO_PWM3_Pin, GPIO_IT_Both, RADIO_CH3_IRQ);
	#if RADIO_NUM_CHANNELS >= 4
	GPIO_EnableInput(RADIO_PWM4_Pin, GPIO_Pull_Down);
	GPIO_OnChange(RADIO_PWM4_Pin, GPIO_IT_Both, RADIO_CH4_IRQ);
	#endif
	#endif

	// SET RELEVANT FLAGS
	initialised = true;

	// RUN A RADIO DATA UPDATE BEFORE PROGRESSING
	RADIO_Update();
}


/*
 * UPDATES RELEVANT RADIO DATA
 * SHOULD BE POLLED REGULARLY, RECOMMENDED PERIOD = ~1MS
 *
 * INPUTS:
 * OUTPUTS:
 */
void RADIO_Update(void) {
	// INIT LOOP VARIABLES
	uint32_t now = CORE_GetTick();
	static uint32_t tick[RADIO_NUM_CHANNELS] = { 0 };
	static uint8_t count = 0;
	static uint8_t timeinCount = 2;

	// ITTERATE THROUGH EACH CHANNEL
	for (uint8_t ch = radioCH1; ch < RADIO_NUM_CHANNELS; ch++) {
		// STATE - TIMEDOUT (OR STARTUP)
		if (data.inputLostCh[ch]) {
			// VALID DATA SITTING IN BUFFER
			if (rx[ch]) {
				// INCREMENT VALID DATA COUNTER
				count += 1;
				// IS TIMEIN CONDITION REACHED
				if (count >= timeinCount) {
					data.inputLostCh[ch] = false; // dont do anything to cascade to normal operation
					timeinCount = PWM_TIMEIN_COUNT;
				}
				// RESET AND WAIT FOR MORE DATA
				else {
					rx[ch] = 0;
					tick[ch] = now;
				}
			}
			// CHECK FOR TIMEIN COUNT RESET
			else if (count && (now - tick[ch]) >= PWM_PERIOD_MAX_MS) {
				count = 0;
				tick[ch] = now;
			}
		}

		// STATE - NORMAL OPERATION
		if (!data.inputLostCh[ch]) {
			// CHECK FOR NEW DATA
			if (rx[ch]) {
				// PROCESS DATA
				RADIO_processChannel(ch);
				// RESET RELEVANT FLAGS
				rx[ch] = 0;
				tick[ch] = now;
			}

			// CHECK FOR TIMEOUT CONDITION
			else if ((now - tick[ch]) >= PWM_TIMEOUT_MS) {
				// SET RELEVANT FLAGS
				data.inputLostCh[ch] = true;
				data.ch[ch] = 0;
				data.chActive[ch] = chActive_False;
				tick[ch] = now;
				count = 0;
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
RADIO_Data* RADIO_getDataPtr ( void )
{
	return &data;
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
bool RADIO_inFaultStateFULL ( void )
{
#if RADIO_NUM_CHANNELS >= 4
	if ( RADIO_inFaultStateCH1() && RADIO_inFaultStateCH2() && RADIO_inFaultStateCH3() && RADIO_inFaultStateCH4() ) {
#elif RADIO_NUM_CHANNELS >= 3
	if ( RADIO_inFaultStateCH1() && RADIO_inFaultStateCH2() && RADIO_inFaultStateCH3() ) {
#else
	if ( RADIO_inFaultStateCH1() && RADIO_inFaultStateCH2() ) {
#endif
		return true;
	} else {
		return false;
	}
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
bool RADIO_inFaultStateANY ( void )
{
#if RADIO_NUM_CHANNELS >= 4
	if ( RADIO_inFaultStateCH1() || RADIO_inFaultStateCH2() || RADIO_inFaultStateCH3() || RADIO_inFaultStateCH4() ) {
#elif RADIO_NUM_CHANNELS >= 3
	if ( RADIO_inFaultStateCH1() || RADIO_inFaultStateCH2() || RADIO_inFaultStateCH3() ) {
#else
	if ( RADIO_inFaultStateCH1() || RADIO_inFaultStateCH2() ) {
#endif
		return true;
	} else {
		return false;
	}
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
bool RADIO_inFaultStateCH1 ( void )
{
	if ( !initialised || (initialised && data.inputLostCh[radioCH1]) ) {
		return true;
	} else {
		return false;
	}
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
bool RADIO_inFaultStateCH2 ( void )
{
	if ( !initialised || (initialised && data.inputLostCh[radioCH2]) ) {
		return true;
	} else {
		return false;
	}
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
#if RADIO_NUM_CHANNELS >= 3
bool RADIO_inFaultStateCH3 ( void )
{
	if ( !initialised || (initialised && data.inputLostCh[radioCH2]) ) {
		return true;
	} else {
		return false;
	}
}
#endif


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
#if RADIO_NUM_CHANNELS >= 4
bool RADIO_inFaultStateCH4 ( void )
{
	if ( !initialised || (initialised && data.inputLostCh[radioCH2]) ) {
		return true;
	} else {
		return false;
	}
}
#endif


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
uint32_t RADIO_truncate(uint32_t r) {
	if (r == 0) {
		return 0;
	} else if (r < PWM_WIDTH_ABSMIN_US) {
		return 0;
	} else if (r < PWM_WIDTH_MIN_US) {
		return PWM_WIDTH_MIN_US;
	} else if (r < PWM_WIDTH_CENTERMIN_US) {
		return r;
	} else if (r <= PWM_WIDTH_CENTERMAX_US) {
		return PWM_WIDTH_CENTER_US;
	} else if (r <= PWM_WIDTH_MAX_US) {
		return r;
	} else if (r <= PWM_WIDTH_ABSMAX_US) {
		return PWM_WIDTH_MAX_US;
	} else {
		return 0;
	}
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
void RADIO_processChannel(uint8_t ch) {
	// MOVE DATA FROM TEMP TO DATA ARRAY
	data.ch[ch] = RADIO_truncate(rx[ch]);
	// CHECK FOR ACTIVE FLAG
	if (data.ch[ch] > PWM_WIDTH_CENTER_US) {
		data.chActive[ch] = chActive_True;
	} else if (data.ch[ch] < PWM_WIDTH_CENTER_US) {
		data.chActive[ch] = chActive_TrueRev;
	} else {
		data.chActive[ch] = chActive_False;
	}
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EVENT HANDLERS   									*/
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
static void RADIO_CH1_IRQ ( void )
{
	// INITIALISE LOOP VARIABLES
	uint32_t 		now 		= TIM_Read(TIM_RADIO);
	bool 			pos 		= GPIO_Read(RADIO_PWM1_Pin);
	static bool 	pos_p 		= false;
	static uint32_t	tickHigh 	= 0;
	static uint32_t tickLow 	= 0;


	// IGNORE NOISE ON SIGNAL I/P THAT RETURNS FASTER THAN INTERRRUPT SERVICE
	if ( pos_p != pos )
	{
		// RISING EDGE PULSE DETECTED
		if ( pos ) {
			// ASSIGN VARIABLES TO USE ON PULSE LOW
			tickHigh = now;
		}
		// FALLING EDGE PULSE DETECTED
		else {
			// CALCULATE SIGNAL PERIOD AND PULSE WIDTH
			uint32_t period = now - tickLow;
			uint32_t pulse = now - tickHigh;
			// CHECK SIGNAL IS VALID
			if ( pulse <= PWM_WIDTH_ABSMAX_US 	&& pulse >= PWM_WIDTH_ABSMIN_US &&
				 period <= PWM_PERIOD_MAX_US	&& period >= PWM_PERIOD_MIN_US )
			{
				// ASSIGN PULSE TO TEMP DATA ARRAY
				rx[radioCH1] = pulse;
			}
			// UPDATE VARIABLES FOR NEXT LOOP
			tickLow = now;
		}
	}

	//
	pos_p = pos;
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
static void RADIO_CH2_IRQ ( void )
{
	uint32_t 		now 		= TIM_Read(TIM_RADIO);
	bool 			pos 		= GPIO_Read(RADIO_PWM2_Pin);
	static bool 	pos_p 		= false;
	static uint32_t	tickHigh 	= 0;
	static uint32_t tickLow 	= 0;

	if ( pos_p != pos )
	{
		if ( pos ) {
			tickHigh = now;
		} else {
			uint32_t period = now - tickLow;
			uint32_t pulse = now - tickHigh;
			if ( pulse <= PWM_WIDTH_ABSMAX_US 	&& pulse >= PWM_WIDTH_ABSMIN_US &&
				 period <= PWM_PERIOD_MAX_US	&& period >= PWM_PERIOD_MIN_US )
			{
				rx[radioCH2] = pulse;
			}
			tickLow = now;
		}
	}
	pos_p = pos;
}


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
#if RADIO_NUM_CHANNELS >= 3
static void RADIO_CH3_IRQ ( void )
{
	uint32_t 		now 		= TIM_Read(TIM_RADIO);
	bool 			pos 		= GPIO_Read(RADIO_PWM3_Pin);
	static bool 	pos_p 		= false;
	static uint32_t	tickHigh 	= 0;
	static uint32_t tickLow 	= 0;

	if ( pos_p != pos )
	{
		if ( pos ) {
			tickHigh = now;
		} else {
			uint32_t period = now - tickLow;
			uint32_t pulse = now - tickHigh;
			if ( pulse <= PWM_WIDTH_ABSMAX_US 	&& pulse >= PWM_WIDTH_ABSMIN_US &&
				 period <= PWM_PERIOD_MAX_US	&& period >= PWM_PERIOD_MIN_US )
			{
				rx[radioCH3] = pulse;
			}
			tickLow = now;
		}
	}
	pos_p = pos;
}
#endif


/*
 * TEXT
 *
 * INPUTS:
 * OUTPUTS:
 */
#if RADIO_NUM_CHANNELS >= 4
static void RADIO_CH4_IRQ ( void )
{
	uint32_t 		now 		= TIM_Read(TIM_RADIO);
	bool 			pos 		= GPIO_Read(RADIO_PWM4_Pin);
	static bool 	pos_p 		= false;
	static uint32_t	tickHigh 	= 0;
	static uint32_t tickLow 	= 0;

	if ( pos_p != pos )
	{
		if ( pos ) {
			tickHigh = now;
		} else {
			uint32_t period = now - tickLow;
			uint32_t pulse = now - tickHigh;
			if ( pulse <= PWM_WIDTH_ABSMAX_US 	&& pulse >= PWM_WIDTH_ABSMIN_US &&
				 period <= PWM_PERIOD_MAX_US	&& period >= PWM_PERIOD_MIN_US )
			{
				rx[radioCH4] = pulse;
			}
			tickLow = now;
		}
	}
	pos_p = pos;
}
#endif


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
