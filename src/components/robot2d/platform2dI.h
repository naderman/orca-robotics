/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
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
    Platform2dI( orcaice::Proxy<orca::Position2dData>    & position2dPipe,
                 orcaice::Notify<orca::Velocity2dCommand>& commandsPipe,
                 orcaice::Proxy<orca::Platform2dConfig>  & setConfigPipe,
                 orcaice::Proxy<orca::Platform2dConfig>  & currentConfigPipe,
                 const IceStorm::TopicPrx                      & topic );

    // NOTE: this implementation can throw DataNotExist exception but does NOT throw
    // HardwareFailedException because it's isolated from the hardware handler by the
    // data buffer.
    virtual ::orca::Position2dData getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometry getGeometry(const ::Ice::Current& ) const;

    virtual ::orca::Platform2dConfig getConfig(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    virtual void setCommand(const ::orca::Velocity2dCommand&, const ::Ice::Current& );

    virtual void setConfig(const ::orca::Platform2dConfig&, const ::Ice::Current& );

    // WARING: not implemented!
    virtual void enableMotor(bool, const ::Ice::Current& = ::Ice::Current()) {};

private:
    // the driver will put the latest data into this proxy
    orcaice::Proxy<orca::Position2dData> &position2dPipe_;
    // the driver will take the latest command from the proxy
    orcaice::Notify<orca::Velocity2dCommand> &commandPipe_;

    // for incoming requests
    orcaice::Proxy<orca::Platform2dConfig> &setConfigPipe_;
    // for the current config
    orcaice::Proxy<orca::Platform2dConfig> &currentConfigPipe_;

    // IceStorm topic to which we send our updates and cand subscribe other to
    IceStorm::TopicPrx topic_;
};

} // namespace

#endif
