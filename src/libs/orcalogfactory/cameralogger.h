// /*
//  * Orca-Robotics Project: Components for robotics 
//  *               http://orca-robotics.sf.net/
//  * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
//  *
//  * This copy of Orca is licensed to you under the terms described in
//  * the LICENSE file included in this distribution.
//  *
//  */

// camera logging is broken. all of image stuff requires a major overhaul
#if 0

#ifndef ORCA_ORCALOGFACTORY_CAMERA_LOGGER_H
#define ORCA_ORCALOGFACTORY_CAMERA_LOGGER_H

#include <orcaice/context.h>
#include <orca/camera.h>
#include <orcalog/logger.h>

#ifdef OPENCV_FOUND
    #include <cv.h>
#endif

namespace orcalogfactory
{

class CameraLogger : public orca::CameraConsumer, public orcalog::Logger
{
public:
    CameraLogger( orcalog::LogMaster *master, 
                const std::string & typeSuffix,
                const std::string & format,
                const std::string & filenamePrefix,
                const orcaice::Context & context );
    virtual ~CameraLogger();

    // from Logger
    virtual void init();

    // reimplement from Logger because we have a custom log format
    virtual void createLogFile();

    // from *Consumer
    virtual void setData(const orca::CameraData& data, const Ice::Current&);
    
private:
    // utility function
    void writeDescription( const orca::CameraDescription& obj );
    
    // custom function to mirror ice_writeCameraData()
    void orca_writeCameraData( Ice::OutputStreamPtr outStreamPtr, const orca::CameraData& data, const std::string & filename );

    // alexm: 'data' cannot be const because we are changing it's format
    void writeCameraDataAsJpeg( const orca::CameraData& data, const std::string & filename );

    // use opencv to convert to jpg
   #ifdef OPENCV_FOUND
       	IplImage* cvImage_;
		//alen: directory prefix length
  #endif

	//alen: directory prefix for jpeg images
	std::string directoryPrefix_;
	int directoryPrefixLength_;
    int nChannels_;
};
} // namespace

#endif

#endif
