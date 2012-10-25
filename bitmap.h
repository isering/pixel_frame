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
 * bitmap.h
 *
 * Created: 21.10.2012 11:58:39
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Routines for bitmap manipulation.
 */ 


#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdint.h>

#include "coord.h"

extern void bitmap_set(coord_t pos, uint16_t color_r, uint16_t color_g, uint16_t color_b);
extern void bitmap_set_row(const uint8_t row, uint16_t val);
extern void bitmap_set_plane(const uint8_t plane, uint16_t color_r, uint16_t color_g, uint16_t color_b);
extern void bitmap_set_all(uint16_t color_r, uint16_t color_g, uint16_t color_b);
extern void bitmap_clear();
extern void bitmap_copy();

#endif /* BITMAP_H_ */