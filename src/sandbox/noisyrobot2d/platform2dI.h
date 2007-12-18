/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_SEGWAY_PLATFORM2D_I_H
#define ORCA_SEGWAY_PLATFORM2D_I_H

#include <IceStorm/IceStorm.h>
#include <orca/platform2d.h>
#include <hydroutil/proxy.h>
#include <orcaice/context.h>
#include "inoisemodel.h"

namespace noisyrobot2d
{

// serves latest data on demand and accepts commands
class Platform2dI : public orca::Platform2d
{
public:
    Platform2dI( const std::string &myTag,
                 const std::string &realRobotTag,
                 INoiseModel       &noiseModel,
                 const orcaice::Context & context );

    virtual ::orca::Position2dData getData(const ::Ice::Current& ) const;

    virtual ::orca::Position2dGeometry getGeometry(const ::Ice::Current& ) const;

    virtual ::orca::Platform2dConfig getConfig(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::Position2dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());


    virtual void setCommand(const ::orca::Velocity2dCommand&, const ::Ice::Current& );

    virtual void setConfig(const ::orca::Platform2dConfig&, const ::Ice::Current& );

    // WARING: not implemented!
    virtual void enableMotor(bool, const ::Ice::Current& = ::Ice::Current()) {};

    // local calls
    void localSetOdom(const ::orca::Position2dData& data);
    orca::Platform2dPrx &realRobotPrx() { return realRobotPrx_; }
    
private:

    // internal storage of latest data
    hydroutil::Proxy<orca::Position2dData> dataBuffer_;

    // The thing we publish to
    orca::Position2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx             topicPrx_;

    // the real robot
    orca::Platform2dPrx            realRobotPrx_;

    std::string                    myTag_;
    std::string                    realRobotTag_;
    INoiseModel                   &noiseModel_;
    orcaice::Context               context_;
};

} // namespace

#endif
