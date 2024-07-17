/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef MAIN_H
#define MAIN_H
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include "STM32X.h"

#include "Core.h"
#include "GPIO.h"
#include "ADC.h"
#include "UART.h"
#include "US.h"

#include "Battery.h"
#include "Temperature.h"
#include "Radio.h"
#include "Motor.h"
#include "LED.h"
#include "FPROM.h"


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define GETBIT(var, bit) 	(((var) >> (bit)) & 1)
#define SETBIT(var, bit) 	(var |= (1 << (bit)))
#define RSTBIT(var, bit)	(var &= (~(1 << (bit))))

#define CONFIG_FPROM_OFFSET	0

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC TYPES      									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


typedef enum {
	IP1,
	IP2,
	IP3,
	IP4,
} SYSTEM_InputNumber;

typedef enum {
	CONFIG_S1,
	CONFIG_S2,
	CONFIG_IP3,
	CONFIG_IP4,
} CONFIG_input;

typedef enum {
	CONFIG_TANK,
	CONFIG_ARCADE,
} CONFIG_driveModes;

typedef enum {
	CONFIG_PPM,
	CONFIG_SBUS,
	CONFIG_IBUS,
	CONFIG_PWM,
} CONFIG_radioProtocols;

typedef struct {
	uint32_t 			hashA;

	CONFIG_driveModes	mode;
	bool				motorBraking_EN;

	uint32_t 			motorA_ch;
	bool 				motorA_rev;

	uint32_t 			motorB_ch;
	bool 				motorB_rev;

	uint32_t 			hashB;
} CONFIG_data;


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */




/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EXTERN DECLARATIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* MAIN_H */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
