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
 * settings.h
 *
 * Created: 19.01.2013 11:05:36
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Saves and loads a set of settings to EEPROM,
 *               minimizing the usage of each individual EEPROM
 *               cell.
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <stdint.h>

#define SETTINGS_SIZE 9

typedef struct  
{
  uint8_t id;
  uint8_t mode;
  uint8_t speed;
  uint8_t color;
  uint8_t gain;
  uint8_t density;
  uint8_t sat;
  uint8_t val;
  uint8_t fixed_hue;
} settings_t;

extern void settings_load();
extern void settings_save();
extern void settings_reset();

extern settings_t settings;

#endif /* SETTINGS_H_ */