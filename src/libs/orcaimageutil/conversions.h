/*
 * Copyright (C) 2000-2001 Dan Dennedy  <dan@dennedy.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef ORCA_IMAGE_CONVERSIONS_H
#define ORCA_IMAGE_CONVERSIONS_H

#include <string.h>


namespace orcaimageutil {

// UYVY <-> YUYV
void
uyvy2yuyv (uint8_t *src, uint8_t *dest, uint32_t size);

void
yuyv2uyvy (uint8_t *src, uint8_t *dest, uint32_t size);

// XXX -> UYVY
void
uyyvyy2uyvy (uint8_t *src, uint8_t *dest, uint32_t size);

void
uyv2uyvy (uint8_t *src, uint8_t *dest, uint32_t size);

void
y2uyvy (uint8_t *src, uint8_t *dest, uint32_t size);

void
y162uyvy (uint8_t *src, uint8_t *dest, uint32_t size, int bits);

void
y162y (uint8_t *src, uint8_t *dest, uint32_t size, int bits);

void
rgb2uyvy (uint8_t *src, uint8_t *dest, uint32_t size);

void
rgb482uyvy (uint8_t *src, uint8_t *dest, uint32_t size);

// XXX -> RGB
void
rgb482rgb (uint8_t *src, uint8_t *dest, uint32_t size);

void
uyv2rgb (uint8_t *src, uint8_t *dest, uint32_t size);

void
uyvy2rgb (uint8_t *src, uint8_t *dest, uint32_t size);

void
uyvy2bgr (uint8_t *src, uint8_t *dest, uint32_t size);

void
uyyvyy2rgb (uint8_t *src, uint8_t *dest, uint32_t size);

void
y2rgb (uint8_t *src, uint8_t *dest, uint32_t size);

void
y162rgb (uint8_t *src, uint8_t *dest, uint32_t size, int bits);

void
uyv2bgr (uint8_t *src, uint8_t *dest, uint32_t size);

void
uyyvyy2bgr (uint8_t *src, uint8_t *dest, uint32_t size);

// MISC

void
rgb2bgr (uint8_t *src, uint8_t *dest, uint32_t size);

void 
y2bgr (uint8_t *src, uint8_t *dest, uint32_t size);

/**
 * TODO
 */
/*
void
bgr2bgra (uint8_t *src, uint8_t *dest, uint32_t size);

void
bgr2argb (uint8_t *src, uint8_t *dest, uint32_t size);

void
bayer2bgr(uint8_t *src, uint8_t *dest, uint32_t size, BayerFormat bayer);

void
bayer2bgra(uint8_t *src, uint8_t *dest, uint32_t size, BayerFormat bayer);
*/

}// namespace

#endif
