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
 * pixel_frame.c
 *
 * Created: 12.10.2012 01:27:12
 *  Author: Julian Iseringhausen
 *
 *  Description: Main file.
 */ 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <avr/interrupt.h>

#include "avrsnippets/helper.h"
#include "avrsnippets/random/random.h"
#include "bitmap.h"
#include "effects.h"
#include "fade.h"
#include "fps_handle.h"
#include "settings.h"
#include "tlc5940.h"

int main(void)
{
  //settings_reset();
  settings_load();
  TLC5940_Init();
  random_init();
  fps_handle_init();
  
  bitmap_clear();
  TLC5940_ClockInGS();
  
  sei();
  
  while (1) {

    if (!(FLAGS & (_BV(FLAG_GS_UPDATE) | _BV(FLAG_FPS_UPDATE)))) {
      bitmap_copy();
      effect_handle();
      fade_handle();
      FLAGS |= _BV(FLAG_GS_UPDATE) | _BV(FLAG_FPS_UPDATE);
    }
	
  } 
}