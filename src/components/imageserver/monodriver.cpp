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

#include "monodriver.h"
#include "imagegrabber/imagegrabber.h"

#include <orca/camera.h>
#include <orcaice/orcaice.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

namespace imageserver {

MonoDriver::MonoDriver( ImageGrabber* imageGrabber, orcaice::Context context )
    : imageGrabber_(imageGrabber)
{

}

MonoDriver::~MonoDriver()
{
    // cvReleaseImage( &cvImage_ );
    disable();
}

int 
MonoDriver::read( orca::CameraDataPtr &data )
{
    cout<<"TRACE(monodriver.cpp): Grabbing camera data..." << endl;
    
    //grab the image and load it into the orca object
    char* rawImage = imageGrabber_->queryFrame();
    if ( rawImage == NULL )
    {
        cout << "ERROR(monodriver.cpp): Unable to get image from image grabber" << endl;
        exit(1);
    }
    else
    {
        // set the time stamp as soon as we get the image from the imagegrabber.
        // this is inexact... is there a better way?
        orcaice::setToNow( data->timeStamp );

        // size of the image (data->image.size()) was defined in mainloop.cpp
        memcpy( &data->image[0], rawImage, data->image.size() );
        data->compression = orca::COMPRESSIONTYPENONE;

        return 0;
    }
}

int 
MonoDriver::setConfig( const orca::CameraConfigPtr &cfg )
{
    isEnabled_ = cfg->isEnabled;
    return 0;
}

int 
MonoDriver::getConfig( orca::CameraConfigPtr &cfg )
{
    return 0;
}


}
