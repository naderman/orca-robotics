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

#ifndef ORCA2_SEGWAY_PLATFORM2D_I_H
#define ORCA2_SEGWAY_PLATFORM2D_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/platform2d.h>

// utilities
#include <orcaice/ptrproxy.h>
#include <orcaice/ptrnotify.h>

namespace robot2d
{

// serves latest data on demand and accepts commands
class Platform2dI : public orca::Platform2d
{
public:
    Platform2dI( orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe,
                 orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandsPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe,
                 const IceStorm::TopicPrx                      & topic );

    // NOTE: this implementation can throw DataNotExist exception but does NOT throw
    // HardwareFailedException because it's isolated from the hardware handler by the
    // data buffer.
    virtual ::orca::Position2dDataPtr getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometryPtr getGeometry(const ::Ice::Current& ) const;

    virtual ::orca::Platform2dConfigPtr getConfig(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    virtual void setCommand(const ::orca::Velocity2dCommandPtr&, const ::Ice::Current& );

    virtual void setConfig(const ::orca::Platform2dConfigPtr&, const ::Ice::Current& );

    // WARING: not implemented!
    virtual void enableMotor(bool, const ::Ice::Current& = ::Ice::Current()) {};

private:
    // the driver will put the latest data into this proxy
    orcaice::PtrProxy<orca::Position2dDataPtr> &position2dPipe_;
    // the driver will take the latest command from the proxy
    orcaice::PtrNotify<orca::Velocity2dCommandPtr> &commandPipe_;

    // for incoming requests
    orcaice::PtrProxy<orca::Platform2dConfigPtr> &setConfigPipe_;
    // for the current config
    orcaice::PtrProxy<orca::Platform2dConfigPtr> &currentConfigPipe_;

    // IceStorm topic to which we send our updates and cand subscribe other to
    IceStorm::TopicPrx topic_;
};

} // namespace

#endif
