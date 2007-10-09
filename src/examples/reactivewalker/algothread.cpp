/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
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
    driver_(0),
    context_(context)
{
    laser_ = new orcaifaceimpl::BufferedRangeScanner2dConsumerImpl(
                    10, orcaiceutil::BufferTypeCircular, context );
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
    // REQUIRED INTERFACE: Laser
    //
    laser_->subscribeWithTag( "Laser", this );

    //
    // REQUIRED INTERFACE: Odometry2d
    //
    odometry_->subscribeWithTag( "Odometry", this );

    //
    // REQUIRED INTERFACE: VelocityControl2d
    //
    orcaice::connectToInterfaceWithTag<orca::VelocityControl2dPrx>( context_, commandPrx_, "Command", this );
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
        context_.tracer()->debug( "loading Random driver",3);
        driver_ = new RandomDriver;
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading Fake driver",3);
        driver_ = new FakeDriver;
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'random', 'fake'}" );
        throw orcaiceutil::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
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

    // temp objects
    orca::RangeScanner2dDataPtr laserData;
    orca::Odometry2dData odometryData;
    orca::VelocityControl2dData commandData;
    
    while ( !isStopping() )
    {
        //
        // Wait for next laser data
        //
        int ret = laser_->buffer().getAndPopNext( laserData, 1000 );
        // todo: check return value
        ret++;
        
        //
        // Get the latest odometry data, do not wait for the next one
        // note: we already made sure there's some data in Store, see initNetwork()
        //
        odometry_->store().get( odometryData );

        //
        // Compute motion command
        //
        driver_->computeCommand( laserData, odometryData, commandData );

        context_.tracer()->debug( orcaice::toString(commandData), 3 );

        //
        // send motion command to the robot
        //
        commandPrx_->setCommand( commandData );
    }
}
