// /*
//  * Orca-Robotics Project: Components for robotics 
//  *               http://orca-robotics.sf.net/
//  * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic
//  *
//  * This copy of Orca is licensed to you under the terms described in
//  * the LICENSE file included in this distribution.
//  *
//  */

#ifndef ORCA_ORCALOGFACTORY_CAMERA_WRITER_H
#define ORCA_ORCALOGFACTORY_CAMERA_WRITER_H

#include <orcaice/context.h>
#include <orca/camera.h>
#include <orcalog/logwriter.h>
#include "logstringutils.h"
#ifdef OPENCV_FOUND
    #include <cv.h>
#endif

namespace orcalogfactory
{

class CameraWriter
{
public:
    CameraWriter();
    virtual ~CameraWriter();

    //initialize the JPEG logger, allocate memory for the OpenCV image
    void initJpegLogWriter(const orca::CameraDescriptionPtr &descr);

    //Create directory to log the data
    void createLogDirectory(const std::string & filename);

    //log description
    void logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::CameraDescriptionPtr &obj);

    //log data
    void logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::ImageDataPtr &obj);
    
private:

    // use opencv to convert to jpg
   #ifdef OPENCV_FOUND
       	IplImage* cvImage_;
    #endif

    //Write JPEG's    
    void writeCameraDataAsJpeg( const orca::ImageDataPtr& data, const std::string & filename );

    //Count the succesive images logged
    int dataCounter_;
    //Image Size as specified by camera description
    int imageSize_;
    //Directory prefix where images will be saved
    std::string directoryPrefix_;

};
} // namespace

#endif
