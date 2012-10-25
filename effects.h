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
 * effects.h
 *
 * Created: 21.10.2012 18:08:16
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Various graphical effects.
 */ 


#ifndef EFFECTS_H_
#define EFFECTS_H_

#define EFFECTS_COUNT 6

extern uint8_t mode_changed;
extern uint16_t wait;

extern uint8_t equ_data[7];

extern void EffectRain();
extern void EffectShade();
extern void EffectShadeDark();
extern void EffectEqualizer2();
extern void EffectLightOrgan();
extern void EffectStars();

extern void effect_handle();

#endif /* EFFECTS_H_ */