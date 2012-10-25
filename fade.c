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
 * fade.c
 *
 * Created: 01.11.2012 00:15:00
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *  Description: Different ways to fade from one color value to
 *               another.
 */ 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <avr/io.h>

#include "bitmap.h"
#include "fade.h"
#include "hsv.h"
#include "rgb.h"

fade_buffer_t fade_buffer[FADE_BUFFER_SIZE];

/*
 * Initialization routine
 */
void fade_init()
{
  for (uint8_t i=0; i<FADE_BUFFER_SIZE; ++i) {
    fade_buffer[i].counter = 0;
    fade_buffer[i].duration = 0;
  }     
}

/*
 * Add a fade to buffer
 */
void fade_add(uint8_t mode, coord_t pos, uint8_t pos_type, hsv_t color_from, hsv_t color_to, uint16_t duration)
{
   uint8_t i;
  
  for (i=0; i<FADE_BUFFER_SIZE; ++i)
    if (fade_buffer[i].pos == pos && fade_buffer[i].pos_type == pos_type) {
      if (mode == FADE_MODE_SMOOTH_REVERSE) {
        fade_buffer[i].hsv = fade_buffer[i].hsv_to;
        fade_buffer[i].hsv_to = fade_buffer[i].hsv_from;
        fade_buffer[i].hsv_from = fade_buffer[i].hsv;
        fade_buffer[i].counter = 0;
        break;
      }else if (fade_buffer[i].counter != fade_buffer[i].duration) {
        fade_buffer_t* ptr = &fade_buffer[i];
        ptr->mode = mode;
        ptr->hsv_from = ptr->hsv;
        ptr->hsv_to = color_to;
        ptr->duration = duration;
        ptr->counter = 0;
        break;
      }      
    }
  
  if (i < FADE_BUFFER_SIZE || mode == FADE_MODE_SMOOTH_REVERSE) return;
  
  for (i=0; i<FADE_BUFFER_SIZE; ++i)
    if (fade_buffer[i].counter == fade_buffer[i].duration) {
      fade_buffer_t* ptr = &fade_buffer[i];
      ptr->mode = mode;
      ptr->pos = pos;
      ptr->pos_type = pos_type;
      ptr->hsv = color_from;
      ptr->hsv_from = color_from;
      ptr->hsv_to = color_to;
      ptr->duration = duration;
      ptr->counter = 0;
      break;
    }    
}

/*
 * Stop all fades
 */
void fade_clear()
{
  for (uint8_t i=0; i<FADE_BUFFER_SIZE; ++i)
    fade_buffer[i].counter = fade_buffer[i].duration;
}

/*
 * Handle fading. Call this in main loop.
 */
void fade_handle()
{
  rgb_t r, g, b;
  hsv_t hsv = {0, 0, 0};
  int16_t hue_int;
  int16_t dir;     
  
  for (uint8_t i=0; i<FADE_BUFFER_SIZE; ++i) {
    if (fade_buffer[i].counter < fade_buffer[i].duration) {
      switch(fade_buffer[i].mode)
      {
      case FADE_MODE_DIRECT_SWITCH:
        if (fade_buffer[i].counter < (fade_buffer[i].duration>>1))
          hsv = fade_buffer[i].hsv_from;
        else
          hsv = fade_buffer[i].hsv_to;
        break;
      case FADE_MODE_BLINK:
        if ((fade_buffer[i].counter&0x20) == 0)
          hsv = fade_buffer[i].hsv_from;
        else
          hsv = fade_buffer[i].hsv_to;
        break;
      case FADE_MODE_SMOOTH:
        dir = fade_buffer[i].hsv_to.h - fade_buffer[i].hsv_from.h;
        if (dir > 0x300) dir -= 0x600;
        else if (dir < -1*0x300) dir += 0x600;
        hue_int = fade_buffer[i].hsv_from.h + (int16_t)((fade_buffer[i].counter * (int32_t)dir) / fade_buffer[i].duration);
        if (hue_int < 0) hue_int += 0x600;
        else if (hue_int >= 0x600) hue_int -= 0x600;
        hsv.h = (uint16_t)hue_int;
        dir = fade_buffer[i].hsv_to.s - fade_buffer[i].hsv_from.s;
        hsv.s = fade_buffer[i].hsv_from.s + (int16_t)((fade_buffer[i].counter * (int32_t)dir) / fade_buffer[i].duration);
        dir = fade_buffer[i].hsv_to.v - fade_buffer[i].hsv_from.v;
        hsv.v = fade_buffer[i].hsv_from.v + (int16_t)((fade_buffer[i].counter * (int32_t)dir) / fade_buffer[i].duration);
        break;
      default:
        break;
      }
      hsv_to_rgb(hsv, &r, &g, &b);
      fade_buffer[i].hsv = hsv;
      switch (fade_buffer[i].pos_type)
      {
        case FADE_DIRECT:
          bitmap_set(fade_buffer[i].pos, r, g, b);
          break;
        case FADE_ROW:
          bitmap_set_row(3*get_pos_y(fade_buffer[i].pos)  , r);
          bitmap_set_row(3*get_pos_y(fade_buffer[i].pos)+1, g);
          bitmap_set_row(3*get_pos_y(fade_buffer[i].pos)+2, b);
          break;
        case FADE_COLUMN:
          for (uint8_t y=0; y<4; ++y)
            bitmap_set(get_pos(get_pos_x(fade_buffer[i].pos), y), r, g, b);
          break;
        case FADE_ALL:
          bitmap_set_all(r, g, b);
          break;
        default:
        break;
      }
      bitmap_set(fade_buffer[i].pos, r, g, b);
      ++fade_buffer[i].counter;
    }
  }
}