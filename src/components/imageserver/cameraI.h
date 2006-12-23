/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_IMAGESERVER_CAMERA_I_H
#define ORCA2_IMAGESERVER_CAMERA_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/camera.h>

// utilities
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

namespace imageserver {

//
// Implements the image server interface: Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//
class CameraI : public virtual orca::Camera
{
public:
    CameraI( const orca::CameraDescription& descr,
             const std::string                &ifaceTag,
             const orcaice::Context           & context );

    //
    // From orca object
    //

    virtual ::orca::CameraData getData(const ::Ice::Current& ) const;

    virtual ::orca::CameraDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    //
    // Local calls:
    //
    void localSetData( const ::orca::CameraData& data );

private:

    // the driver will put the latest data into this buffer
    orcaice::Buffer<orca::CameraData>          dataPipe_;

    orca::CameraDescription  descr_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::CameraConsumerPrx  consumerPrx_;

    std::string                    ifaceTag_;
    orcaice::Context               context_;
};

}

#endif
