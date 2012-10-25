/*
 * serial.c
 *
 * Created: 21.10.2012 12:08:43
 *  Author: Julian
 */ 

#include "config.h"

#ifdef AVRSNIPPETS_USE_SERIAL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stddef.h>

#include "avrsnippets/serial/serial.h"

#ifdef SERIAL_ENABLE_RX_INT_DEFAULT
volatile uint8_t serial_buffer_in[SERIAL_BUFSIZE_IN];
ringbuffer_t serial_rb_in;
volatile uint8_t serial_data_received = 0;
#endif

#ifdef SERIAL_ENABLE_TX_INT_DEFAULT
volatile uint8_t serial_buffer_out[SERIAL_BUFSIZE_OUT];
ringbuffer_t serial_rb_out;
#endif

#ifdef SERIAL_ENABLE_RX_INT_DEFAULT
ISR(SERIAL_RX_vect) __attribute__((naked));
ISR(SERIAL_RX_vect)
{
  asm volatile (
    "push r16"         "\n\t"
    "in r16, __SREG__" "\n\t"
    "push r16"         "\n\t"
    "push r26"         "\n\t"
    "push r27"         "\n\t"

    "ldi r26, lo8(serial_rb_in + %[offset_begin])" "\n\t"
    "ldi r27, hi8(serial_rb_in + %[offset_begin])" "\n\t"    
    "lds r16, serial_rb_in + %[offset_write]"      "\n\t"
    "add r26, r16"                                 "\n\t"
    "clr r16"                                      "\n\t"
    "adc r27, r16"                                 "\n\t"
    "lds r16, serial_rb_in + %[offset_write]"      "\n\t"
    "inc r16"                                      "\n\t"
    "cpi r16, %[bufsize]"                          "\n\t"
    "brne no_clear%="                              "\n\t"
    "clr r16"                                      "\n\t"
    "no_clear%=: "
    "sts serial_rb_in + %[offset_write], r16"      "\n\t"
    "lds r16, %[SDR]"                              "\n\t"
    "st X, r16"                                    "\n\t"
    
    "pop r27"           "\n\t"
    "pop r26"           "\n\t"
    "pop r16"           "\n\t"
    "out __SREG__, r16" "\n\t"
    "pop r16"           "\n\t"
    "reti"
  : 
  : [offset_write] "M" (offsetof(ringbuffer_t, write)),
    [offset_begin] "M" (offsetof(ringbuffer_t, begin)),
    [bufsize] "M" (SERIAL_BUFSIZE_IN),
    [SDR] "m" (SERIAL_DATA_REGISTER)
  );
}
#endif /* SERIAL_ENABLE_RX_INT_DEFAULT */

#ifdef SERIAL_ENABLE_TX_INT_DEFAULT
ISR(SERIAL_UDRE_vect)
{
  if (serial_rb_out.read != serial_rb_out.write)
  SERIAL_DATA_REGISTER = ringbuffer_get_nowait(&serial_rb_out);
  else
  clear(SERIAL_DRE_INT_CONTROL , SERIAL_DRE_INT);
}

void serial_transmit(uint8_t data)
{
  // Wait for empty transmit buffer
  ringbuffer_put(&serial_rb_out, data);
  set(SERIAL_DRE_INT_CONTROL, SERIAL_DRE_INT);
}

#endif /* SERIAL_ENABLE_TX_INT_DEFAULT */

void serial_init()
{
  // Set baud rate
  SERIAL_BAUDRATE_REGISTER_H = (uint8_t)(SERIAL_UBRR_VAL >> 8);
  SERIAL_BAUDRATE_REGISTER_L = (uint8_t)(SERIAL_UBRR_VAL);
  
  #if SERIAL_CHARSIZE == 5
  SERIAL_CHARSIZE_CONTROL_1 = SERIAL_CHARSIZE_5BIT_1;
  SERIAL_CHARSIZE_CONTROL_2 = SERIAL_CHARSIZE_5BIT_2;
  #elif SERIAL_CHARSIZE == 6
  SERIAL_CHARSIZE_CONTROL_1 = SERIAL_CHARSIZE_6BIT_1;
  SERIAL_CHARSIZE_CONTROL_2 = SERIAL_CHARSIZE_6BIT_2;
  #elif SERIAL_CHARSIZE == 7
  SERIAL_CHARSIZE_CONTROL_1 = SERIAL_CHARSIZE_7BIT_1;
  SERIAL_CHARSIZE_CONTROL_2 = SERIAL_CHARSIZE_7BIT_2;
  #elif SERIAL_CHARSIZE == 8
  SERIAL_CHARSIZE_CONTROL_1 = SERIAL_CHARSIZE_8BIT_1;
  SERIAL_CHARSIZE_CONTROL_2 = SERIAL_CHARSIZE_8BIT_2;
  #elif SERIAL_CHARSIZE == 9
  SERIAL_CHARSIZE_CONTROL_1 = SERIAL_CHARSIZE_9BIT_1;
  SERIAL_CHARSIZE_CONTROL_2 = SERIAL_CHARSIZE_9BIT_2;
  #endif
  
  #ifdef SERIAL_ENABLE_DOUBLE_SPEED
  set(SERIAL_U2X_CONTROL, SERIAL_U2X);
  #endif

  #ifdef SERIAL_ENABLE_RX_INT_DEFAULT
  ringbuffer_init(&serial_rb_in, serial_buffer_in, SERIAL_BUFSIZE_IN);
  #endif
  
  #ifdef SERIAL_ENABLE_TX_INT_DEFAULT
  ringbuffer_init(&serial_rb_out, serial_buffer_out, SERIAL_BUFSIZE_OUT);
  #endif
  
  #ifdef SERIAL_ENABLE_RX_INT
  set(SERIAL_RX_INT_CONTROL, SERIAL_RX_INT);
  #endif
  
  #ifdef SERIAL_ENABLE_RX
  set(SERIAL_RXEN_CONTROL, SERIAL_RXEN);
  #endif

  #ifdef SERIAL_ENABLE_TX
  set(SERIAL_TXEN_CONTROL, SERIAL_TXEN);
  #endif
}

#endif /* AVRSNIPPETS_USE_SERIAL */