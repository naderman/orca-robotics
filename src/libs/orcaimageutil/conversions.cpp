/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
/*
*  Copied directly from Coriander, and added conversions to bgr (backwards rgb).
*
*/

#include "conversions.h"

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cassert>

//extern void swab(const void *from, void *to, ssize_t n);

// The following #define is there for the users who experience green/purple
// images in the display. This seems to be a videocard driver problem.

#define YUYV // instead of the standard UYVY

// color conversion functions from Bart Nabbe.
// corrected by Damien: bad coeficients in YUV2RGB
#define YUV2RGB(y, u, v, r, g, b)\
r = y + ((v*1436) >>10);\
g = y - ((u*352 + v*731) >> 10);\
b = y + ((u*1814) >> 10);\
r = r < 0 ? 0 : r;\
g = g < 0 ? 0 : g;\
b = b < 0 ? 0 : b;\
r = r > 255 ? 255 : r;\
g = g > 255 ? 255 : g;\
b = b > 255 ? 255 : b


#define RGB2YUV(r, g, b, y, u, v)\
y = (306*r + 601*g + 117*b)  >> 10;\
u = ((-172*r - 340*g + 512*b) >> 10)  + 128;\
v = ((512*r - 429*g - 83*b) >> 10) + 128;\
y = y < 0 ? 0 : y;\
u = u < 0 ? 0 : u;\
v = v < 0 ? 0 : v;\
y = y > 255 ? 255 : y;\
u = u > 255 ? 255 : u;\
v = v > 255 ? 255 : v

#define CLIP(in, out)\
{\
in = in < 0 ? 0 : in;\
in = in > 255 ? 255 : in;\
out=in;\
}

using namespace orcaimageutil;

/**********************************************************************
*
*  CONVERSION FUNCTIONS TO UYVY 
*
**********************************************************************/

void
yuyv2uyvy(uint8_t *src, uint8_t *dest, uint32_t size) {
#ifdef YUYV
//  swab(src, dest, size << 1);
#else
//  memcpy(dest,src, size<<1);
#endif
}

void
uyvy2yuyv(uint8_t *src, uint8_t *dest, uint32_t size) {
#ifdef YUYV
//  swab(src, dest, size << 1);
#else
//  memcpy(dest,src, size<<1);
#endif
}

void
uyyvyy2uyvy (uint8_t *src, uint8_t *dest, uint32_t size) {
register int i=size + (size >> 1)-1;
register int j=(size << 1)-1;
register int y0, y1, y2, y3, u, v;

while (i > 0)
{
    y3 = src[i--];
    y2 = src[i--];
    v  = src[i--];
    y1 = src[i--];
    y0 = src[i--];
    u  = src[i--];
#ifdef YUYV
    dest[j--] = v;
    dest[j--] = y3;
    dest[j--] = u;
    dest[j--] = y2;

    dest[j--] = v;
    dest[j--] = y1;
    dest[j--] = u;
    dest[j--] = y0;
#else // UYVY
    dest[j--] = y3;
    dest[j--] = v;
    dest[j--] = y2;
    dest[j--] = u;

    dest[j--] = y1;
    dest[j--] = v;
    dest[j--] = y0;
    dest[j--] = u;
#endif
}
}

void
uyv2uyvy (uint8_t *src, uint8_t *dest, uint32_t size) {
                    register int i = size + (size << 1)-1;
                    register int j = (size << 1)-1;
                    register int y0, y1, u0, u1, v0, v1;

while (i > 0)
{
    v1 = src[i--];
    y1 = src[i--];
    u1 = src[i--];
    v0 = src[i--];
    y0 = src[i--];
    u0 = src[i--];

#ifdef YUYV
        dest[j--] = (v0+v1) >> 1;
        dest[j--] = y1;
        dest[j--] = (u0+u1) >> 1;
        dest[j--] = y0;
#else // UYVY
        dest[j--] = y1;
        dest[j--] = (v0+v1) >> 1;
        dest[j--] = y0;
        dest[j--] = (u0+u1) >> 1;
#endif
}
}


