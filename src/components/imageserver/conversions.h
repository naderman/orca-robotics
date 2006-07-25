/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGE_SERVER_CONVERSIONS_H
#define ORCA2_IMAGE_SERVER_CONVERSIONS_H

#include <orca/camera.h>
//#include <libdc1394/dc1394_control.h>

namespace imageserver {

    // enumerations taken from libdc1394/dc1394_control.h

/* Enumeration of camera framerates */
    enum 
    {
        FRAMERATE_1_875= 32,
        FRAMERATE_3_75,
        FRAMERATE_7_5,
        FRAMERATE_15,
        FRAMERATE_30,
        FRAMERATE_60,
        FRAMERATE_120,
        FRAMERATE_240
    };
#define FRAMERATE_MIN               FRAMERATE_1_875
#define FRAMERATE_MAX               FRAMERATE_240
#define NUM_FRAMERATES              (FRAMERATE_MAX - FRAMERATE_MIN + 1)

/* Enumeration of camera modes for Format_0 */
    enum 
    {
        MODE_160x120_YUV444= 64,
        MODE_320x240_YUV422,
        MODE_640x480_YUV411,
        MODE_640x480_YUV422,
        MODE_640x480_RGB,
        MODE_640x480_MONO,
        MODE_640x480_MONO16
    };
#define MODE_FORMAT0_MIN	    MODE_160x120_YUV444
#define MODE_FORMAT0_MAX	    MODE_640x480_MONO16
#define NUM_FORMAT0_MODES	    (MODE_FORMAT0_MAX - MODE_FORMAT0_MIN + 1)

/* Enumeration of camera modes for Format_1 */
    enum 
    {
        MODE_800x600_YUV422= 96,
        MODE_800x600_RGB,
        MODE_800x600_MONO,
        MODE_1024x768_YUV422,
        MODE_1024x768_RGB,
        MODE_1024x768_MONO,
        MODE_800x600_MONO16,
        MODE_1024x768_MONO16
    };
#define MODE_FORMAT1_MIN	    MODE_800x600_YUV422
#define MODE_FORMAT1_MAX	    MODE_1024x768_MONO16
#define NUM_FORMAT1_MODES	    (MODE_FORMAT1_MAX - MODE_FORMAT1_MIN + 1)


/* Enumeration of camera modes for Format_2 */
    enum 
    {
        MODE_1280x960_YUV422= 128,
        MODE_1280x960_RGB,
        MODE_1280x960_MONO,
        MODE_1600x1200_YUV422,
        MODE_1600x1200_RGB,
        MODE_1600x1200_MONO,
        MODE_1280x960_MONO16,
        MODE_1600x1200_MONO16
    };
#define MODE_FORMAT2_MIN	    MODE_1280x960_YUV422
#define MODE_FORMAT2_MAX	    MODE_1600x1200_MONO16
#define NUM_FORMAT2_MODES	    (MODE_FORMAT2_MAX - MODE_FORMAT2_MIN + 1)

/* Enumeration of camera modes for Format_6 */
    enum 
    {
        MODE_EXIF= 256
    };
#define MODE_FORMAT6_MIN            MODE_EXIF
#define MODE_FORMAT6_MAX            MODE_EXIF
#define NUM_FORMAT6_MODES           (MODE_FORMAT6_MAX - MODE_FORMAT6_MIN + 1)

/* Enumeration of camera modes for Format_7 */
    enum {
        MODE_FORMAT7_0= 288,
        MODE_FORMAT7_1,
        MODE_FORMAT7_2,
        MODE_FORMAT7_3,
        MODE_FORMAT7_4,
        MODE_FORMAT7_5,
        MODE_FORMAT7_6,
        MODE_FORMAT7_7
    };
#define MODE_FORMAT7_MIN            MODE_FORMAT7_0
#define MODE_FORMAT7_MAX            MODE_FORMAT7_7
#define NUM_MODE_FORMAT7            (MODE_FORMAT7_MAX - MODE_FORMAT7_MIN + 1)

/* Enumeration of Format_7 color modes */
    enum {
        COLOR_FORMAT7_MONO8= 320,
        COLOR_FORMAT7_YUV411,
        COLOR_FORMAT7_YUV422,
        COLOR_FORMAT7_YUV444,
        COLOR_FORMAT7_RGB8,
        COLOR_FORMAT7_MONO16,
        COLOR_FORMAT7_RGB16,
        COLOR_FORMAT7_MONO16S,
        COLOR_FORMAT7_RGB16S,
        COLOR_FORMAT7_RAW8,
        COLOR_FORMAT7_RAW16
    };
#define COLOR_FORMAT7_MIN           COLOR_FORMAT7_MONO8
#define COLOR_FORMAT7_MAX           COLOR_FORMAT7_RAW16
#define NUM_COLOR_FORMAT7           (COLOR_FORMAT7_MAX - COLOR_FORMAT7_MIN + 1)



    // convert from dc_1394 colour mode to orca colour mode 
    orca::ImageFormat orcaImageMode( int mode );

}

#endif
