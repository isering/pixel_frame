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
 * gamma.h
 *
 * Created: 19.10.2012 00:04:59
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Gamma corrected brightness values.
 */ 


#ifndef GAMMA_H_
#define GAMMA_H_

#include <avr/pgmspace.h>

extern const uint16_t gamma_red[] PROGMEM;
extern const uint16_t gamma_green[] PROGMEM;
extern const uint16_t gamma_blue[] PROGMEM;

#define get_red(val)   pgm_read_word(&gamma_red[(uint8_t)(val)])
#define get_green(val) pgm_read_word(&gamma_green[(uint8_t)(val)])
#define get_blue(val)  pgm_read_word(&gamma_blue[(uint8_t)(val)])

#endif /* GAMMA_H_ */