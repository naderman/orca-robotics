// /*
//  * Orca-Robotics Project: Components for robotics 
//  *               http://orca-robotics.sf.net/
//  * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic, Michael Waren
//  *
//  * This copy of Orca is licensed to you under the terms described in
//  * the LICENSE file included in this distribution.
//  *
//  */

#ifndef ORCA_ORCALOGFACTORY_MULTICAMERA_WRITER_H
#define ORCA_ORCALOGFACTORY_MULTICAMERA_WRITER_H

#include <orcaice/context.h>
#include <orca/multicamera.h>
#include <orcalog/logwriter.h>
#include <vector>
#include "logstringutils.h"
#ifdef OPENCV_FOUND
    #include <cv.h>
    #include <highgui.h>
#endif

#define FOURCC_RAWRGB 0x00000000

namespace orcalogfactory
{

class MultiCameraWriter
{
public:
    MultiCameraWriter();
    virtual ~MultiCameraWriter();

    //initialize the image logger, allocate memory for the OpenCV image
    void initMultiImageLogWriter(const orca::MultiCameraDescriptionPtr &descr);

    //initialize the video logger, allocate OpenCV video writers
    void initVideoLogWriter(const orca::MultiCameraDescriptionPtr &descr);

    void init(const orca::MultiCameraDescriptionPtr &descr, const std::string &format);

    //Create directory to log the data
    void createLogDirectory(const std::string & filename);

    //log description
    void logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::MultiCameraDescriptionPtr &obj);

    //log data
    void logToFile( std::ofstream *file, const std::string &format, orcaice::Context context, const orca::MultiCameraDataPtr &obj);
    
private:

    //! OpenCV pads byte arrays for machine efficiency (see IplImage::widthstep). 
    //! Orca does not.
    //! If image width is a multiple of 4, then padding does not occur and
    //! the opencv image data is identical to orca image data.
    //! If not, we have to do an inefficient copy line be line from orca data
    //! to opencv data.
    //! This variable flags if opencv has padded the array not.
    std::vector<bool> isPadded_;

    //! Number of bytes in one row of the orca image data structure 
    //! i.e. the width in number of bytes 
    std::vector<int32_t> orcaByteWidth_;

    
   #ifdef OPENCV_FOUND
   // Use OpenCV to store image data for writing to disk
   std::vector<IplImage*> cvImage_;
   std::vector<IplImage*> cvBayer_;
   // use OpenCV to write to video
   std::vector<CvVideoWriter*> cvVideoWriters;
    #endif

    //Write JPEG's    
    void writeCameraDataAsFile( const orca::MultiCameraDataPtr& data, const std::string &filename, int i);
    // Write video's
    void writeCameraDataAsVideo( const orca::MultiCameraDataPtr& data, int i);
    //Count the succesive images logged
    int dataCounter_;
    //Directory prefix where images will be saved
    std::string directoryPrefix_;
};
} // namespace

#endif
