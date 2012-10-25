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
 * effects.c
 *
 * Created: 21.10.2012 18:07:32
 *  Author: Julian Iseringhausen <julian.iseringhausen@gmail.com>
 *
 *  Description: Various graphical effects.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdint.h>

#include <avr/pgmspace.h>
#include <util/delay.h>

#include "avrsnippets/helper.h"
#include "avrsnippets/random/random.h"
#include "bitmap.h"
#include "coord.h"
#include "effects.h"
#include "fade.h"
#include "hsv.h"
#include "rgb.h"
#include "settings.h"

coord_t pos[SIZE_X*SIZE_Y];
rgb_t r[16], g[16], b[16];
hsv_t hue[2];
uint8_t buffer[8];
uint16_t buffer_16[2];

uint8_t mode_changed = 0;
uint16_t wait = 0;

static inline uint8_t scale_16(uint8_t val)
{
  return (val>settings.gain ? 16 : ((32*val)/settings.gain+1)/2);
}

static inline uint8_t scale_256(uint8_t val)
{
  return (val>settings.gain ? 255 : (255*val)/settings.gain);
}

static void (* const effect_ptr[]) (void) PROGMEM =
{
  EffectShade,
  EffectRain,
  EffectStars,
  EffectShadeDark
};

#define EFFECT_INIT(STR) \
  if (mode_changed == 0) { \
    STR \
    mode_changed = 1; \
  }
  
#define SPEED_INIT(BUF, MS_2_5) \
  do { \
    (BUF) = MS_2_5 * speed; \
  }while(0)
  
#define SPEED_UPDATE(BUF, MS_2_5) \
  do { \
    if (--(BUF) != 0) return EFFECT_GS_NOUPDATE; \
    (BUF) = MS_2_5 * speed; \
  }while(0)

#define random_pos() (random_shift_8(SIZE_X_BITS+SIZE_Y_BITS) & (SIZE_X*SIZE_Y-1))
#define random_pos_x() (random_shift_8(SIZE_X_BITS) & (SIZE_X-1))
#define random_pos_y() (random_shift_8(SIZE_Y_BITS) & (SIZE_Y-1))

void effect_handle()
{
  if (wait == 0) ((void(*)(void))pgm_read_word(&effect_ptr[settings.mode]))();
  else --wait;
}

void EffectStars()
{
  hsv_t hsv_temp;
  uint8_t x,y;
  
  EFFECT_INIT(
    buffer[0] = 0;
    hue[0] = hsv_random();
    bitmap_clear();
  )
  
  wait = 100*settings.speed;
  
  switch(buffer[0])
  {
  case 0:
      pos[0] = random_pos();
    
    if (settings.fixed_hue == 0) {
      hue[0] = hsv_random_new(hue[0]);
    } else {
      hue[0].h = random_variate(settings.color, 0, 191, 4) << 3;
      hue[0].s = random_variate(settings.sat, 0, 255, 5);
      hue[0].v = random_variate(settings.val, 0, 255, 5);
    }    
    
    hsv_temp = hue[0]; hsv_temp.v = 0;
    fade_add(FADE_MODE_SMOOTH, pos[0], FADE_DIRECT, hsv_temp, hue[0], 100*settings.speed);
    break;
  case 1:
    x = get_pos_x(pos[0]);
    y = get_pos_y(pos[0]);
    hsv_temp = hue[0]; hsv_temp.v = 0;
    if (x < SIZE_X-1) fade_add(FADE_MODE_SMOOTH, get_pos(x+1,y), FADE_DIRECT, hsv_temp, hue[0], 100*settings.speed);
    if (x > 0)        fade_add(FADE_MODE_SMOOTH, get_pos(x-1,y), FADE_DIRECT, hsv_temp, hue[0], 100*settings.speed);
    if (y < SIZE_Y-1) fade_add(FADE_MODE_SMOOTH, get_pos(x,y+1), FADE_DIRECT, hsv_temp, hue[0], 100*settings.speed);
    if (y > 0)        fade_add(FADE_MODE_SMOOTH, get_pos(x,y-1), FADE_DIRECT, hsv_temp, hue[0], 100*settings.speed);
    break;
  case 2:
    hsv_temp = hue[0]; hsv_temp.v = 0;
    fade_add(FADE_MODE_SMOOTH, pos[0], FADE_DIRECT, hue[0], hsv_temp, 100*settings.speed);
    break;
  case 3:
    x = get_pos_x(pos[0]);
    y = get_pos_y(pos[0]);
    hsv_temp = hue[0]; hsv_temp.v = 0;
    if (x < 15) fade_add(FADE_MODE_SMOOTH, get_pos(x+1,y), FADE_DIRECT, hue[0], hsv_temp, 100*settings.speed);
    if (x > 0)  fade_add(FADE_MODE_SMOOTH, get_pos(x-1,y), FADE_DIRECT, hue[0], hsv_temp, 100*settings.speed);
    if (y < 3)  fade_add(FADE_MODE_SMOOTH, get_pos(x,y+1), FADE_DIRECT, hue[0], hsv_temp, 100*settings.speed);
    if (y > 0)  fade_add(FADE_MODE_SMOOTH, get_pos(x,y-1), FADE_DIRECT, hue[0], hsv_temp, 100*settings.speed);
    break;
  default:
    break;
  }
  
  if (++buffer[0] == 4) buffer[0] = 0;;
}

