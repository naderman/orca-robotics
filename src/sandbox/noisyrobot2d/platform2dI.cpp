/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcanavutil/orcanavutil.h>
#include "platform2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace noisyrobot2d {

Platform2dI::Platform2dI( const std::string &myTag, 
                          const std::string &realRobotTag, 
                          INoiseModel       &noiseModel,
                          const orcaice::Context & context )
    : myTag_(myTag),
      realRobotTag_(realRobotTag),
      noiseModel_(noiseModel),
      context_(context)
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<Position2dConsumerPrx>
        ( context_, consumerPrx_, myTag );

    // Connect to the real robot
    orcaice::connectToInterfaceWithTag<orca::Platform2dPrx>( context_, realRobotPrx_, realRobotTag );
}

// served out the data to the client (it was stored here by the driver at the last read)
orca::Position2dData
Platform2dI::getData(const Ice::Current& current) const
{
    // create a null pointer. data will be cloned into it.
    Position2dData data;

    try
    {
        dataBuffer_.get( data );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "try again later." );
    }

    return data;
}

orca::Position2dGeometry
Platform2dI::getGeometry(const Ice::Current& current) const
{
    return realRobotPrx_->getGeometry();
}

void
Platform2dI::subscribe(const ::orca::Position2dConsumerPrx& subscriber, const ::Ice::Current&)
{

}

void
Platform2dI::unsubscribe(const ::orca::Position2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    topicPrx_->unsubscribe( subscriber );
}

void
Platform2dI::setCommand(const ::orca::Velocity2dCommand& command, const ::Ice::Current& )
{
    orca::Velocity2dCommand warpedCommand;
    warpedCommand = command;

    noiseModel_.perturbCommand( warpedCommand );

    realRobotPrx_->setCommand( warpedCommand );
}

orca::Platform2dConfig
Platform2dI::getConfig(const ::Ice::Current& ) const
{
    return realRobotPrx_->getConfig();
}

void
Platform2dI::setConfig(const ::orca::Platform2dConfig& config, const ::Ice::Current& )
{
    realRobotPrx_->setConfig( config );
}

void
Platform2dI::localSetOdom( const ::orca::Position2dData& data )
{
    orca::Position2dData warpedData = data;
    noiseModel_.perturbOdom( warpedData );

    dataBuffer_.set( warpedData );

    // Then push to IceStorm.
    try {
        consumerPrx_->setData( warpedData );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

}
