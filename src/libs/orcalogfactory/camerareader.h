/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOGFACTORY_CAMERA_READER_H
#define ORCA_ORCALOGFACTORY_CAMERA_READER_H

#include <orcalog/logreader.h>
#include <orca/camera.h>

#ifdef OPENCV_FOUND
    #include <cv.h>
#endif

namespace orcalogfactory
{

class CameraReader
{
public:
    CameraReader();
    virtual ~CameraReader();

    void readFromFile(  std::ifstream *file, 
                        const std::string &format,
                        orcaice::Context context,
                        orca::CameraDescriptionPtr &obj );

    void readFromFile(  std::ifstream     *file, 
                        const std::string &format,
                        orcaice::Context   context,
                        orca::ImageDataPtr   &obj );

    void loadJpegData( const std::string &format,
                       orca::ImageData   &obj );

    
private:

    #ifdef OPENCV_FOUND
        IplImage* cvImage_;

    #endif

    //Number of Channels in image
    int numOfChannels_;
    //Directory prefix where images have been saved
    std::string directoryPrefix_;
    //Frame number
    int dataCounter_;

};

} // namespace
#endif
