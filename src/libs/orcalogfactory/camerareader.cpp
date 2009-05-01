/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include <orcaobj/stringutils.h>
#include <orcaifacelog/camera.h>  // for auto-generated (ascii) to/fromStream functions
#include "logstringutils.h"

#include "camerareader.h"

#ifdef OPENCV_FOUND
    #include <highgui.h>
#endif

using namespace std;
using namespace orcalogfactory;

CameraReader::CameraReader()
{
}

CameraReader::~CameraReader()
{
}


void
CameraReader::readFromFile( std::ifstream                   *file, 
                              const std::string               &format,
                              orcaice::Context                 context,
                              orca::CameraDescriptionPtr &obj )
{        
    if ( format=="ice" )
    {
        orcalog::IceReadHelper helper( context.communicator(), file );
        ice_readCameraDescription( helper.stream_, obj );
        helper.read();
    }
    else if ( format=="jpeg" )
    {

#ifndef OPENCV_FOUND
        context.tracer().info( "Images can only be replayed in 'jpeg' format if you have OpenCV." );
        context.tracer().info( "Please have a look at the documentation for installing OpenCV." );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Logger: 'jpeg' format not supported because OpenCV is not installed." );
#endif
        obj = new orca::CameraDescription;

        std::string line;
        std::getline( *file, line );

        std::stringstream ss( line );
        fromLogString( ss, *obj );

    }
    else
    {
        stringstream ss;
        ss <<  "can't handle format: " << format;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }
}

void
CameraReader::readFromFile( std::ifstream     *file, 
                              const std::string &format,
                              orcaice::Context   context,
                              orca::ImageDataPtr   &obj )
{        
    if ( format=="ice" )
    {
        orcalog::IceReadHelper helper( context.communicator(), file );
        ice_readImageData( helper.stream_, obj );
        helper.read();
    }
      else if ( format=="jpeg" )
        {

#ifndef OPENCV_FOUND
            context.tracer().info( "Images can only be replayed in 'jpeg' format if you have OpenCV." );
            context.tracer().info( "Please have a look at the documentation for installing OpenCV." );
            throw orcalog::FormatNotSupportedException( ERROR_INFO, "Logger: 'jpeg' format not supported because OpenCV is not installed." );
#endif
            obj = new orca::ImageData;
            std::string line;
            std::getline( *file, line );

            std::stringstream ss( line );

            fromLogString( ss, *obj );
            std::string filename;
            ss >> filename; 
            
            loadJpegData( filename, *obj );
        }    
    else
    {
        stringstream ss;
        ss <<  "can't handle format: " << format;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }
}

void 
CameraReader::loadJpegData( const std::string &filename,
                            orca::ImageData   &obj )
{
    
    #ifdef OPENCV_FOUND
    {
        //TODO: Number of channels not yet known from image
        //previous version of ORCA had the utility to detect the number of channels
        //int nChannels_ = orcaimage::numChannels( data_.format );
        int nChannels =3;

        // load compressed image into opencv struct
        IplImage* cvImage = cvLoadImage( filename.c_str(), -1);
        
        // resize object buffer to fit image
        int imageSize = (int)ceil( nChannels * obj.description->height * obj.description->width );
        obj.pixelData.resize( imageSize );
        
        // load image from opencv struct to orca object
        memcpy( &obj.pixelData[0], cvImage->imageData, obj.pixelData.size() );

        cvReleaseImage( &cvImage );
    }
    #endif
}




