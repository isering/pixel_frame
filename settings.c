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
 * settings.c
 *
 * Created: 19.01.2013 11:11:37
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Saves and loads a set of settings to EEPROM,
 *               minimizing the usage of each individual EEPROM
 *               cell.
 */ 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <avr/eeprom.h>

#include "settings.h"

settings_t settings;

/*
 * Load settings from EEPROM
 */
void settings_load()
{
  uint8_t* ptr;
  settings.id = eeprom_read_byte((uint8_t*)SETTINGS_EEPROM_BEGIN);
  
  // Search for the last used EEPROM block
  for (ptr=(uint8_t*)SETTINGS_EEPROM_BEGIN+SETTINGS_SIZE; ptr<(uint8_t*)SETTINGS_EEPROM_END; ptr+=SETTINGS_SIZE)
    if (++settings.id != eeprom_read_byte(ptr)) break;
    
  if (ptr >= (uint8_t*)SETTINGS_EEPROM_END) ++settings.id;
  
  // Read settings
  eeprom_read_block(&settings, ptr-SETTINGS_SIZE, SETTINGS_SIZE);
  ++settings.id;
}

/*
 * Save settings from EEPROM
 */
void settings_save()
{
  uint8_t* ptr;
  
  // Search for the last used EEPROM block
  for (ptr=(uint8_t*)SETTINGS_EEPROM_BEGIN; ptr<(uint8_t*)SETTINGS_EEPROM_END; ptr+=SETTINGS_SIZE)
    if (settings.id-1 == eeprom_read_byte(ptr)) break;
    
  if (ptr >= (uint8_t*)SETTINGS_EEPROM_END - SETTINGS_SIZE)
    ptr = (uint8_t*)SETTINGS_EEPROM_BEGIN;
  else
    ptr += SETTINGS_SIZE;
    
  // Save settings to EEPROM
  eeprom_write_block(&settings, ptr, SETTINGS_SIZE);
}

/*
 * Reset settings to default
 */
void settings_reset()
{
  uint8_t* ptr;

/*  
  //Clear complete EEPROM area.
  for (ptr=(uint8_t*)SETTINGS_EEPROM_BEGIN; ptr<(uint8_t*)SETTINGS_EEPROM_END; ++ptr)
    eeprom_write_byte(ptr, 0);
*/
  if (settings.id == 0) {
    ptr = (uint8_t*)SETTINGS_EEPROM_BEGIN;
  } else {
    for (ptr=(uint8_t*)SETTINGS_EEPROM_BEGIN; ptr<(uint8_t*)SETTINGS_EEPROM_END; ptr+=SETTINGS_SIZE)
      if (settings.id-1 == eeprom_read_byte(ptr)) break;
    if (ptr >= (uint8_t*)SETTINGS_EEPROM_END - SETTINGS_SIZE)
      ptr = (uint8_t*)SETTINGS_EEPROM_BEGIN;
    else
      ptr += SETTINGS_SIZE;
  }  
  
  settings.mode = 0;
  settings.speed = 5;
  settings.color = 0;
  settings.gain = 225;
  settings.density = 4;
  settings.sat = 240;
  settings.val = 255;
  settings.fixed_hue = 0;
  
  eeprom_write_block(&settings, ptr, SETTINGS_SIZE);  
}