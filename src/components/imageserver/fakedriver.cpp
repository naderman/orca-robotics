/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <stdlib.h>
#include <orca/camera.h>
#include <orcaice/orcaice.h>

#include "fakedriver.h"

using namespace std;

namespace imageserver {

FakeDriver::FakeDriver( const Config & cfg, const orcaice::Context & context )
    : Driver(cfg),
      context_(context)
{
}

FakeDriver::~FakeDriver()
{
}

int
FakeDriver::init()
{ 
    return 0;
}

int 
FakeDriver::read( orca::CameraDataPtr &data )
{
    cout<<"TRACE(fakedriver.cpp): Generating fake camera data..." << endl;

    // initialise values for the camera object
    data->imageWidth = 640;
    data->imageHeight = 480;
    data->format = orca::ImageFormatModeBgr;
    data->compression = orca::ImageCompressionNone;
    cout << "TODO: resize image properly for different modes" << endl;
    int imageSize = (int)ceil( 3 * data->imageHeight * data->imageWidth );
    data->image.resize( imageSize );

    bool random_data = true;
    if ( random_data )
    {
        // fill the image with a random colour
        int xr=rand()%256, xg=rand()%256, xb=rand()%256;
        fill( data->image, (unsigned char)xr, (unsigned char)xg, (unsigned char)xb );

        // cout << "opencv uses BGR format rather than rgb: " << endl; 
        cout<<"B:G:R  ("<<xr<<":"<<xg<<":"<<xb<<")"<<endl;
    }
    else 
    {
        // use a real image from file

//         ifstream file;
//         cout << "    TODO(fakedriver.cpp): this image should be loaded in from a .def file" << endl;
//         string filename = "/opt/empty-project-0.0.1/images/penguin.jpg";
//         file.open(filename.c_str());
//         if ( file.is_open() )
//         {
//             // if file exists close it and load into cv structure
//             file.close();
//             cvImage_ = cvLoadImage( filename.c_str()  );
//             //cout << "image loaded... " << endl;
//         }
//         else 
//         {
//             cout << "ERROR(fakedriver.cpp): \""<< filename << "\""<< " does not exist." << endl;
//             exit(1);
//         }

//         // copy image data into ice object
//         memcpy( &data->image[0], cvImage_->imageData, cvImage_->imageSize );

//         cout << "Image copied into ice object" << endl;
    }

    orcaice::setToNow( data->timeStamp );
        
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeDriver::fill( orca::ByteSequence& image, 
            const unsigned char R, const unsigned char G, const unsigned B )
{
    orca::ByteSequence::iterator i;
    for ( i= image.begin(); i != image.end(); i+=3 )
    {
        *i = B;
        *(i+1) = G;
        *(i+2) = R;
    }
    return 0;
}

}
