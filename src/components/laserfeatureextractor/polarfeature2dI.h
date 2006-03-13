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
