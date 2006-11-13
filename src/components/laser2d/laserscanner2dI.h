/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_LASERSCANNER2D_I_H
#define ORCA2_LASER2D_LASERSCANNER2D_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/laserscanner2d.h>

// utilities
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>

namespace laser2d {

//
// Implements the Laser interface: Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//
class LaserScanner2dI : public virtual orca::LaserScanner2d
{
public:
    LaserScanner2dI( orca::RangeScanner2dGeometryPtr geometry,
                     const std::string              &ifaceTag,
                     const orcaice::Context         &context );

    //
    // Remote calls:
    //

    // Get RangeScanner Data
    virtual ::orca::RangeScanner2dDataPtr     getData(const ::Ice::Current& ) const;

    // Get Laser Config
    virtual ::orca::RangeScanner2dConfigPtr   getConfig(const ::Ice::Current& ) const;

    // Get Laser Geometry
    virtual ::orca::RangeScanner2dGeometryPtr getGeometry(const ::Ice::Current& ) const;

    // Subscribe people
    virtual void subscribe(const ::orca::RangeScanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::RangeScanner2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    //
    // Local calls:
    //
    void localSetData( const ::orca::LaserScanner2dDataPtr data );

    // We get the current config from here.
    orcaice::PtrBuffer<orca::RangeScanner2dConfigPtr> currentConfigBuffer_;

private:

    // the driver will put the latest data into this buffer
    orcaice::PtrBuffer<orca::LaserScanner2dDataPtr> dataBuffer_;

    orca::RangeScanner2dGeometryPtr  geometry_;

    // The topic to which we'll publish
    IceStorm::TopicPrx             topicPrx_;
    // The interface to which we'll publish
    orca::RangeScanner2dConsumerPrx  consumerPrx_;

    std::string                    ifaceTag_;
    orcaice::Context               context_;
};

} // namespace

#endif
