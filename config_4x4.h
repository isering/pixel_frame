/*
 * config.h
 *
 * Created: 12.10.2012 01:39:39
 *  Author: Julian Iseringhausen
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#define TLC5940_N 1            // Number of TLC5940 chips
#define TLC5940_MULTIPLEX_N 3  // Number of rows driven by one chip

#define SIZE_X 4               // Number of LEDs in x direction
#define SIZE_Y 4               // Number of LEDs in y direction
#define SIZE_X_BITS 2          // Number of bits necessary to represent SIZE_X - 1
#define SIZE_Y_BITS 2          // Number of bits nevessary to represent SIZE_Y - 1

/* Pins for multiplexing */
#define MULTIPLEX_DDR DDRC 
#define MULTIPLEX_PORT PORTC
#define MULTIPLEX_MASK 0x3f

/* DCPRG pin */
#define DCPRG_DDR DDRD
#define DCPRG_PORT PORTD
#define DCPRG_PIN PD4

/* VPRG pin */
#define VPRG_DDR DDRD
#define VPRG_PORT PORTD
#define VPRG_PIN PD7

/* XLAT pin */
#define XLAT_DDR DDRB
#define XLAT_PORT PORTB
#define XLAT_PIN PB1

/* SIN pin */
#define SIN_DDR DDRB
#define SIN_PORT PORTB
#define SIN_PIN PB3

/* SCLK pin */
#define SCLK_DDR DDRB
#define SCLK_PORT PORTB
#define SCLK_PIN PB5

/* BLANK pin */
#define BLANK_DDR DDRB
#define BLANK_PORT PORTB
#define BLANK_PIN PB2

/* GPIO register flags */
#define FLAGS GPIOR0
#define FLAG_GS_UPDATE 0
#define FLAG_XLAT_NEEDS_PULSE 1
#define FLAG_FPS_UPDATE 2
#define FLAG_EFFECT_MODE_CHANGED 3
#define FLAG_UART_BIT_9 4

#define AVRSNIPPETS_USE_RANDOM      // Use AVRSnippets random number generator
#define AVRSNIPPETS_USE_SEED_EEPROM // Use AVRSnippets EEPROM random number seed

#undef AVRSNIPPETS_USE_SERIAL       // Use AVRSnippets serial library
#undef AVRSNIPPETS_USE_RINGBUFFER   // Use AVRSnippets ringbuffer for serial communication

/* UART settings */
//#define SERIAL_ENABLE_RX
//#define SERIAL_ENABLE_RX_INT
//#define SERIAL_BAUDRATE 250000UL
//#define SERIAL_CHARSIZE 9

/* For convenience */
#define EFFECT_GS_UPDATE 0
#define EFFECT_GS_NOUPDATE 1

/* EEPROM range for random number seed */
#define RANDOM_EEPROM_BEGIN 0
#define RANDOM_EEPROM_END 256

/* EEPROM range for settings */
#define SETTINGS_EEPROM_BEGIN 256
#define SETTINGS_EEPROM_END 512

#ifdef __ASSEMBLER__

#define tmp  r18
#define tmp2  r19
#define tmp3  r20

#endif


#endif /* CONFIG_H_ */