/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "monodriver.h"
#include "imagegrabber/imagegrabber.h"

#include <orca/camera.h>
#include <orcaice/orcaice.h>

//#include <triclops/pnmutils.h>
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

//         TriclopsInput triclopsInput;
//         triclopsInput.nrows = 768;
//         triclopsInput.ncols = 1024;
//         triclopsInput.inputType = TriInp_RGB;
//         int imageSize = triclopsInput.nrows * triclopsInput.ncols;
//         vector<char> red;
//         vector<char> green;
//         vector<char> blue;
//         red.resize(imageSize);
//         green.resize(imageSize);
//         blue.resize(imageSize);

//         // the R,G, and B values are stored in separate arrays
//         memcpy(&red[0], rawImage, imageSize );
//         memcpy(&green[0], rawImage+imageSize, imageSize );
//         memcpy(&blue[0], rawImage+imageSize*2, imageSize );

//         triclopsInput.u.rgb.red = &red[0];
//         triclopsInput.u.rgb.green = &green[0];
//         triclopsInput.u.rgb.blue = &blue[0];

//         char file[] = "test2.ppm";
//         bool res = ppmWriteFromTriclopsInput( file , &triclopsInput );
//         cout << "res: " << res << endl;

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
