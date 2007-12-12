/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include "algothread.h"

// algorithm implementations
#include "fakedriver.h"
#include "randomdriver.h"

using namespace std;
using namespace reactivewalker;

AlgoThread::AlgoThread( const orcaice::Context& context ) : 
    SubsystemThread( context.tracer(), context.status(), "AlgoThread" ),
    driver_(0),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    laser_ = new orcaifaceimpl::BufferedRangeScanner2dConsumerImpl(
                    10, hydroutil::BufferTypeCircular, context );
    odometry_ = new orcaifaceimpl::StoringOdometry2dConsumerImpl( context );
}

AlgoThread::~AlgoThread()
{
    delete driver_;
}

void
AlgoThread::initNetwork()
{
    //
    // REQUIRED INTERFACE: VelocityControl2d
    //
    orcaice::connectToInterfaceWithTag<orca::VelocityControl2dPrx>( context_, commandPrx_, "Command", this );
    context_.tracer().info( "Connected to VelocityControl2d interface.");

    //
    // REQUIRED INTERFACE: Laser
    //
    laser_->subscribeWithTag( "Laser", this );
    context_.tracer().info( "Connected and subscribed to LaserScanner2d interface.");

    //
    // REQUIRED INTERFACE: Odometry2d
    //
    odometry_->subscribeWithTag( "Odometry", this );
    context_.tracer().info( "Connected and subscribed to Odometry2d interface.");

    // NOTE: odometry_ has a small buffer called odometry_->store(), see hydroutil::Store.
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
AlgoThread::initDriver()
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
        driver_ = new RandomDriver;
    }
    else if ( driverName == "fake" )
    {
        context_.tracer().debug( "loading Fake driver",3);
        driver_ = new FakeDriver;
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer().error( errorStr);
        context_.tracer().info( "Valid driver values are {'random', 'fake'}" );
        throw hydroutil::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer().debug("driver instantiated",5);
}

void 
AlgoThread::walk()
{
    // multi-try function, will continue trying until successful or ctrl-c
    orcaice::activate( context_, this );

    initNetwork();
    initDriver();

    assert( driver_ );
    assert( laser_ );
    assert( odometry_ );
    assert( commandPrx_ );

    // we are fully initialized
    subStatus().ok();

    // temp objects
    orca::RangeScanner2dDataPtr laserData;
    orca::Odometry2dData odometryData;
    orca::VelocityControl2dData commandData;
    
    const int laserTimeoutMs = 1000;

    while ( !isStopping() )
    {
        //
        // Wait for next laser data
        //
        if ( laser_->buffer().getAndPopNext( laserData, laserTimeoutMs ) ) {
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

        context_.tracer().debug( orcaice::toString(commandData), 5 );

        //
        // send motion command to the robot
        //
        commandPrx_->setCommand( commandData );

        subStatus().heartbeat();
    }
}
