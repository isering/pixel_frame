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
 * coord.h
 *
 * Created: 21.10.2012 00:40:14
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Typedef for coordinates type.
 */ 


#ifndef COORD_H_
#define COORD_H_

#include <stdint.h>

typedef uint8_t coord_t;

#define get_pos(x,y) (SIZE_X-1 - (x) + SIZE_X*(y))
#define get_pos_x(pos) (SIZE_X-1-((pos)%SIZE_X))
#define get_pos_y(pos) ((pos)/SIZE_X)

#define set_pos_x(pos, x) ((pos)=((pos)&(SIZE_X*(SIZE_Y-1)))|(x))
#define set_pos_y(pos, y) ((pos)=((pos)&(SIZE_X-1))|((y)*SIZE_X))

#endif /* COORD_H_ */