/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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
    : Driver(cfg, context),
      context_(context)
{
     context_.tracer()->info( "Initializing fake driver with config: "+config_.toString() );

//      if ( cfg.format != orca::ImageFormatModeBgr ) {
//          throw "Unsupported image format. Only 'ModeBgr' is supported";
//      }
}

FakeDriver::~FakeDriver()
{
}

int
FakeDriver::init()
{ 
    config_.imageWidth = 640;
    config_.imageHeight = 480;
    context_.tracer()->debug("TODO(fakedriver::init()): resize image properly for different modes", 3);
    config_.imageSize = 3 * config_.imageHeight * config_.imageWidth;
  
    config_.format = orca::ImageFormatModeBgr;
    config_.compression = orca::ImageCompressionNone;
    context_.tracer()->info( "FakeDriver configuration has now been initialised to: "+config_.toString() );
    
    return 0;
}

int 
FakeDriver::read( orca::CameraData& data )
{
    context_.tracer()->debug( "Generating fake image data...", 6 );

    // fill the image with a random colour
    int rr = rand()%256;
    int gg = rand()%256;
    int bb = rand()%256;
    flatColor( data.image, (unsigned char)rr, (unsigned char)gg, (unsigned char)bb );

//     randomColor( data->image );

    // use a real image from file
//     fromFile( data->image, "/opt/empty-project-0.0.1/images/penguin.jpg" );

    orcaice::setToNow( data.timeStamp );
        
    // we are controlling frame rate by sleeping
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
    return 0;
}

int 
FakeDriver::flatColor( orca::ByteSequence& image, 
            unsigned char R, unsigned char G, unsigned char B )
{
    // cout << "opencv uses BGR format rather than rgb: " << endl;
    std::stringstream infoString;
    infoString << "B:G:R  (" << (int)B << ":" << (int)G << ":" << (int)R << ")";
    context_.tracer()->info( infoString.str() );

    orca::ByteSequence::iterator i;
    for ( i= image.begin(); i != image.end(); i+=3 )
    {
        *i = B;
        *(i+1) = G;
        *(i+2) = R;
    }
    return 0;
}

int 
FakeDriver::randomColor( orca::ByteSequence& image )
{
    orca::ByteSequence::iterator i;
    for ( i= image.begin(); i+2 != image.end(); i+=3 )
    {
        *i = (unsigned char)rand()%256;
        *(i+1) = (unsigned char)rand()%256;
        *(i+2) = (unsigned char)rand()%256;
    }
    return 0;
}

int 
FakeDriver::fromFile( orca::ByteSequence& image, const std::string & filename )
{
//     ifstream file;
//     cout << "    TODO(fakedriver.cpp): this image should be loaded in from a .def file" << endl;
//     file.open(filename.c_str());
// 
//     if ( file.is_open() )
//     {
//         // if file exists close it and load into cv structure
//         file.close();
//         cvImage_ = cvLoadImage( filename.c_str()  );
//         //cout << "image loaded... " << endl;
//     }
//     else 
//     {
//         cout << "ERROR(fakedriver.cpp): \""<< filename << "\""<< " does not exist." << endl;
//         exit(1);
//     }
//     
//     // copy image data into ice object
//     memcpy( &data->image[0], cvImage_->imageData, cvImage_->imageSize );
//     
//     cout << "Image copied into ice object" << endl;
    return 0;
}

} // namespace
