/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGESERVER_MONO_DRIVER_H
#define ORCA2_IMAGESERVER_MONO_DRIVER_H

#include <orcaice/context.h>
#include "driver.h"
#include "imagegrabber/imagegrabber.h"

// #include "imagegrabber/cvgrabber.h"
// #include "imagegrabber/digiclopsgrabber.h"
// #include <highgui.h>
// #include <cv.h>

namespace imageserver {

//
// Monocular camera driver using opencv imagegrabber.
//
class MonoDriver : public Driver
{

public:

    MonoDriver( ImageGrabber* imageGrabber, 
                const Config & cfg, const orcaice::Context & context );
    virtual ~MonoDriver();

    virtual int init();

    // Blocks till new data is available
    virtual int read( orca::CameraData& data );

private:
    ImageGrabber* imageGrabber_;

    orcaice::Context context_;

};

}

#endif
