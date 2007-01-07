/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <iomanip>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include <orcaimage/colourconversions.h>
#include <orcaimage/imageutils.h>

#include "cameralogger.h"

#ifdef OPENCV_FOUND
    #include <opencv/cv.h>
    #include <opencv/highgui.h>
#endif

using namespace std;
using namespace orcalogfactory;


CameraLogger::CameraLogger( orcalog::LogMaster *master, 
                        const std::string & typeSuffix,
                        const std::string & format,
                        const std::string & filenamePrefix,
                        const orcaice::Context & context )
    : orcalog::Logger( master, 
             "Camera",
             "Camera"+typeSuffix,
             format,
             filenamePrefix+"camera"+typeSuffix+".log",
             context )
{
#ifndef OPENCV_FOUND
    // check that we support this format
    if ( format_=="ice" )
    {
        // this one is supported
    }
    else if ( format_=="jpeg" )
    {
        context_.tracer()->info( "Images can only be logged in 'jpeg' format if you have OpenCV." );
        context_.tracer()->info( "Please have a look at the documentation for ImageServer component for installing OpenCv." );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: 'jpeg' format not supported because OpenCV is not installed." );
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
#endif


#ifdef OPENCV_FOUND
    // check that we support this format
    if ( format_!="ice" && format_!="jpeg" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
#endif
}

CameraLogger::~CameraLogger()
{
}

// reimplement from Logger because we have a custom log format
void
CameraLogger::createLogFile()
{
    //
    // create log file, may throw and it will kill us
    //
    if ( format_ == "ice" ) {
        createBinaryFile();
    }
    else if ( format_ == "jpeg" ) {
        createBinaryFile();
    }
}

void 
CameraLogger::init()
{   
    // throws orcaice::NetworkException but it's ok to quit here
    orca::CameraPrx objectPrx;
    orcaice::connectToInterfaceWithTag<orca::CameraPrx>( context_, objectPrx, interfaceTag_ );
    
    // get description. should do try/catch
    orca::CameraDescription obj = objectPrx->getDescription();
    writeDescription( obj );

    // consumer
    Ice::ObjectPtr consumer = this;
    orca::CameraConsumerPrx callbackPrx = orcaice::createConsumerInterface<orca::CameraConsumerPrx>( context_, consumer );

    context_.tracer()->debug("Subscribing to IceStorm now.",5);
    objectPrx->subscribe( callbackPrx );
}

void 
CameraLogger::setData(const orca::CameraData& data, const Ice::Current&)
{
    // Write reference to master file
    appendMasterFile( data.timeStamp.seconds, data.timeStamp.useconds );

    if ( format_ == "ice" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeCameraData( helper.stream_, data );
        helper.write( file_ );
    }
    else if ( format_ == "jpeg" )
    {        
        // image filename (different file for each image)
        std::ostringstream filename;
        filename << "image" << std::setw(5) << std::setfill('0') << dataCounter_ << ".jpg";

        // write object meta data into a binary file
        orcalog::IceWriteHelper helper( context_.communicator() );
        this->orca_writeCameraData( helper.stream_, data, filename.str() );
        helper.write( file_ );

        // the image itself is saved as jpeg into a separate file
// ##########################################
//
// alexm: 'data' cannot be const because we are changing it's format
//
//         writeCameraDataAsJpeg( data, filename.str() );
// ##########################################
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
}

void 
CameraLogger::writeDescription( const orca::CameraDescription& obj )
{
    context_.tracer()->debug( "Writing description to file", 5 );
    
    if ( format_ == "ice" || format_ == "jpeg" )
    {
        orcalog::IceWriteHelper helper( context_.communicator() );
        ice_writeCameraDescription( helper.stream_, obj );
        helper.write( file_ );  
    }
    else
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }
} 


// Write image object information and image filename to our personal file 
// (this is the file that the master file looks at)
// Note: need to do our own marshalling as we don't want the whole object in the personal file
void 
CameraLogger::orca_writeCameraData( Ice::OutputStreamPtr outStreamPtr, 
        const orca::CameraData& data, const std::string & filename )
{
    outStreamPtr->writeInt(data.imageWidth);
    outStreamPtr->writeInt(data.imageHeight);
    outStreamPtr->writeInt(data.format);
    outStreamPtr->writeInt(data.compression);
    outStreamPtr->writeDouble(orcaice::timeAsDouble(data.timeStamp));
    outStreamPtr->writeString( filename );
}    

// Write image to file after compressing to jpeg format using opencv
void 
CameraLogger::writeCameraDataAsJpeg( orca::CameraData& data, const std::string & filename )
{
#ifdef OPENCV_FOUND
    IplImage* cvImage_ = 0;
    
    if ( dataCounter_ == 1 )
    {    
        nChannels_ = orcaimage::numChannels( data.format );
        
        // setup opencv image
        cvImage_  = cvCreateImage( cvSize( data.imageWidth, data.imageHeight ),  8, nChannels_ );
    }
    
    // cout << "Image Format: " << orcaimage::formatName( data.format ) << endl;
    
    // copy image in orca object into opencv struct
    // and make sure the image is BGR format before compression
    // cout << "nChannels_: " << nChannels_ << endl;       
    if ( nChannels_ == 3 )
    {       
        orcaimage::cvtToBgr( cvImage_, cvImage_, data );
        data.format = orca::ImageFormatModeBgr;
    }
    else
    {
        memcpy( cvImage_->imageData, &data.image[0], data.image.size() );
    }                     
    
    // Save image to file
    // This assumes that the file has been stored in bgr format (if the image is colour) which is 
    // the default for opencv
    cvSaveImage( filename.c_str(), cvImage_ );
#endif

}     
