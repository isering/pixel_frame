 /*
  * pixel_frame
  * Copyright (C) 2013  Julian Iseringhausen <julian.iseringhausen@gmail.com>
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */

/*
 * bitmap.c
 *
 * Created: 20.10.2012 23:20:16
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Routines for bitmap manipulation.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <avr/io.h>
#include <stdint.h>

#include "coord.h"
#include "gamma.h"
#include "rgb.h"
#include "tlc5940.h"

void bitmap_set(coord_t pos, uint16_t color_r, uint16_t color_g, uint16_t color_b)
{
  uint8_t temp, temp2;
  uint8_t* ptr;
  
  color_r = get_red(color_r);
  color_g = get_green(color_g);
  color_b = get_blue(color_b);
  
  ptr = ptr_back;
  
  asm volatile (
    "mov %[temp], %[pos]"      "\n\t"
    "swap %[temp]"             "\n\t"
    "andi %[temp], 0x0f"       "\n\t"
    "ldi %[temp2], 72"         "\n\t"
    "mul %[temp], %[temp2]"    "\n\t"
    "add %A[ptr], r0"          "\n\t"
    "adc %B[ptr], r1"          "\n\t"
    "andi %[pos], 0x0f"        "\n\t"
    "ldi %[temp], 3"           "\n\t"
    "mul %[pos], %[temp]"      "\n\t"
    "lsr r0"                   "\n\t"
    "add %A[ptr], r0"          "\n\t"
    "adc %B[ptr], r1"          "\n\t"
    "andi %[pos], 1"           "\n\t"
    "breq mod_0%="             "\n\t"
  "mod_1%=: "
    "ld %[temp], %a[ptr]"      "\n\t"
    "andi %[temp], 0xf0"       "\n\t"
    "or %B[color_r], %[temp]"  "\n\t"
    "st %a[ptr]+, %B[color_r]" "\n\t"
    "st %a[ptr], %A[color_r]"  "\n\t"
    "adiw %[ptr], 23"          "\n\t"
    "ld %[temp], %a[ptr]"      "\n\t"
    "andi %[temp], 0xf0"       "\n\t"
    "or %B[color_g], %[temp]"  "\n\t"
    "st %a[ptr]+, %B[color_g]" "\n\t"
    "st %a[ptr], %A[color_g]"  "\n\t"
    "adiw %[ptr], 23"          "\n\t"
    "ld %[temp], %a[ptr]"      "\n\t"
    "andi %[temp], 0xf0"       "\n\t"
    "or %B[color_b], %[temp]"  "\n\t"
    "st %a[ptr]+, %B[color_b]" "\n\t"
    "st %a[ptr], %A[color_b]"  "\n\t"
    "rjmp end%="               "\n\t"
  "mod_0%=: "
    "swap %A[color_r]"         "\n\t"
    "swap %B[color_r]"         "\n\t"
    "mov %[temp], %A[color_r]" "\n\t"
    "andi %[temp], 0x0f"       "\n\t"
    "or %B[color_r], %[temp]"  "\n\t"
    "st %a[ptr]+, %B[color_r]" "\n\t"
    "ld %[temp], %a[ptr]"      "\n\t"
    "andi %[temp], 0x0f"       "\n\t"
    "andi %A[color_r], 0xf0"   "\n\t"
    "or %A[color_r], %[temp]"  "\n\t"
    "st %a[ptr], %A[color_r]"  "\n\t"
    "adiw %[ptr], 23"          "\n\t"
    "swap %A[color_g]"         "\n\t"
    "swap %B[color_g]"         "\n\t"
    "mov %[temp], %A[color_g]" "\n\t"
    "andi %[temp], 0x0f"       "\n\t"
    "or %B[color_g], %[temp]"  "\n\t"
    "st %a[ptr]+, %B[color_g]" "\n\t"
    "ld %[temp], %a[ptr]"      "\n\t"
    "andi %[temp], 0x0f"       "\n\t"
    "andi %A[color_g], 0xf0"   "\n\t"
    "or %A[color_g], %[temp]"  "\n\t"
    "st %a[ptr], %A[color_g]"  "\n\t"
    "adiw %[ptr], 23"          "\n\t"
    "swap %A[color_b]"         "\n\t"
    "swap %B[color_b]"         "\n\t"
    "mov %[temp], %A[color_b]" "\n\t"
    "andi %[temp], 0x0f"       "\n\t"
    "or %B[color_b], %[temp]"  "\n\t"
    "st %a[ptr]+, %B[color_b]" "\n\t"
    "ld %[temp], %a[ptr]"      "\n\t"
    "andi %[temp], 0x0f"       "\n\t"
    "andi %A[color_b], 0xf0"   "\n\t"
    "or %A[color_b], %[temp]"  "\n\t"
    "st %a[ptr], %A[color_b]"  "\n\t"
  "end%=: "
  : [ptr] "+e" (ptr),
    [pos] "+d" (pos),
    [temp] "=&d" (temp),
    [temp2] "=d" (temp2),
    [color_r] "+r" (color_r),
    [color_g] "+r" (color_g),
    [color_b] "+r" (color_b)
  );
}

void bitmap_set_row(const uint8_t row, uint16_t val)
{
  uint8_t* ptr = ptr_back;
  uint8_t temp, digit;
  asm volatile (
    "ldi %[temp], 24"       "\n\t"
    "mul %[temp], %[row]"   "\n\t"
    "add %A[ptr], r0"       "\n\t"
    "adc %B[ptr], r1"       "\n\t"
    "andi %B[val], 0x0f"    "\n\t"
    "mov %[digit], %A[val]" "\n\t"
    "swap %[digit]"         "\n\t"
    "andi %[digit], 0xf0"   "\n\t"
    "or %[digit], %B[val]"  "\n\t"
    "swap %B[val]"          "\n\t"
    "mov %[temp], %A[val]"  "\n\t"
    "swap %[temp]"          "\n\t"
    "andi %[temp], 0x0f"    "\n\t"
    "or %B[val], %[temp]"   "\n\t"
    "ldi %[temp], 8"        "\n\t"
  "loop%=: "
    "st %a[ptr]+, %B[val]"  "\n\t"
    "st %a[ptr]+, %[digit]" "\n\t"
    "st %a[ptr]+, %A[val]"  "\n\t"
    "dec %[temp]"           "\n\t"
    "brne loop%="           "\n\t"
    "clr __zero_reg__"
  : [ptr] "+e" (ptr),
    [val] "+d"  (val),
    [digit] "=d" (digit),
    [temp]"=&d" (temp)
  : [row] "r" (row)
  );
}

void bitmap_set_plane(const uint8_t plane, uint16_t color_r, uint16_t color_g, uint16_t color_b)
{
  uint8_t* ptr = ptr_back;
  uint8_t temp, digit;
  
  color_r = get_red(color_r);
  color_g = get_green(color_g);
  color_b = get_blue(color_b);
  
  asm volatile (
    "ldi %[temp], 72"           "\n\t"
    "mul %[plane], %[temp]"     "\n\t"
    "add %A[ptr], r0"           "\n\t"
    "adc %B[ptr], r1"           "\n\t"
    
    "mov %[digit], %A[color_r]" "\n\t"
    "swap %[digit]"             "\n\t"
    "andi %[digit], 0xf0"       "\n\t"
    "or %[digit], %B[color_r]"  "\n\t"
    "swap %B[color_r]"          "\n\t"
    "mov %[temp], %A[color_r]"  "\n\t"
    "swap %[temp]"              "\n\t"
    "andi %[temp], 0x0f"        "\n\t"
    "or %B[color_r], %[temp]"   "\n\t"
    "ldi %[temp], 8"            "\n\t"
  "loop_r%=: "
    "st %a[ptr]+, %B[color_r]"  "\n\t"
    "st %a[ptr]+, %[digit]"     "\n\t"
    "st %a[ptr]+, %A[color_r]"  "\n\t"
    "dec %[temp]"               "\n\t"
    "brne loop_r%="             "\n\t"
    
    "mov %[digit], %A[color_g]" "\n\t"
    "swap %[digit]"             "\n\t"
    "andi %[digit], 0xf0"       "\n\t"
    "or %[digit], %B[color_g]"  "\n\t"
    "swap %B[color_g]"          "\n\t"
    "mov %[temp], %A[color_g]"  "\n\t"
    "swap %[temp]"              "\n\t"
    "andi %[temp], 0x0f"        "\n\t"
    "or %B[color_g], %[temp]"   "\n\t"
    "ldi %[temp], 8"            "\n\t"
  "loop_g%=: "
    "st %a[ptr]+, %B[color_g]"  "\n\t"
    "st %a[ptr]+, %[digit]"     "\n\t"
    "st %a[ptr]+, %A[color_g]"  "\n\t"
    "dec %[temp]"               "\n\t"
    "brne loop_g%="             "\n\t"
        
    "mov %[digit], %A[color_b]" "\n\t"
    "swap %[digit]"             "\n\t"
    "andi %[digit], 0xf0"       "\n\t"
    "or %[digit], %B[color_b]"  "\n\t"
    "swap %B[color_b]"          "\n\t"
    "mov %[temp], %A[color_b]"  "\n\t"
    "swap %[temp]"              "\n\t"
    "andi %[temp], 0x0f"        "\n\t"
    "or %B[color_b], %[temp]"   "\n\t"
    "ldi %[temp], 8"            "\n\t"
  "loop_b%=: "
    "st %a[ptr]+, %B[color_b]"  "\n\t"
    "st %a[ptr]+, %[digit]"     "\n\t"
    "st %a[ptr]+, %A[color_b]"  "\n\t"
    "dec %[temp]"               "\n\t"
    "brne loop_b%="             "\n\t"
    
    "clr __zero_reg__"
  : [ptr] "+e" (ptr),
    [temp]  "=&d" (temp),
    [digit] "=&d" (digit)
  : [plane] "r" (plane),
    [color_r] "r" (color_r),
    [color_g] "r" (color_g),
    [color_b] "r" (color_b)
  );
}

void bitmap_set_all(uint16_t color_r, uint16_t color_g, uint16_t color_b)
{
  bitmap_set_plane(0, color_r, color_g, color_b);
  bitmap_set_plane(1, color_r, color_g, color_b);
  bitmap_set_plane(2, color_r, color_g, color_b);
  bitmap_set_plane(3, color_r, color_g, color_b);
}

void bitmap_clear()
{
  uint8_t* ptr = ptr_back;
  uint8_t i,j;
  asm volatile (
    "ldi %[i], %[c_i]"          "\n\t"
    "ldi %[j], %[c_j]"          "\n\t"
  "loop%=: "
    "st %a[ptr]+, __zero_reg__" "\n\t"
    "dec %[j]"                  "\n\t"
    "brne loop%="               "\n\t"
    "ldi %[j], %[c_j]"          "\n\t"
    "dec %[i]"                  "\n\t"
    "brne loop%="
  : [ptr] "+e" (ptr),
    [i] "=d" (i),
    [j] "=d" (j)
  : [c_i] "M" (TLC5940_MULTIPLEX_N),
    [c_j] "M" (24)
  );
}

void bitmap_copy()
{
  uint8_t* ptr_from = ptr_front;
  uint8_t* ptr_to = ptr_back;
  uint16_t temp = 24 * TLC5940_MULTIPLEX_N;
  
 do {
    *ptr_to++ = *ptr_from++;
  }while (--temp);
}