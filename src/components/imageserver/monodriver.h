/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef MONO_DRIVER_H
#define MONO_DRIVER_H

#include "driver.h"
#include "imagegrabber/imagegrabber.h"
// #include "imagegrabber/cvgrabber.h"
// #include "imagegrabber/digiclopsgrabber.h"

#include <orcaice/context.h>
#include <orca/camera.h>

// #include <opencv/highgui.h>
// #include <opencv/cv.h>

#include <iostream>

namespace imageserver {

//
// Monocular camera driver using opencv imagegrabber.
//
class MonoDriver : public Driver
{

public:

//    MonoDriver( CvGrabber* cvGrabber, orcaice::Context context );
    MonoDriver( ImageGrabber* imageGrabber, orcaice::Context context );
    virtual ~MonoDriver();

    virtual int enable() { isEnabled_=true; return 0; };
    virtual int disable() { isEnabled_=false; return 0; };

    virtual bool isEnabled() { return isEnabled_; };

    // Blocks till new data is available
    virtual int read( orca::CameraDataPtr &data );

    // Set a specific configuration
    virtual int setConfig( const orca::CameraConfigPtr &cfg );

    // Get the current configuration
    virtual int getConfig( orca::CameraConfigPtr &cfg );

private:

    bool isEnabled_;

    ImageGrabber* imageGrabber_;

};

}

#endif
