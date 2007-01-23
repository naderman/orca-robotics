/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_CAMERA_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_CAMERA_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/camera.h>
#include <orcaice/ptrbuffer.h>
#include <orcalog/replayer.h>

#ifdef OPENCV_FOUND
    #include <cv.h>
#endif

namespace orcalogfactory
{

class CameraReplayer : public orca::Camera, public orcalog::Replayer
{
public:
    CameraReplayer( const std::string      &format,
                   const std::string      &filename,
                   const orcaice::Context &context );
    virtual ~CameraReplayer();
    
    // from Orca object

    virtual ::orca::CameraData getData(const ::Ice::Current& ) const;

    virtual ::orca::CameraDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Buffer<orca::CameraData> dataPipe_;
    // alexm: this pipe is not necessary
    orcaice::Buffer<orca::CameraDescription> descrPipe_;
    
    orca::CameraData data_;
    orca::CameraConsumerPrx publisher_;
    IceStorm::TopicPrx topic_;

    virtual void initInterfaces();
    virtual void initDescription();
    
    // binary file protocol specifics
    void loadHeaderIce();
    void loadDataIce( int index );
    void loadDataJpeg( int index );

    // special case: custom function to replace ice_readCameraData
    void orca_readCameraData( Ice::InputStreamPtr iceInputStreamPtr , int index);

    // use opencv to convert to jpg
    #ifdef OPENCV_FOUND
        IplImage* cvImage_;
        int nChannels_;
    #endif

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
