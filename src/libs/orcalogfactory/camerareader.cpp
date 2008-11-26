/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include <orcaobj/stringutils.h>
#include <orcaifacelog/orcaifacelog.h>  // catch-all for auto-generated (ascii) to/fromStream functions
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
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: 'jpeg' format not supported because OpenCV is not installed." );
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
                              orca::CameraDataPtr   &obj )
{        
    if ( format=="ice" )
    {
        orcalog::IceReadHelper helper( context.communicator(), file );
        ice_readCameraData( helper.stream_, obj );
        helper.read();
    }
      else if ( format=="jpeg" )
        {

#ifndef OPENCV_FOUND
            context.tracer().info( "Images can only be replayed in 'jpeg' format if you have OpenCV." );
            context.tracer().info( "Please have a look at the documentation for installing OpenCV." );
            throw orcalog::FormatNotSupportedException( ERROR_INFO, "Logger: 'jpeg' format not supported because OpenCV is not installed." );
#endif
            obj = new orca::CameraData;
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
                            orca::CameraData   &obj )
{
    
    #ifdef OPENCV_FOUND
    {
        //Number of channels not yet known from image
        //int nChannels_ = orcaimage::numChannels( data_.format );
        int nChannels =3;

        // load compressed image into opencv struct
        IplImage* cvImage = cvLoadImage( filename.c_str(), -1);
        
        // resize object buffer to fit image
        int imageSize = (int)ceil( nChannels * obj.description->height * obj.description->width );
        obj.data.resize( imageSize );
        
        // load image from opencv struct to orca object
        memcpy( &obj.data[0], cvImage->imageData, obj.data.size() );

        cvReleaseImage( &cvImage );
    }
    #endif
}


/*
void 
CameraReader::loadDataJpeg( int index )
{
    while (index != (dataCounter_) )
    {
        char id;
        file_->read( (char*)&id, sizeof(char) );

        size_t length;
        file_->read( (char*)&length, sizeof(size_t) );
        // checkFile();

        if ( length==0 ) {
            throw orcalog::FileException( ERROR_INFO, "attempt to read object of size 0" );
        }
    
        checkFile();

//         cout << "TRACE(camerareplayer.cpp): before resize, length: " << length << endl;
        std::vector<Ice::Byte> byteData;
        byteData.resize( length );
        file_->read( (char*)&byteData[0], length );

        if ( byteData.empty() ) {
            throw orcalog::FileException( ERROR_INFO, "read object of size 0" );
        }
    
        checkFile();

        // If we're skipping through many frames, just read the one we're interested in.
        if ( dataCounter_ == (index-1) )
        {
            Ice::InputStreamPtr iceInputStreamPtr = Ice::createInputStream( context_.communicator(),
                                                                            byteData );

            // TODO: Make this so that it an option is read from the logged master file
            // for reading back as an ice binary file or as separate images        
            // ice_readCameraData( iceInputStreamPtr, data_ );
            orca_readCameraData( iceInputStreamPtr, index );
        }

        dataCounter_++;
    }
}
*/


/*


void 
CameraReader::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    if (format_=="ice") {
        loadDataIce( index );
    }
    else if (format_=="jpeg") {
        loadDataJpeg( index );
    }
    else {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Unknown format: "+format_ );
    }

    // push to buffer for direct remote access
    dataPipe_.set( data_ );

    if ( !isTest ) 
    {   
        // push to IceStorm
        publisher_ -> setData( data_ );
    }
}

*/

/*
void 
CameraReader::orca_readCameraData( Ice::InputStreamPtr iceInputStreamPtr, int index )
{
    // User defined demarshalling as we can't use the ice demarshalling as it expects the actual
    // image to be in the object whereas the image has been saved separately by the logger
    data_.imageWidth = iceInputStreamPtr->readInt();
    data_.imageHeight = iceInputStreamPtr->readInt();
    data_.format = (orca::ImageFormat)iceInputStreamPtr->readInt();
    double t = iceInputStreamPtr->readDouble();
    data_.timeStamp = orcaice::toOrcaTime( t );
    std::string fname = iceInputStreamPtr->readString();
    
    #ifdef OPENCV_FOUND
    {
        nChannels_ = orcaimage::numChannels( data_.format );

        // load compressed image into opencv struct
        cvImage_ = cvLoadImage( fname.c_str(), -1);
        
        // resize object buffer to fit image
        int imageSize = (int)ceil( nChannels_ * data_.imageHeight * data_.imageWidth );
        data_.image.resize( imageSize );
        
        // load image from opencv struct to orca object
        memcpy( &data_.image[0], cvImage_->imageData, data_.image.size() );

        cvReleaseImage( &cvImage_ );
    }
    #endif
}
*/       