void
y2uyvy (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i= size-1;
    register int j = (size << 1)-1;
    register int y0, y1;

    while (i > 0)
    {
        y1 = src[i--];
        y0 = src[i--];
#ifdef YUYV
        dest[j--] = 128;
        dest[j--] = y1;
        dest[j--] = 128;
        dest[j--] = y0;
#else // UYVY
        dest[j--] = y1;
        dest[j--] = 128;
        dest[j--] = y0;
        dest[j--] = 128;
#endif
    }
}

void
y162uyvy (uint8_t *src, uint8_t *dest, uint32_t size, int bits) {
    register int i = (size << 1)-1;
    register int j = (size << 1)-1;
    register int y0, y1;
    //fprintf(stderr,"bpp:%d\n",bits);
    while (i > 0)
    {
        y1 = src[i--];
        y1 = (y1 + (((int)src[i--])<<8))>>(bits-8);
        y0 = src[i--];
        y0 = (y0 + (((int)src[i--])<<8))>>(bits-8);
#ifdef YUYV
        dest[j--] = 128;
        dest[j--] = y1;
        dest[j--] = 128;
        dest[j--] = y0;
#else // UYVY
        dest[j--] = y1;
        dest[j--] = 128;
        dest[j--] = y0;
        dest[j--] = 128;
#endif
    }
}

void
y162y (uint8_t *src, uint8_t *dest, uint32_t size, int bits) {
    register int i = (size<<1)-1;
    register int j = size-1;
    register int y;

    while (i > 0)
    {
        y = src[i--];
        dest[j--] = (y + (src[i--]<<8))>>(bits-8);
    }
}

void
rgb2uyvy (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i = size + ( size << 1 )-1;
    register int j = (size << 1)-1;
    register int y0, y1, u0, u1, v0, v1 ;
    register int r, g, b;

    while (i > 0)
    {
        b = (uint8_t) src[i--];
        g = (uint8_t) src[i--];
        r = (uint8_t) src[i--];
        RGB2YUV (r, g, b, y0, u0 , v0);
        b = (uint8_t) src[i--];
        g = (uint8_t) src[i--];
        r = (uint8_t) src[i--];
        RGB2YUV (r, g, b, y1, u1 , v1);
#ifdef YUYV
        dest[j--] = (v0+v1) >> 1;
        dest[j--] = y0;
        dest[j--] = (u0+u1) >> 1;
        dest[j--] = y1;
#else // UYVY
        dest[j--] = y0;
        dest[j--] = (v0+v1) >> 1;
        dest[j--] = y1;
        dest[j--] = (u0+u1) >> 1;
#endif
    }
}

void
rgb482uyvy (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i = ( (size + ( size << 1 )) << 1 ) -1;
    register int j = (size << 1)-1;
    register int y0, y1, u0, u1, v0, v1 ;
    register int r, g, b;

    while (i > 0)
    {
        i--;
        b = (uint8_t) src[i--];
        i--;
        g = (uint8_t) src[i--];
        i--;
        r = (uint8_t) src[i--];
        i--;
        RGB2YUV (r, g, b, y0, u0 , v0);
        b = (uint8_t) src[i--];
        i--;
        g = (uint8_t) src[i--];
        i--;
        r = (uint8_t) src[i--];
        RGB2YUV (r, g, b, y1, u1 , v1);

#ifdef YUYV
        dest[j--] = (v0+v1) >> 1;
        dest[j--] = y0;
        dest[j--] = (u0+u1) >> 1;
        dest[j--] = y1;
#else // UYVY
        dest[j--] = y0;
        dest[j--] = (v0+v1) >> 1;
        dest[j--] = y1;
        dest[j--] = (u0+u1) >> 1;
#endif
    }
}

/**********************************************************************
*
*  CONVERSION FUNCTIONS TO RGB 24bpp 
*
**********************************************************************/

void
rgb482rgb (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i = ((size + ( size << 1 )) << 1)-1;
    register int j = size + ( size << 1 ) -1;

    while (i > 0)
    {
        i--;
        dest[j--]=src[i--];
        i--;
        dest[j--]=src[i--];
        i--;
        dest[j--]=src[i--];
    }
}


