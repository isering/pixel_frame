/*

  tlc5940.s

  Copyright 2010-2011 Matthew T. Pandina. All rights reserved.
  Copyright 2013 Julian Iseringhausen. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY MATTHEW T. PANDINA "AS IS" AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
  EVENT SHALL MATTHEW T. PANDINA OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/*
 * tlc5940.s
 *
 * Created: 12.10.2012 01:39:30
 *  Author: Matthew T. Padina, original code, see https://sites.google.com/site/artcfox/demystifying-the-tlc5940
 *          Julian Iseringhausen, assembler port <julian.iseringhausen@gmail.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <avr/io.h>

/*
 * Global variables
 */
.data

.comm row, 1
.comm bitmap_1, 24*TLC5940_MULTIPLEX_N
.comm bitmap_2, 24*TLC5940_MULTIPLEX_N
.comm ptr_front, 2
.comm ptr_back, 2

/*
 * Code section
 */
.text

.global TLC5940_Init
.func TLC5940_Init
TLC5940_Init:

	// Set multiplex port to output
	in tmp, _SFR_IO_ADDR(MULTIPLEX_DDR)
	ori tmp, MULTIPLEX_MASK
	out _SFR_IO_ADDR(MULTIPLEX_DDR), tmp
	
	// Set TLC5940 ports to output
	sbi _SFR_IO_ADDR(SCLK_DDR), SCLK_PIN
	sbi _SFR_IO_ADDR(DCPRG_DDR), DCPRG_PIN
	sbi _SFR_IO_ADDR(VPRG_DDR), VPRG_PIN
	sbi _SFR_IO_ADDR(XLAT_DDR), XLAT_PIN
	sbi _SFR_IO_ADDR(BLANK_DDR), BLANK_PIN
	sbi _SFR_IO_ADDR(SIN_DDR), SIN_PIN

	// Initialize multiplex port
	in tmp, _SFR_IO_ADDR(MULTIPLEX_PORT)
	ori tmp, MULTIPLEX_MASK
	out _SFR_IO_ADDR(MULTIPLEX_PORT), tmp

	// Initialize TLC5940 ports
	cbi _SFR_IO_ADDR(SCLK_PORT), SCLK_PIN
	cbi _SFR_IO_ADDR(DCPRG_PORT), DCPRG_PIN
	sbi _SFR_IO_ADDR(VPRG_PORT), VPRG_PIN
	cbi _SFR_IO_ADDR(XLAT_PORT), XLAT_PIN
	sbi _SFR_IO_ADDR(BLANK_PORT), BLANK_PIN

	// Initialize flags
	cbi _SFR_IO_ADDR(FLAGS), FLAG_GS_UPDATE
	cbi _SFR_IO_ADDR(FLAGS), FLAG_XLAT_NEEDS_PULSE

	// Initialize SPI
	ldi tmp, (1<<SPE)|(1<<MSTR)
	out _SFR_IO_ADDR(SPCR), tmp

	ldi tmp, (1<<SPI2X)
	out _SFR_IO_ADDR(SPSR), tmp

	// Initialize timer
	ldi tmp, (1<<WGM01)
	out _SFR_IO_ADDR(TCCR0A), tmp

	ldi tmp, (1<<CS02)
	out _SFR_IO_ADDR(TCCR0B), tmp

	ldi tmp, 15
	out _SFR_IO_ADDR(OCR0A), tmp

	lds tmp, TIMSK0
	ori tmp, (1<<OCIE0A)
	sts TIMSK0, tmp

	// Initialize row counter
	sts row, r1

	// Initialize frame buffer pointers
	ldi tmp, lo8(bitmap_1)
	sts ptr_front, tmp
	ldi tmp, hi8(bitmap_1)
	sts ptr_front+1, tmp
	ldi tmp, lo8(bitmap_2)
	sts ptr_back, tmp
	ldi tmp, hi8(bitmap_2)
	sts ptr_back+1, tmp

	ret
.endfunc

