/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
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

namespace imageviewocv{

/**********************************************************************
*
*  CONVERSION FUNCTIONS TO UYVY 
*
**********************************************************************/

void
yuyv2uyvy(unsigned char *src, unsigned char *dest, int NumPixels) {
#ifdef YUYV
//  swab(src, dest, NumPixels << 1);
#else
//  memcpy(dest,src, NumPixels<<1);
#endif
}

void
uyvy2yuyv(unsigned char *src, unsigned char *dest, int NumPixels) {
#ifdef YUYV
//  swab(src, dest, NumPixels << 1);
#else
//  memcpy(dest,src, NumPixels<<1);
#endif
}

void
uyyvyy2uyvy (unsigned char *src, unsigned char *dest, int NumPixels) {
register int i=NumPixels + (NumPixels >> 1)-1;
register int j=(NumPixels << 1)-1;
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
uyv2uyvy (unsigned char *src, unsigned char *dest, int NumPixels) {
                    register int i = NumPixels + (NumPixels << 1)-1;
                    register int j = (NumPixels << 1)-1;
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
y2uyvy (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i= NumPixels-1;
    register int j = (NumPixels << 1)-1;
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
y162uyvy (unsigned char *src, unsigned char *dest, int NumPixels, int bits) {
    register int i = (NumPixels << 1)-1;
    register int j = (NumPixels << 1)-1;
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
y162y (unsigned char *src, unsigned char *dest, int NumPixels, int bits) {
    register int i = (NumPixels<<1)-1;
    register int j = NumPixels-1;
    register int y;

    while (i > 0)
    {
        y = src[i--];
        dest[j--] = (y + (src[i--]<<8))>>(bits-8);
    }
}

void
rgb2uyvy (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i = NumPixels + ( NumPixels << 1 )-1;
    register int j = (NumPixels << 1)-1;
    register int y0, y1, u0, u1, v0, v1 ;
    register int r, g, b;

    while (i > 0)
    {
        b = (unsigned char) src[i--];
        g = (unsigned char) src[i--];
        r = (unsigned char) src[i--];
        RGB2YUV (r, g, b, y0, u0 , v0);
        b = (unsigned char) src[i--];
        g = (unsigned char) src[i--];
        r = (unsigned char) src[i--];
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
rgb482uyvy (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i = ( (NumPixels + ( NumPixels << 1 )) << 1 ) -1;
    register int j = (NumPixels << 1)-1;
    register int y0, y1, u0, u1, v0, v1 ;
    register int r, g, b;

    while (i > 0)
    {
        i--;
        b = (unsigned char) src[i--];
        i--;
        g = (unsigned char) src[i--];
        i--;
        r = (unsigned char) src[i--];
        i--;
        RGB2YUV (r, g, b, y0, u0 , v0);
        b = (unsigned char) src[i--];
        i--;
        g = (unsigned char) src[i--];
        i--;
        r = (unsigned char) src[i--];
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
rgb482rgb (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i = ((NumPixels + ( NumPixels << 1 )) << 1)-1;
    register int j = NumPixels + ( NumPixels << 1 ) -1;

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
        uyv2rgb (unsigned char *src, unsigned char *dest, int NumPixels)
{
    register int i = (NumPixels << 1)-1;
    register int j = NumPixels + ( NumPixels << 1 ) -1;
    register int y, u, v;
    register int r, g, b;

    while (i > 0)
    {
        v = (unsigned char) src[i--] - 128;
        y = (unsigned char) src[i--];
        u = (unsigned char) src[i--] - 128;
        YUV2RGB (y, u, v, r, g, b);
        dest[j--] = b;
        dest[j--] = g;
        dest[j--] = r;
    
    }
}

void
        uyv2bgr (unsigned char *src, unsigned char *dest, int NumPixels)
{
    register int i = (NumPixels << 1)-1;
    register int j = NumPixels + ( NumPixels << 1 ) -1;
    register int y, u, v;
    register int r, g, b;

    while (i > 0)
    {
        v = (unsigned char) src[i--] - 128;
        y = (unsigned char) src[i--];
        u = (unsigned char) src[i--] - 128;
        YUV2RGB (y, u, v, r, g, b);
        dest[j--] = r;
        dest[j--] = g;
        dest[j--] = b;
    
    }
}

void
        uyvy2rgb (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i = (NumPixels << 1)-1;
    register int j = NumPixels + ( NumPixels << 1 ) -1;
    register int y0, y1, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y1 = (unsigned char) src[i--];
        v  = (unsigned char) src[i--] - 128;
        y0 = (unsigned char) src[i--];
        u  = (unsigned char) src[i--] - 128;
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
        uyvy2bgr (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i = (NumPixels << 1)-1;
    register int j = NumPixels + ( NumPixels << 1 ) -1;
    register int y0, y1, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y1 = (unsigned char) src[i--];
        v  = (unsigned char) src[i--] - 128;
        y0 = (unsigned char) src[i--];
        u  = (unsigned char) src[i--] - 128;
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
uyyvyy2rgb (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i = NumPixels + ( NumPixels >> 1 )-1;
    register int j = NumPixels + ( NumPixels << 1 )-1;
    register int y0, y1, y2, y3, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y3 = (unsigned char) src[i--];
        y2 = (unsigned char) src[i--];
        v  = (unsigned char) src[i--] - 128;
        y1 = (unsigned char) src[i--];
        y0 = (unsigned char) src[i--];
        u  = (unsigned char) src[i--] - 128;
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
uyyvyy2bgr (unsigned char *src, unsigned char *dest, int NumPixels) {
    register int i = NumPixels + ( NumPixels >> 1 )-1;
    register int j = NumPixels + ( NumPixels << 1 )-1;
    register int y0, y1, y2, y3, u, v;
    register int r, g, b;

    while (i > 0)
    {
        y3 = (unsigned char) src[i--];
        y2 = (unsigned char) src[i--];
        v  = (unsigned char) src[i--] - 128;
        y1 = (unsigned char) src[i--];
        y0 = (unsigned char) src[i--];
        u  = (unsigned char) src[i--] - 128;
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
y2rgb (unsigned char *src, unsigned char *dest, int NumPixels) 
{
    register int i = NumPixels-1;
    register int j = NumPixels + ( NumPixels << 1 )-1;
    register int y;

    while (i > 0)
    {
        y = (unsigned char) src[i--];
        dest[j--] = y;
        dest[j--] = y;
        dest[j--] = y;
    }
}

void
y162rgb (unsigned char *src, unsigned char *dest, int NumPixels, int bits) {
    register int i = (NumPixels << 1)-1;
    register int j = NumPixels + ( NumPixels << 1 )-1;
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
rgb2bgr (unsigned char *src, unsigned char *dest, int NumPixels)
{
    register int i = 0;

    for( ; i < NumPixels*3; i+=3 )
    {
        dest[i] = src[i+2];
        dest[i+1] = src[i+1];
        dest[i+2] = src[i];
    }
}

void 
y2bgr (unsigned char *src, unsigned char *dest, int NumPixels)
{
    y2rgb(src, dest, NumPixels); //is the same as image is grayscale
}

} // namespace
