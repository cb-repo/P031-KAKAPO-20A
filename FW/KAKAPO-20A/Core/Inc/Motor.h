
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef MOTOR_H
#define MOTOR_H
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "STM32X.h"
#include "GPIO.h"
#include "TIM.h"
#include "SPI.h"

#include "main.h"
#include "Battery.h"
#include "Temperature.h"
#include "Radio.h"


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define MOTOR_MIN			RADIO_CH_MIN
#define MOTOR_CENTER		RADIO_CH_CENTER
#define MOTOR_MAX			RADIO_CH_MAX

#define MOTOR_HALFSCALE		RADIO_CH_HALFSCALE
#define MOTOR_FULLSCALE		RADIO_CH_FULLSCALE

#define MOTOR_JERK_OFFSET	100
#define MOTOR_JERK_HRVS		(MOTOR_CENTER - MOTOR_JERK_OFFSET)
#define MOTOR_JERK_HFWD		(MOTOR_CENTER + MOTOR_JERK_OFFSET)


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC TYPES      									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


void 	MOTOR_Init 				( void );
void 	MOTOR_Deinit			( void );
void 	MOTOR_Update			( void );

void 	MOTOR_getConfig			( void );

bool 	MOTOR_M1_isActive 		( void );
bool 	MOTOR_M2_isActive		( void );

void 	MOTOR_StartupBuzz 		( void );


void 	MOTOR_calibASSERT 		( void );
void 	MOTOR_calibRELEASE		( void );
void 	MOTOR_calibOFF 			( void );
void 	MOTOR_calibTwitchFWD 	( uint32_t );
void 	MOTOR_calibTwitchLEFT	( uint32_t );


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EXTERN DECLARATIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* MOTOR_H */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