.global TIMER0_COMPA_vect
TIMER0_COMPA_vect:

	// Push used registers to stack
	push r0
	push r1
	in r0, _SFR_IO_ADDR(SREG)
	push r0
	push tmp
	push tmp2
	push tmp3
	push XL
	push XH

	/*
	 * Send data to the TLC5940.
	 * For more insight, visit https://sites.google.com/site/artcfox/demystifying-the-tlc5940
	 */
	sbi _SFR_IO_ADDR(BLANK_PORT), BLANK_PIN
	sbis _SFR_IO_ADDR(FLAGS), FLAG_XLAT_NEEDS_PULSE
	rjmp no_xlat_update
	sbi _SFR_IO_ADDR(XLAT_PORT), XLAT_PIN
	cbi _SFR_IO_ADDR(XLAT_PORT), XLAT_PIN
	cbi _SFR_IO_ADDR(FLAGS), FLAG_XLAT_NEEDS_PULSE
	lds tmp, row
	in tmp2, _SFR_IO_ADDR(MULTIPLEX_PORT)
	andi tmp2, 0xc0
	or tmp2, tmp
	out _SFR_IO_ADDR(MULTIPLEX_PORT), tmp2
	inc tmp
	cpi tmp, TLC5940_MULTIPLEX_N
	brne no_xlat_update
	clr tmp
no_xlat_update:
	sts row, tmp
	cbi _SFR_IO_ADDR(BLANK_PORT), BLANK_PIN
	lds XL, ptr_front
	lds XH, ptr_front+1
	ldi tmp2, 24
	mul tmp, tmp2
	add XL, r0
	adc XH, r1
spi_send_loop:
	ld tmp3, X+
	out _SFR_IO_ADDR(SPDR), tmp3
spi_send_wait:
	in tmp3, _SFR_IO_ADDR(SPSR)
	sbrs tmp3, SPIF
	rjmp spi_send_wait
	dec tmp2
	brne spi_send_loop
	cpi tmp, TLC5940_MULTIPLEX_N-1
	brne no_gs_update
	sbis _SFR_IO_ADDR(FLAGS), FLAG_GS_UPDATE
	rjmp no_gs_update
	cbi _SFR_IO_ADDR(FLAGS), FLAG_GS_UPDATE
	lds tmp, ptr_front
	lds tmp2, ptr_back
	sts ptr_back, tmp
	sts ptr_front, tmp2
	lds tmp, ptr_front+1
	lds tmp2, ptr_back+1
	sts ptr_back+1, tmp
	sts ptr_front+1, tmp2
no_gs_update:
	sbi _SFR_IO_ADDR(FLAGS), FLAG_XLAT_NEEDS_PULSE

	// Restore used registers
	pop XH
	pop XL
	pop tmp3
	pop tmp2
	pop tmp
	pop r0
	out _SFR_IO_ADDR(SREG), r0
	pop r1
	pop r0
	reti

/*
 * Send first frame to the TLC5940
 */
.global TLC5940_ClockInGS
.func TLC5940_ClockInGS
TLC5940_ClockInGS:
	clr tmp
	sbis _SFR_IO_ADDR(VPRG_PORT), VPRG_PIN
	rjmp no_first_cycle
	cbi _SFR_IO_ADDR(VPRG_PORT), VPRG_PIN
	inc tmp
no_first_cycle:
	cbi _SFR_IO_ADDR(BLANK_PORT), BLANK_PIN
	ldi XL, lo8(bitmap_2+24*(TLC5940_MULTIPLEX_N-1))
	ldi XH, hi8(bitmap_2+24*(TLC5940_MULTIPLEX_N-1))
	ldi tmp2, 24
clock_in_loop:
	ld tmp3, X+
	out _SFR_IO_ADDR(SPDR), tmp3
clock_in_wait:
	in tmp3, _SFR_IO_ADDR(SPSR)
	sbrs tmp3, SPIF
	rjmp clock_in_wait	
	dec tmp2
	brne clock_in_loop
	sbi _SFR_IO_ADDR(BLANK_PORT), BLANK_PIN
	sbi _SFR_IO_ADDR(XLAT_PORT), XLAT_PIN
	cbi _SFR_IO_ADDR(XLAT_PORT), XLAT_PIN
	sbrs tmp, 0
	rjmp clock_in_end
	sbi _SFR_IO_ADDR(SCLK_PORT), SCLK_PIN
	cbi _SFR_IO_ADDR(SCLK_PORT), SCLK_PIN
clock_in_end:
	lds tmp, ptr_front
	lds tmp2, ptr_back
	sts ptr_back, tmp
	sts ptr_front, tmp2
	lds tmp, ptr_front+1
	lds tmp2, ptr_back+1
	sts ptr_back+1, tmp
	sts ptr_front+1, tmp2
	ret
.endfunc