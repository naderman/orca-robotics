/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include <orcaifaceutil/velocitycontrol2d.h>
#include "mainthread.h"

// algorithm implementations
#include "fakedriver.h"
#include "randomdriver.h"

using namespace std;
using namespace reactivewalker;

MainThread::MainThread( const orcaice::Context& context ) : 
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    driver_(0),
    context_(context)
{
}

void 
MainThread::initialise()
{
    setMaxHeartbeatInterval( 20.0 );

    laser_ = new orcaifaceimpl::BufferedRangeScanner2dConsumerImpl(
                    10, gbxiceutilacfr::BufferTypeCircular, context_ );
    odometry_ = new orcaifaceimpl::StoringOdometry2dConsumerImpl( context_ );

    initNetwork();
    initDriver();

    assert( driver_.get() );
    assert( laser_ );
    assert( odometry_ );
    assert( commandPrx_ );
}

void 
MainThread::work()
{
    const int laserTimeoutMs = 1000;
    setMaxHeartbeatInterval( 5*laserTimeoutMs/1000 );

    // temp objects
    orca::RangeScanner2dDataPtr laserData;
    orca::Odometry2dData odometryData;
    orca::VelocityControl2dData commandData;
    
    while ( !isStopping() )
    {
        //
        // Wait for next laser data
        //
        if ( laser_->buffer().getAndPopWithTimeout( laserData, laserTimeoutMs ) ) {
            // timeout without a laser scan. 
            // this is a sign of a problem. a real component would have to deal with this
            stringstream ss; ss<<"No new laser scan after "<<laserTimeoutMs<<"ms";
            context_.tracer().warning( ss.str() );
            continue;
        }

        //
        // Get the latest odometry data, do not wait for the next one
        // NOTE: we already made sure there's some data in Store, see initNetwork()
        //
        odometry_->store().get( odometryData );
        // NOTE: the odometry data is not aligned with the laser scan
        // in a real application, we would probably need to align them somehow (e.g. interpolate)

        //
        // Compute motion command
        //
        driver_->computeCommand( laserData, odometryData, commandData );

        context_.tracer().debug( ifaceutil::toString(commandData), 5 );

        //
        // send motion command to the robot
        //
        commandPrx_->setCommand( commandData );

        health().heartbeat();
    }
}

////////////////////

void
MainThread::initNetwork()
{
    //
    // REQUIRED INTERFACE: VelocityControl2d
    //
    orcaice::connectToInterfaceWithTag<orca::VelocityControl2dPrx>( context_, commandPrx_, "Command", this, subsysName() );
    context_.tracer().info( "Connected to VelocityControl2d interface.");

    //
    // REQUIRED INTERFACE: Laser
    //
    laser_->subscribeWithTag( "Laser", this, subsysName() );
    context_.tracer().info( "Connected and subscribed to LaserScanner2d interface.");

    //
    // REQUIRED INTERFACE: Odometry2d
    //
    odometry_->subscribeWithTag( "Odometry", this, subsysName() );
    context_.tracer().info( "Connected and subscribed to Odometry2d interface.");

    // NOTE: odometry_ has a small buffer called odometry_->store(), see gbxiceutilacfr::Store.
    // it is empty until the first piece of data arrives. if you try to get data from an
    // empty Store an exception will be thrown. It is easier if just wait until it's not
    // empty here so that we don't have to worry about it later.

    orca::Odometry2dData odometryData;
    const int odometryTimeoutMs = 500;
    while ( !isStopping() ) {
        if ( !odometry_->store().getNext( odometryData, odometryTimeoutMs ) )
            break;
    }
    context_.tracer().info( "Received the first update from Odometry2d interface.");
}

void
MainThread::initDriver()
{
    //
    // Read configuration settings
    //
    std::string prefix = context_.tag() + ".Config.";

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(),
            prefix+"Driver", "random" );
    if ( driverName == "random" )
    {
        context_.tracer().debug( "loading Random driver",3);
        driver_.reset( new RandomDriver );
    }
    else if ( driverName == "fake" )
    {
        context_.tracer().debug( "loading Fake driver",3);
        driver_.reset( new FakeDriver );
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer().error( errorStr);
        context_.tracer().info( "Valid driver values are {'random', 'fake'}" );
        throw gbxutilacfr::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer().debug("driver instantiated",5);
}
