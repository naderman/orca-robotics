/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_POLAR_FEATURE_2D_I_H
#define ORCA2_POLAR_FEATURE_2D_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>
#include <orcaice/ptrbuffer.h>

// include provided interfaces
#include <orca/polarfeature2d.h>

namespace laserfeatures
{

//
// Implements the Polarfeature2d interface: Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//
class PolarFeature2dI : public orca::PolarFeature2d
{
public:
    PolarFeature2dI( orcaice::PtrBuffer<orca::PolarFeature2dDataPtr> & polarFeatureDataBuffer,
                     const IceStorm::TopicPrx & topicPrx );

    // remote calls:

    // Get Polarfeature2dSequence Data
    virtual ::orca::PolarFeature2dDataPtr getData(const ::Ice::Current& ) const;
    
    // Subscribe people
    virtual void subscribe(const ::orca::PolarFeature2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::PolarFeature2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());



private:

    // the driver will put the latest data into this buffer
    orcaice::PtrBuffer<orca::PolarFeature2dDataPtr> & polarFeatureDataBuffer_;
    
    const IceStorm::TopicPrx topicPrx_;
};

} // namespace

#endif
