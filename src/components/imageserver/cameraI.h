/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_SICKLASER_LASER_I_H
#define ORCA2_SICKLASER_LASER_I_H

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
    CameraI( orca::CameraGeometryPtr geometry,
            orcaice::Context              context );

    //
    // Remote calls:
    //

    // Get Camera Data
    virtual ::orca::CameraDataPtr     getData(const ::Ice::Current& ) const;

    // Get Camera Config
    virtual ::orca::CameraConfigPtr   getConfig(const ::Ice::Current& ) const;

    // Get Camera Geometry
    virtual ::orca::CameraGeometryPtr getGeometry(const ::Ice::Current& ) const;

    // Set Camera Config
    virtual void setConfig(const ::orca::CameraConfigPtr &config, const ::Ice::Current& );

    // Subscribe people
    virtual void subscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::CameraConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    //
    // Local calls:
    //
    void localSetData( const ::orca::CameraDataPtr data );

    void localSetCurrentConfig( const ::orca::CameraConfigPtr config )
        { currentConfigBuffer_.push( config ); }
    void localSetDesiredConfig( const ::orca::CameraConfigPtr config )
        { desiredConfigBuffer_.push( config ); }

    // We put incoming config requests into this buffer
    orcaice::PtrBuffer<orca::CameraConfigPtr> desiredConfigBuffer_;

    // We get the current config from here.
    orcaice::PtrBuffer<orca::CameraConfigPtr> currentConfigBuffer_;

private:

    // the driver will put the latest data into this buffer
    orcaice::PtrBuffer<orca::CameraDataPtr>          dataBuffer_;

    orca::CameraGeometryPtr  geometry_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::CameraConsumerPrx  consumerPrx_;

    orcaice::Context context_;
};

}

#endif
