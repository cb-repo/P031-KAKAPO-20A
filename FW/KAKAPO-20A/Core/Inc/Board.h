
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef BOARD_H
#define BOARD_H
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* PUBLIC DEFINITIONS									*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#define STM32C0

// Core config
#define CORE_USE_TICK_IRQ

// Flash config
#define FPROM_PAGE_SIZE 	256

// ADC config
#define ADC_VREF	        3300

// CLK config
#define CLK_SYSCLK_FREQ		48000000

// TIM config
#define TIM_USE_IRQS

// MOTOR I/P
#define TIM1_ENABLE
#define TIM_MOTORA			TIM_1
#define TIM_MOTORA_CH		0
#define TIM_MOTOR_M2_CH		1
#define TIM_MOTORA_RELOAD	500
#define TIM_MOTORA_FREQ		750000
#define TIM3_ENABLE
#define TIM_MOTORB			TIM_3
#define TIM_MOTORB_CH		0
#define TIM_MOTORB_RELOAD	TIM_MOTORA_RELOAD	// 500
#define TIM_MOTORB_FREQ		TIM_MOTORA_FREQ 		// (((TIM_MOTORB_RELOAD + 1) * 1000) / MOTOR_PERIOD_MS)

// RADIO I/P
#define TIM14_ENABLE
#define TIM_RADIO			TIM_14
#define TIM_RADIO_RELOAD	0xffff
#define TIM_RADIO_FREQ		1000000
#define PWM_TIM				TIM_RADIO
#define PWM_TIM_FREQ		TIM_RADIO_FREQ
#define PWM_TIM_RELOAD		TIM_RADIO_RELOAD
#define PPM_TIM				TIM_RADIO
#define PPM_TIM_FREQ		TIM_RADIO_FREQ
#define PPM_TIM_RELOAD		TIM_RADIO_RELOAD

// SERVO1 O/P
#define TIM16_ENABLE
#define TIM_SERVO1			TIM_16
#define TIM_SERVO1_RELOAD	(10*1000)
#define TIM_SERVO1_FREQ		1000000

// SERVO2 O/P
#define TIM17_ENABLE
#define TIM_SERVO2			TIM_17
#define TIM_SERVO2_RELOAD	(10*1000)
#define TIM_SERVO2_FREQ		1000000

// GPIO config
#define GPIO_USE_IRQS
#define GPIO_IRQ1_ENABLE
#define GPIO_IRQ2_ENABLE

// RADIO INPUTS
#define RADIO_NUM_CHANNELS		2
#define RADIO_PWM1_Pin			PA1
#define RADIO_PWM2_Pin			PA2

// MOTOR
#define MOTOR_SPI_CLK		PA5
#define MOTOR_SPI_MOSI		PA7
#define MOTOR_SPI_MISO		PA6
#define MOTORA_Select		PA8
#define MOTORA_Fault		PA11
#define MOTORA_Sense		PA4
#define MOTORA_Sense_Ch		ADC_Channel_4
#define MOTORB_Select		PC14
#define MOTORB_Fault		PB7
#define MOTORB_Sense		PA0
#define MOTORB_Sense_Ch		ADC_Channel_0

#define SPI1_GPIO		    GPI0A
#define SPI1_PINS		    (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)
#define SPI1_AF			    GPIO_AF0_SPI1

// LED O/P
#define LED_PWR_Pin			PB6
#define LED_M1_Pin			PA3
#define LED_M2_Pin			PC15

// BATTERY DETECT
#define BATT_Pin			PA12
#define BATT_Ch      		ADC_Channel_12
#define BATT_Rlow			10
#define BATT_Rhigh			100


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
