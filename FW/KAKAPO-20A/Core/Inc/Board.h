
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef BOARD_H
#define BOARD_H
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#define STM32C0

//
#define RADIO_NUM_CHANNELS		4
#define SERVO_NUM_OUTPUTS		2

// CORE CONFIG
#define CORE_USE_TICK_IRQ

// FLASH CONFIG
#define FPROM_PAGE_SIZE 		256

// ADC CONFIG
#define ADC_VREF	       		3300

// CLK CONFIG
#define CLK_SYSCLK_FREQ			48000000

// TIMER CONFIG
#define TIM_USE_IRQS

// TIMER CONFIG - RADIO INPUT
#define TIM14_ENABLE
#define TIM_RADIO				TIM_14
#define TIM_RADIO_RELOAD		0xffff
#define TIM_RADIO_FREQ			1000000
#define PWM_TIM					TIM_RADIO
#define PWM_TIM_FREQ			TIM_RADIO_FREQ
#define PWM_TIM_RELOAD			TIM_RADIO_RELOAD
#define PPM_TIM					TIM_RADIO
#define PPM_TIM_FREQ			TIM_RADIO_FREQ
#define PPM_TIM_RELOAD			TIM_RADIO_RELOAD

// TIMER CONFIG - MOTOR OUTPUT
#define TIM1_ENABLE
#define TIM_MOTORA				TIM_1
#define TIM_MOTORA_CH			0
#define TIM_MOTOR_M2_CH			1
#define TIM_MOTORA_RELOAD		500
#define TIM_MOTORA_FREQ			750000
#define TIM3_ENABLE
#define TIM_MOTORB				TIM_3
#define TIM_MOTORB_CH			0
#define TIM_MOTORB_RELOAD		TIM_MOTORA_RELOAD	// 500
#define TIM_MOTORB_FREQ			TIM_MOTORA_FREQ 		// (((TIM_MOTORB_RELOAD + 1) * 1000) / MOTOR_PERIOD_MS)

// TIMER CONFIG - SERVO OUTPUT
#if SERVO_NUM_OUTPUTS >= 1
#define TIM16_ENABLE
#define TIM_SERVO_S1			TIM_16
#define TIM_SERVO_S1_RELOAD		(10*1000)
#define TIM_SERVO_S1_FREQ		1000000
#if SERVO_NUM_OUTPUTS >= 2
#define TIM17_ENABLE
#define TIM_SERVO_S2			TIM_17
#define TIM_SERVO_S2_RELOAD		(10*1000)
#define TIM_SERVO_S2_FREQ		1000000
#endif
#endif

// SPI CONFIG - MOTOR OUTPUT
#define MOTOR_SPI_CLK			PB3
#define MOTOR_SPI_MOSI			PB5
#define MOTOR_SPI_MISO			PB4
#define MOTORA_Select			PB9
#define MOTORA_Fault			PB7
#define MOTORA_Sense			PA11
#define MOTORA_Sense_Ch			ADC_Channel_11
#define MOTORB_Select			PA15
#define MOTORB_Fault			PB6
#define MOTORB_Sense			PA12
#define MOTORB_Sense_Ch			ADC_Channel_12

#define SPI1_PINS		    	(MOTOR_SPI_CLK | MOTOR_SPI_MOSI | MOTOR_SPI_MISO)
#define SPI1_AF			    	GPIO_AF0_SPI1

// GPIO CONFIG
#define GPIO_USE_IRQS
#define GPIO_IRQ6_ENABLE
#define GPIO_IRQ8_ENABLE
#define GPIO_IRQ9_ENABLE
#define GPIO_IRQ10_ENABLE

// GPIO CONFIG - RADIO INPUTS
#if RADIO_NUM_CHANNELS >= 2
#define RADIO_PWM1_Pin			PA8
#define RADIO_PWM2_Pin			PA9
#if RADIO_NUM_CHANNELS >= 3
#define RADIO_PWM3_Pin			PC6
#if RADIO_NUM_CHANNELS >= 4
#define RADIO_PWM4_Pin			PA10
#endif
#endif
#else
#error "Minimum: x2 Radio Inputs Defined"
#endif

// GPIO CONFIG - SERVO OUTPUT
#if SERVO_NUM_OUTPUTS >= 1
#define SERVO_S1_Pin			PB1
#if SERVO_NUM_OUTPUTS >= 2
#define SERVO_S2_Pin			PB2
#endif
#endif

// GPIO CONFIG - LED OUTPUT
//#define LED_PWR_Pin			PB6
#define LED_M1_Pin				PA0
#define LED_M2_Pin				PB0

// GPIO CONFIG - BATTERY DETECT
#define BATT_Pin				PA1
#define BATT_Ch      			ADC_Channel_1
#define BATT_Rlow				10
#define BATT_Rhigh				100


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC TYPES      									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC FUNCTIONS										*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* EXTERN DECLARATIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#endif /* BOARD_H */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

