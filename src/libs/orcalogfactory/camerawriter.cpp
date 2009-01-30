// /*
//  * Orca-Robotics Project: Components for robotics 
//  *               http://orca-robotics.sf.net/
//  * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic
//  *
//  * This copy of Orca is licensed to you under the terms described in
//  * the LICENSE file included in this distribution.
//  *
//  */

#include <iostream>
#include <iomanip>
#include <fstream>

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>
#include <orcaifacelog/orcaifacelog.h>
#include "camerawriter.h"

#include <orcaobj/stringutils.h>


#ifdef OPENCV_FOUND
	#include <highgui.h>
#endif

//alen - currently missing conversion utilities
//#include <orcaimage/colourconversions.h>
//#include <orcaimage/imageutils.h>

//alen - Added the below to create a directory to save images
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;
using namespace orcalogfactory;


CameraWriter::CameraWriter()
{
#ifdef OPENCV_FOUND
    // Initialise the iplimage pointer to NULL.
    cvImage_ = 0;
    dataCounter_=1;
#endif
}

CameraWriter::~CameraWriter()
{
}

void
CameraWriter::initJpegLogWriter(const orca::CameraDescriptionPtr &descr)
{
    // setup opencv image
    // alen HACK current assumptions:
    // depth of 8, no utility available to obtain depth
    // no utility available to obtain number of channels
    // number of channels 3 for RGB8 and BGR8,
    // number of channels 1 for MONO8 and RAW8
    // otherwise throw an exemption
#ifdef OPENCV_FOUND
    int numOfChannels;
    if ( (descr->format == "RGB8") || (descr->format == "BGR8")){
        numOfChannels=3;
    }
    else if ( (descr->format == "RAW8") || (descr->format == "MONO8")){
        numOfChannels=1;
    }
    else{
        stringstream ss;
        ss << "Camera LogWriter 'jpeg' " << descr->format ;
        ss << " Only BGR8/RGB8/RAW8/MONO8 logging supported." ;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }

    cvImage_  = cvCreateImage( cvSize( descr->width, descr->height ),  8, numOfChannels );
    imageSize_=descr->width*descr->height*numOfChannels;
#endif
}

void
CameraWriter::createLogDirectory(const std::string & filename)
{
    int directoryPrefixLength = filename.length();
    //Removing the _cameraX.log (total of 12 characters) will give the directory name 
    directoryPrefix_= filename.substr(0,directoryPrefixLength-12);
    int ret= mkdir(directoryPrefix_.c_str(),0755);
    if (ret != 0){	
        stringstream ss;
        ss << "Camera LogRecorder 'jpeg' : Could not create directory " << directoryPrefix_;
        throw orcalog::FileException( ERROR_INFO, ss.str() );
    }
}

void 
CameraWriter::logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::CameraDescriptionPtr &obj)
{

    if ( format=="ice" ){
        orcalog::IceWriteHelper helper( context.communicator() );
        ice_writeCameraDescription( helper.stream_, obj );
        helper.write( file );  
    }
    else if ( format=="jpeg" ){
#ifndef OPENCV_FOUND
        context.tracer().info( "Images can only be logged in 'jpeg' format if you have OpenCV." );
        context.tracer().info( "Please have a look at the documentation for ImageServer component for installing OpenCV." );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "LogWriter: 'jpeg' format not supported because OpenCV is not installed." );
#else
        initJpegLogWriter(obj);
#endif
        //Log the Camera Description to an ascii file nevertheless
        (*file) << toLogString(obj) << endl;
    }
    else{
        stringstream ss;
        ss << "can't handle format: " << format;
        throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    }

}

void 
CameraWriter::logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::ImageDataPtr &obj)
{

        if ( format == "ice" ){
            orcalog::IceWriteHelper helper( context.communicator() );
            ice_writeImageData( helper.stream_, obj );
            helper.write( file );
        }
        else if ( format=="jpeg" ){

#ifndef OPENCV_FOUND
            context.tracer().info( "Images can only be logged in 'jpeg' format if you have OpenCV." );
            context.tracer().info( "Please have a look at the documentation for ImageServer component for installing OpenCV." );
            throw orcalog::FormatNotSupportedException( ERROR_INFO, "Logger: 'jpeg' format not supported because OpenCV is not installed." );
#endif
            // image filename (different file for each image)
            std::stringstream filename;
            filename << ".//" << directoryPrefix_ << "//";
            filename << "image" << std::setw(5) << std::setfill('0') << dataCounter_++ << ".jpg";

            // write object meta data into a ascii file
            (*file) << toLogString(obj) << endl;
            // the image itself is saved as jpeg into a separate file
            writeCameraDataAsJpeg( obj, filename.str() );
        }
        else{
            stringstream ss;
            ss << "can't handle format: " << format;
            throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
        }
}

// Write image to file after compressing to jpeg format using opencv
void 
CameraWriter::writeCameraDataAsJpeg( const orca::ImageDataPtr& data, const std::string & filename )
{
#ifdef OPENCV_FOUND
    // copy image in orca object into opencv struct
    // and make sure the image is BGR format before compression
    //(if the image is colour) which is the default for opencv

    //if (data->description->format != "BGR8"){
    //    orcaimage::cvtToBgr( cvImage_, cvImage_, data );
    //    stringstream ss;
    //    ss << "Camera LogWriter 'jpeg' " << data->description->format << " Only BGR8 logging supported.";
    //    throw orcalog::FormatNotSupportedException( ERROR_INFO, ss.str() );
    //}

    memcpy( cvImage_->imageData, &(data->pixelData[0]), imageSize_ );

    cvSaveImage( filename.c_str(), cvImage_ );
#endif

}     

