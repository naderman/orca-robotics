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
//#include "cameraI.h"
#include <orca/camera.h>
#include <orcaice/orcaice.h>

// #include <libcamera/camera.hpp>
#include "imagegrabber/cvgrabber.h"

#include <iostream>
#include <stdlib.h>


using namespace std;

namespace imageserver {

MonoFireWireDriver::MonoFireWireDriver( CvGrabber* cvGrabber, orcaice::Context context )
    : imageGrabber_(cvGrabber)
      //requestedImageMode_( LC_IMAGE_MODE_DONTCARE ),
      //requestedImageMode_( LC_IMAGE_MODE_RGB ),
      //actualImageMode_( (requestedImageMode_==LC_IMAGE_MODE_DONTCARE)?imageGrabber_->getDefaultImageMode():requestedImageMode_ )
//      image_( new orca::Image( imageGrabber->getWidth(), imageGrabber->getHeight(), orcaImageMode(actualImageMode_) ) ),

{

}

MonoFireWireDriver::~MonoFireWireDriver()
{
    disable();
}

// orca::ImageFormat
// MonoFireWireDriver::orcaImageMode( LibcameraImageMode mode )
// {
//     orca::ImageFormat f;

//     switch( mode )
//     {
//     case LC_IMAGE_MODE_GRAY:
//         f = orca::MODEGRAY;
//         break;
//     case LC_IMAGE_MODE_YUV422:
//         f = orca::MODEYUV422;
//         break;
//     case LC_IMAGE_MODE_BGR:
//         f = orca::MODEBGR;
//         break;
//     case LC_IMAGE_MODE_RGB:
//         f = orca::MODERGB;
//         break;
//     case LC_IMAGE_MODE_NFI:
//     case LC_IMAGE_MODE_DONTCARE:
//     default:
//         f = orca::MODENFI;
//         break;
//     }
    
//     return f;
// }

int 
MonoFireWireDriver::read( orca::CameraDataPtr &data )
{
    cout<<"TRACE(monofirewiredriver.cpp): Grabbing camera data..." << endl;

    // LibcameraImageMode modeDone;
    timeval ts;
    bool firstImage = false;
    forceClearBuffer_ = false;

    orcaice::setToNow( data->timeStamp );
    
    // initialise values for the camera object
    // should this be done in the constructor?
//     int imageWidth;
//     imageGrabber_->getWidth( imageWidth );
//     data->imageWidth = imageWidth;
//     imageGrabber_->getHeight( data->imageHeight );
//    data->format = orcaImageMode(actualImageMode_);
//    imageGrabber_->getFormat( data->format );
//     data->compression = orca::COMPRESSIONTYPENONE;
//     cout << "TODO: resize image properly for different modes" << endl;
//     int imageSize = (int)ceil( 3 * data->imageHeight * data->imageWidth );
//     data->image.resize( imageSize );

//     cout << "imageWidth: " << data->imageWidth << endl;
//     cout << "imageHeight: " << data->imageHeight << endl;
//     cout << "imageSize: " << data->image.size() << endl;

    double gain = 500;
    imageGrabber_->setGain( gain );

    // for some reason opencv grabber doesn't get the correct height or
    // width so load it into a iplimage and read off the configuration
    // details.
    imageGrabber_->queryFrame();
    data->imageWidth = imageGrabber_->frame_->width;
    data->imageHeight = imageGrabber_->frame_->height;
    cout << "TODO: resize image properly for different modes" << endl;
    // for colour images
    int imageSize = (int)ceil( 3 * data->imageHeight * data->imageWidth );
    // for mono images
//    int imageSize = (int)ceil( 1 * data->imageHeight * data->imageWidth );
    data->image.resize( imageSize );

    cout << "imageWidth: " << data->imageWidth << endl;
    cout << "imageHeight: " << data->imageHeight << endl;
    cout << "imageSize: " << data->image.size() << endl;


    memcpy( &data->image[0], imageGrabber_->frame_->imageData, imageSize );
    // Slow it down a bit if we're reading from disk.
//     if ( imageGrabber_->isSavedImageGrabber() )
//     {
//         usleep(50000);
//     }

    // this will block and set the time stamp as well
    //cout << "before grab image " << endl;
    //imageGrabber_->getImage( (char*)(&data->image[0]), actualImageMode_, &ts, &modeDone );
//    cout << "image mode: " << actualImageMode_ << endl;


//     if ( imageGrabber_->getImage( (char*)(&data->image[0]), actualImageMode_, &ts, &modeDone ) )
//     {

//         if ( !imageGrabber_->isSavedImageGrabber() && forceClearBuffer_ )
//         {
//             // Get a few more to clear the buffer
//             cout << "before grab 3 images " << endl;
//             for ( int i=0; i < 3; i++ )
//             {
//                 imageGrabber_->getImage( (char*)(&data->image[0]), actualImageMode_, &ts, &modeDone );
//             }
//             cout << "after grab 3 images " << endl;
//         }
//         cout << "imagesize: " << data->image.size() << endl;
//         assert( actualImageMode_ == modeDone );

// //         if ( jpegCompression_ )
// //         {
// //             image_->setCompressTypeOnTransmit( Image::COMPRESSIONTYPEJPEG );
// //         }
// //         else
// //         {
// //             image_->setCompressTypeOnTransmit( Image::COMPRESSIONTYPENONE );
// //         }

//         data->compression = orca::COMPRESSIONTYPENONE;
//         //cout << "after buffer clear " << endl;
//         if ( firstImage )
//         {
//             cout<<"TRACE(monofirewiredriver.cpp): Got first image." << endl;
// //            cout<<"TRACE(monofirewiredriver.cpp): " << *image_ << endl;
// //            cout<<"TRACE(imageservercontainer.cpp): compression type: " 
// //                << Image::compressTypeName(image_->compressTypeOnTransmit()) << endl;
		
//             firstImage = false;
//         }

//         // push it out the door
//         cout << "TODO(monofirewiredriver.cpp): are the time stamps right?" << endl;
// //        data->setTimeStamp( ts.tv_sec, ts.tv_usec );
// //        port_->putData( *image_ );
           
	    
//         if ( imageGrabber_->isSavedImageGrabber() )
//         {
//             usleep(50000);
//         }
//     }
//     else
//     {
//         cout<<"ImageServer: Couldn't grab image -- exitting." << endl;
//         exit(1);
//     }

//    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
MonoFireWireDriver::setConfig( const orca::CameraConfigPtr &cfg )
{
    isEnabled_ = cfg->isEnabled;
    return 0;
}

int 
MonoFireWireDriver::getConfig( orca::CameraConfigPtr &cfg )
{
    return 0;
}

int 
MonoFireWireDriver::fill( orca::ByteSequence& image, const unsigned char R, const unsigned char G, const unsigned B )
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
