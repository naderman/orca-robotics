/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_POSITION3D_I_H
#define ORCA2_SEGWAY_POSITION3D_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/position3d.h>

// utilities
#include <orcaice/ptrproxy.h>

namespace segwayrmp
{

// serves latest data on demand and accepts commands
class Position3dI : public orca::Position3d
{
public:
    Position3dI( orcaice::PtrProxy<orca::Position3dDataPtr>    & position3dPipe,
                 const IceStorm::TopicPrx                      & topic );

    // NOTE: this implementation can throw DataNotExist exception but does NOT throw
    // HardwareFailedException because it's isolated from the hardware handler by the
    // data buffer.
    virtual ::orca::Position3dDataPtr getData(const ::Ice::Current& ) const;

    virtual ::orca::Position3dGeometryPtr getGeometry(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

private:
    // the driver will put the latest data into this proxy
    orcaice::PtrProxy<orca::Position3dDataPtr> &position3dPipe_;

    // IceStorm topic to which we send our updates and cand subscribe other to
    IceStorm::TopicPrx topic_;
};

} // namespace

#endif
