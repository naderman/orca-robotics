/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_CAMERA_LOGGER_H
#define ORCA2_ORCALOGFACTORY_CAMERA_LOGGER_H

#include <orcaice/context.h>
#include <orca/camera.h>
#include <orcalog/logger.h>

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
    void writeCameraDataAsJpeg( orca::CameraData& data, const std::string & filename );

    // use opencv to convert to jpg
//         #ifdef OPENCV_FOUND
//             IplImage* cvImage_;
//         #endif
    
    int nChannels_;
};

} // namespace

#endif
