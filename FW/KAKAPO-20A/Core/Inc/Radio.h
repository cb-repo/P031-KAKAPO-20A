
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef RADIO_H
#define RADIO_H
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "STM32X.h"
#include "Core.h"
#include "GPIO.h"
#include "TIM.h"


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


//#define RADIO_MIN(X, Y) 		(((X) < (Y)) ? (X) : (Y))
//#define RADIO_MAX(X, Y) 		(((X) > (Y)) ? (X) : (Y))

//#define RADIO_GETBIT(var, bit) 	(((var) >> (bit)) & 1)
//#define RADIO_SETBIT(var, bit) 	(var |= (1 << (bit)))
//#define RADIO_RSTBIT(var, bit)	(var &= (~(1 << (bit))))

#define RADIO_CH_MIN			1000
#define RADIO_CH_CENTER			1500
#define RADIO_CH_MAX			2000
#define RADIO_CH_ERROR			200 								// Out-of-range Error... Above + Below Min/Max

#define RADIO_CH_HALFSCALE		( RADIO_CH_MAX - RADIO_CH_CENTER )
#define RADIO_CH_FULLSCALE		( RADIO_CH_MAX - RADIO_CH_MIN )


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC TYPES      									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


typedef enum {
	chActive_False,
	chActive_True,
	chActive_TrueRev,
} RADIO_chActiveFlags;

typedef struct {
	bool inputLostCh[RADIO_NUM_CHANNELS];
	uint32_t ch[RADIO_NUM_CHANNELS];
	RADIO_chActiveFlags chActive[RADIO_NUM_CHANNELS];
} RADIO_Data;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


void 		RADIO_Init 				( void );
void 		RADIO_Update 			( void );
RADIO_Data* RADIO_getDataPtr		( void );
bool 		RADIO_inFaultState   	( void );
bool 		RADIO_inFaultStateFULL	( void );
bool 		RADIO_inFaultStateANY	( void );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EXTERN DECLARATIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* RADIO_H */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
