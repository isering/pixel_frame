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
 * fade.h
 *
 * Created: 01.11.2012 00:14:53
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Different ways to fade from one color value to
 *               another.
 */ 


#ifndef FADE_H_
#define FADE_H_

#include <stdint.h>

#include "coord.h"
#include "hsv.h"

#define FADE_BUFFER_SIZE 8

#define FADE_MODE_GLOBAL 0
#define FADE_MODE_DIRECT_SWITCH 1
#define FADE_MODE_BLINK 2
#define FADE_MODE_SMOOTH 3
#define FADE_MODE_SMOOTH_REVERSE 4

#define FADE_DIRECT 0
#define FADE_ROW 1
#define FADE_COLUMN 2
#define FADE_ALL 3

typedef struct  
{
  uint8_t mode;
  coord_t pos;
  uint8_t pos_type;
  hsv_t hsv, hsv_from, hsv_to;
  uint16_t duration;
  uint16_t counter;
}fade_buffer_t;

extern fade_buffer_t fade_buffer[];
extern uint8_t fade_mode;

extern void fade_add(uint8_t mode, coord_t pos, uint8_t pos_type, hsv_t color_from, hsv_t color_to, uint16_t duration);
extern void fade_clear();
extern void fade_handle();


#endif /* FADE_H_ */