void EffectRain()
{
  EFFECT_INIT(
    for (uint8_t i=0; i<settings.density; ++i) {
      pos[i+0*settings.density] = get_pos(0,3);
      pos[i+1*settings.density] = get_pos(0,2);
      pos[i+2*settings.density] = get_pos(0,1);
      pos[i+3*settings.density] = get_pos(0,0);
    }
  )
  
  wait = 8 * settings.speed;
  
  bitmap_clear();

  for (uint8_t i=0; i<4*settings.density; ++i) {

    bitmap_set(pos[i], r[i], g[i], b[i]);

    if (get_pos_y(pos[i]) == 0) {
      if (settings.fixed_hue == 0) {
        hue[0] = hsv_random();
      } else {
        hue[0].h = random_variate(settings.color, 0, 191, 3) << 3;
        hue[0].s = random_variate(settings.sat, 0, 255, 3);
        hue[0].v = random_variate(settings.val, 0, 255, 3);
      }      
      hsv_to_rgb(hue[0], &r[i], &g[i], &b[i]);
      pos[i] = get_pos(random_pos_x(), 4);
    }

    pos[i] = get_pos(get_pos_x(pos[i]), get_pos_y(pos[i])-1);

  }
}

static void permutate()
{
  for (uint8_t i=0; i<2*SIZE_X*SIZE_Y; ++i) {
    uint8_t permutation_1 = random_pos();
    uint8_t permutation_2 = random_pos();
    uint8_t tmp = pos[permutation_1];
    pos[permutation_1] = pos[permutation_2];
    pos[permutation_2] = tmp;
  }
}

void EffectShade()
{
  EFFECT_INIT(
    for (uint8_t i=0; i<SIZE_X*SIZE_Y; ++i)
      pos[i] = i;
    permutate();
    buffer[0] = 0;
    if (settings.fixed_hue == 0) {
      hue[0] = hsv_random();
    } else {
      hue[0].h = random_variate(settings.color, 0, 191, 4) << 3;
      hue[0].s = random_variate(settings.sat, 0, 255, 5);
      hue[0].v = max(random_shift_8(8), settings.val);
    }    
    hue[1] = hue[0]; hue[1].v = 0;
    hsv_to_rgb(hue[0], &r[0], &g[0], &b[0]);
    r[1]=0; g[1]=0; b[1]=0;
  )
  
  fade_add(FADE_MODE_SMOOTH, pos[buffer[0]], FADE_DIRECT, hue[1], hue[0], 192*settings.speed);
  wait = 24 * settings.speed;

  for (uint8_t i=0; i<buffer[0]; ++i)
    bitmap_set(pos[i], r[0], g[0], b[0]);

  for (uint8_t i=buffer[0]; i<64; ++i)
    bitmap_set(pos[i], r[1], g[1], b[1]);
 
  if (++buffer[0] >= SIZE_X*SIZE_Y) {
    permutate();
    buffer[0] = 0;
    r[1]=r[0]; g[1]=g[0]; b[1]=b[0];
    hue[1] = hue[0];
    if (settings.fixed_hue == 0) {
      hue[0] = hsv_random_new(hue[0]);
    } else {
      hue[0].h = random_variate(settings.color, 0, 191, 4) << 3;
      hue[0].s = random_variate(settings.sat, 0, 255, 5);
      hue[0].v = max(random_shift_8(8), settings.val);
    }    
    hsv_to_rgb(hue[0], &r[0], &g[0], &b[0]);
  }
}

void EffectShadeDark()
{
  EFFECT_INIT(
    for (uint8_t i=0; i<2*settings.density; ++i)
      pos[i] = 0;
    buffer[0] = 0;
  )
  
  bitmap_clear();
  
  wait = (192/settings.density) * settings.speed;
  
  fade_add(FADE_MODE_SMOOTH_REVERSE, pos[(buffer[0]+settings.density)%(2*settings.density)], FADE_DIRECT, hue[1], hue[0], 192*settings.speed);

  uint8_t is_new;
  do {
    pos[buffer[0]] = random_pos();
    is_new = 1;
    for (uint8_t i=0; i<2*settings.density; ++i)
      is_new &= (pos[i] != pos[buffer[0]] || i == buffer[0] ? 1 : 0);
    }while (is_new == 0);

  if (settings.fixed_hue == 0) {
    hue[0] = hsv_random();
  } else {
    hue[0].h = random_variate(settings.color, 0, 191, 4) << 3;
    hue[0].s = random_variate(settings.sat, 0, 255, 5);
    hue[0].v = random_variate(settings.val, 0, 255, 5);  
  }  
  
  hue[1] = hue[0]; hue[1].v = 0;
  fade_add(FADE_MODE_SMOOTH, pos[buffer[0]], FADE_DIRECT, hue[1], hue[0], 192*settings.speed);
 
 if (++buffer[0] == 2*settings.density) buffer[0] = 0;
}