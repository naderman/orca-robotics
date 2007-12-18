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

// typedef enum
// {
//     NO_STEREO_DECODING,
//     STEREO_DECODING
// } stereo_decoding_t;

namespace orcaimage{

// UYVY <-> YUYV
void
uyvy2yuyv (unsigned char *src, unsigned char *dest, int NumPixels);

void
yuyv2uyvy (unsigned char *src, unsigned char *dest, int NumPixels);

// XXX -> UYVY
void
uyyvyy2uyvy (unsigned char *src, unsigned char *dest, int NumPixels);

void
uyv2uyvy (unsigned char *src, unsigned char *dest, int NumPixels);

void
y2uyvy (unsigned char *src, unsigned char *dest, int NumPixels);

void
y162uyvy (unsigned char *src, unsigned char *dest, int NumPixels, int bits);

void
y162y (unsigned char *src, unsigned char *dest, int NumPixels, int bits);

void
rgb2uyvy (unsigned char *src, unsigned char *dest, int NumPixels);

void
rgb482uyvy (unsigned char *src, unsigned char *dest, int NumPixels);

// XXX -> RGB
void
rgb482rgb (unsigned char *src, unsigned char *dest, int NumPixels);

void
uyv2rgb (unsigned char *src, unsigned char *dest, int NumPixels);

void
uyvy2rgb (unsigned char *src, unsigned char *dest, int NumPixels);

void
uyvy2bgr (unsigned char *src, unsigned char *dest, int NumPixels);

void
uyyvyy2rgb (unsigned char *src, unsigned char *dest, int NumPixels);

void
y2rgb (unsigned char *src, unsigned char *dest, int NumPixels);

void
y162rgb (unsigned char *src, unsigned char *dest, int NumPixels, int bits);

// void
// StereoDecode(unsigned char *src, unsigned char *dest, int NumPixels);

void
uyv2bgr (unsigned char *src, unsigned char *dest, int NumPixels);

void
uyyvyy2bgr (unsigned char *src, unsigned char *dest, int NumPixels);

// MISC

void
rgb2bgr (unsigned char *src, unsigned char *dest, int NumPixels);

void 
y2bgr (unsigned char *src, unsigned char *dest, int NumPixels);

}// namespace

#endif
