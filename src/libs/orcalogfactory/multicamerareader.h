/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Alen Alempijevic, Michael Warren
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCALOGFACTORY_MULTI_CAMERA_READER_H
#define ORCA_ORCALOGFACTORY_MULTI_CAMERA_READER_H

#include <orcalog/logreader.h>
#include <orca/multicamera.h>



#include <map>
#include <vector>

#ifdef OPENCV_FOUND
    #include <cv.h>
    #include <highgui.h>
#endif

namespace orcalogfactory
{

class MultiCameraReader
{
public:
    MultiCameraReader();
    virtual ~MultiCameraReader();

    void initDataStorage();
  
    void readFromFile(  std::ifstream *file, 
                        const std::string &format,
                        orcaice::Context context,
                        orca::MultiCameraDescriptionPtr &obj );

    void readFromFile(  std::ifstream     *file, 
                        const std::string &format,
                        orcaice::Context   context,
                        orca::MultiCameraDataPtr &obj );

    void loadImageData( std::stringstream &ss,
                       orca::MultiCameraDataPtr& obj );

    void loadVideoData( std::stringstream &ss,
                       orca::MultiCameraDataPtr& obj );
private:

    // The MultiCamera Description
    orca::MultiCameraDescriptionPtr descr_;

    // The Multicamera Data
    orca::MultiCameraDataPtr data_;

    //std::vector<std::string> videoFilenames;
    #ifdef OPENCV_FOUND
        std::vector<IplImage*> cvImage_;
        std::map<std::string,CvCapture*> cvCapture_;
        //std::vector<CvCapture*> cvCapture_;
    #endif

};

} // namespace
#endif
