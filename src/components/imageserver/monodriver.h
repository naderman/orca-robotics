/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