void
        uyv2rgb (uint8_t *src, uint8_t *dest, uint32_t size)
{
    register int i = (size << 1)-1;
    register int j = size + ( size << 1 ) -1;
    register int y, u, v;
    register int r, g, b;

    while (i > 0)
    {
        v = (uint8_t) src[i--] - 128;
        y = (uint8_t) src[i--];
        u = (uint8_t) src[i--] - 128;
        YUV2RGB (y, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
    
    }
}

void
        uyv2bgr (uint8_t *src, uint8_t *dest, uint32_t size)
{
    register int i = (size << 1)-1;
    register int j = size + ( size << 1 ) -1;
    register int y, u, v;
    register int r, g, b;

    while (i > 0)
    {
        v = (uint8_t) src[i--] - 128;
        y = (uint8_t) src[i--];
        u = (uint8_t) src[i--] - 128;
        YUV2RGB (y, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
    
    }
}

void
        uyvy2rgb (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i = (size << 1)-1;
    register int j = size + ( size << 1 ) -1;
    register int y0, y1, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y1 = (uint8_t) src[i--];
        v  = (uint8_t) src[i--] - 128;
        y0 = (uint8_t) src[i--];
        u  = (uint8_t) src[i--] - 128;
        YUV2RGB (y1, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
        YUV2RGB (y0, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
    }
        }


void
        uyvy2bgr (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i = (size << 1)-1;
    register int j = size + ( size << 1 ) -1;
    register int y0, y1, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y1 = (uint8_t) src[i--];
        v  = (uint8_t) src[i--] - 128;
        y0 = (uint8_t) src[i--];
        u  = (uint8_t) src[i--] - 128;
        YUV2RGB (y1, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
        YUV2RGB (y0, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
    }
        }


void
uyyvyy2rgb (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i = size + ( size >> 1 )-1;
    register int j = size + ( size << 1 )-1;
    register int y0, y1, y2, y3, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y3 = (uint8_t) src[i--];
        y2 = (uint8_t) src[i--];
        v  = (uint8_t) src[i--] - 128;
        y1 = (uint8_t) src[i--];
        y0 = (uint8_t) src[i--];
        u  = (uint8_t) src[i--] - 128;
        YUV2RGB (y3, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
        YUV2RGB (y2, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
        YUV2RGB (y1, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
        YUV2RGB (y0, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
    }
}

void
uyyvyy2bgr (uint8_t *src, uint8_t *dest, uint32_t size) {
    register int i = size + ( size >> 1 )-1;
    register int j = size + ( size << 1 )-1;
    register int y0, y1, y2, y3, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y3 = (uint8_t) src[i--];
        y2 = (uint8_t) src[i--];
        v  = (uint8_t) src[i--] - 128;
        y1 = (uint8_t) src[i--];
        y0 = (uint8_t) src[i--];
        u  = (uint8_t) src[i--] - 128;
        YUV2RGB (y3, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
        YUV2RGB (y2, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
        YUV2RGB (y1, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
        YUV2RGB (y0, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
    }
}

void
y2rgb (uint8_t *src, uint8_t *dest, uint32_t size) 
{
    register int i = size-1;
    register int j = size + ( size << 1 )-1;
    register int y;

    while (i > 0)
    {
        y = (uint8_t) src[i--];
        dest[j--] = y;
        dest[j--] = y;
        dest[j--] = y;
    }
}

void
y162rgb (uint8_t *src, uint8_t *dest, uint32_t size, int bits) {
    register int i = (size << 1)-1;
    register int j = size + ( size << 1 )-1;
    register int y;

    while (i > 0)
    {
    
        y = src[i--];
        y = (y + (src[i--]<<8))>>(bits-8);
        dest[j--] = y;
        dest[j--] = y;
        dest[j--] = y;
    }
}


//****************************************************************

void
rgb2bgr (uint8_t *src, uint8_t *dest, uint32_t size)
{
    register int i = 0;

    for( ; i < size*3; i+=3 )
    {
        dest[i] = src[i+2];
        dest[i+1] = src[i+1];
        dest[i+2] = src[i];
    }
}

void 
y2bgr (uint8_t *src, uint8_t *dest, uint32_t size)
{
    y2rgb(src, dest, size); //is the same as image is grayscale
}

