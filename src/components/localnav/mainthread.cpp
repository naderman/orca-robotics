/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>
#include <orcaice/orcaice.h>
#include <hydronavutil/pose.h>
#include <orcalocalnav/pathfollower2dI.h>
#include <localnavutil/pose.h>
#include <hydroutil/realtimestopwatch.h>

#include "mainthread.h"
#include "testsim/simulator.h"

using namespace std;
using namespace localnav;

MainThread::MainThread( DriverFactory                &driverFactory,
                    orcalocalnav::Clock              &clock,
                    orcalocalnav::PathFollower2dI    &pathFollowerInterface,
                    const orcaice::Context           &context ) :
    hydroutil::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    speedLimiter_(NULL),
    pathMaintainer_(NULL),
    driver_(NULL),
    driverFactory_(driverFactory),
    obsConsumer_(new orcaifaceimpl::StoringRangeScanner2dConsumerImpl(context)),
    locConsumer_(new orcaifaceimpl::StoringLocalise2dConsumerImpl(context)),
    odomConsumer_(new orcaifaceimpl::StoringOdometry2dConsumerImpl(context)),
    obsStore_(NULL),
    locStore_(NULL),
    odomStore_(NULL),
    pathFollowerInterface_(pathFollowerInterface),
    clock_(clock),
    testMode_(false),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

MainThread::MainThread( DriverFactory               &driverFactory,
                    orcalocalnav::Clock             &clock,
                    orcalocalnav::PathFollower2dI   &pathFollowerInterface,
                    Simulator                       &testSimulator,
                    const orcaice::Context          &context ) :
    hydroutil::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    speedLimiter_(NULL),
    pathMaintainer_(NULL),
    driver_(NULL),
    driverFactory_(driverFactory),
    obsConsumer_(NULL),
    locConsumer_(NULL),
    odomConsumer_(NULL),
    obsStore_(NULL),
    locStore_(NULL),
    odomStore_(NULL),
    pathFollowerInterface_(pathFollowerInterface),
    testSimulator_(&testSimulator),
    clock_(clock),
    testMode_(true),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

MainThread::~MainThread()
{
    if ( speedLimiter_ ) delete speedLimiter_;
    if ( pathMaintainer_ ) delete pathMaintainer_;
    if ( driver_ ) delete driver_;
}

void
MainThread::ensureProxiesNotEmpty()
{
    // Ensure that there's something in our proxys
    while ( !isStopping() )
    {
        bool gotObs  = !obsStore_->isEmpty();
        bool gotLoc  = !locStore_->isEmpty();
        bool gotOdom = !odomStore_->isEmpty();


        if ( gotObs && gotLoc && gotOdom )
        {
            context_.tracer().info( "Received at least one data item from every provided interface." );
            return;
        }
        else
        {
            stringstream ss;
            ss << "Still waiting for intial data to arrive.  gotObs="<<gotObs<<", gotLoc="<<gotLoc<<", gotOdom="<<gotOdom;
            context_.tracer().warning( ss.str() );
            subStatus().initialising( ss.str() );
            sleep(1);
        }
    }
    // Set the initial time
    orca::RangeScanner2dDataPtr obs;
    obsStore_->get( obs );
    clock_.setTime( obs->timeStamp );
}

void 
MainThread::getStopCommand( orca::VelocityControl2dData& cmd )
{
    cmd.motion.v.x = 0.0;
    cmd.motion.v.y = 0.0;
    cmd.motion.w   = 0.0;
}

void
MainThread::initInterfaces()
{
    while ( !isStopping() )
    {
        try {
            pathFollowerInterface_.initInterface();
            return;
        }
        catch ( hydroutil::Exception &e )
        {
            stringstream ss;
            ss << "MainThread: Failed to initialise PathFollower2d interface: " << e.what();
            context_.tracer().warning( ss.str() );
            if ( testMode_ )
            {
                context_.tracer().warning( "Continuing regardless..." );
                return;
            }
        }
        subStatus().initialising( "initInterfaces()" );
        sleep(2);
    }
}

void
MainThread::connectToController()
{
    while ( !isStopping() )
    {
        try 
        {
            // connect to the controller
            orcaice::connectToInterfaceWithTag( context_, velControl2dPrx_, "VelocityControl2d" );
            context_.tracer().debug("connected to a 'VelocityControl2d' interface",5);
            break;
        }
        catch ( Ice::Exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }
    while ( !isStopping() )
    {
        try 
        {
            // Get the vehicle description
            vehicleDescr_ = velControl2dPrx_->getDescription();
            break;
        }
        catch ( Ice::Exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }
}

void
MainThread::subscribeForOdometry()
{
    orca::Odometry2dPrx   odomPrx;
    
    while ( !isStopping() )
    {
        try {
            orcaice::connectToInterfaceWithTag( context_, odomPrx, "Odometry2d" );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to odometry: " << e;
            context_.tracer().error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to odometry: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }    
    while ( !isStopping() )
    {
        try {
            odomPrx->subscribe( odomConsumer_->consumerPrx() );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to odometry: " << e;
            context_.tracer().error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to odometry: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }
    context_.tracer().info( "Subscribed for odometry" );
}

void
MainThread::subscribeForLocalisation()
{
    orca::Localise2dPrx   locPrx;
        
    while ( !isStopping() )
    {
        try {
            orcaice::connectToInterfaceWithTag( context_, locPrx, "Localisation" );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e;
            context_.tracer().error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch( std::string &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e;
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }    
    while ( !isStopping() )
    {
        try {
            locPrx->subscribe( locConsumer_->consumerPrx() );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to localise2d: " << e;
            context_.tracer().error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to localise2d: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }
    context_.tracer().info( "Subscribed for localisation" );
}

void
MainThread::subscribeForObservations()
{
    orca::RangeScanner2dPrx obsPrx;

    while ( !isStopping() )
    {
        try {
            orcaice::connectToInterfaceWithTag( context_, obsPrx, "Observations" );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to laser: " << e;
            context_.tracer().error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to laser: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }    
    while ( !isStopping() )
    {
        try {
            obsPrx->subscribe(  obsConsumer_->consumerPrx() );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e;
            context_.tracer().error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }
    while ( !isStopping() )
    {
        try {
            scannerDescr_ = obsPrx->getDescription();
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e;
            context_.tracer().error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e.what();
            context_.tracer().error( ss.str() );
        }
        subStatus().heartbeat();
        sleep(2);
    }
    context_.tracer().info( "Subscribed for laser" );
}

void
MainThread::setup()
{
    if ( !testMode_ )
    {
        connectToController();

        subscribeForOdometry();
        subscribeForLocalisation();
        subscribeForObservations();

        obsStore_  = &(obsConsumer_->store());
        locStore_  = &(locConsumer_->store());
        odomStore_ = &(odomConsumer_->store());
    }
    else
    {
        vehicleDescr_ = testSimulator_->getVehicleDescription();
        scannerDescr_ = testSimulator_->rangeScanner2dDescription();
        obsStore_  = &(testSimulator_->obsStore_);
        locStore_  = &(testSimulator_->locStore_);
        odomStore_ = &(testSimulator_->odomStore_);
    }

    stringstream descrStream;
    descrStream << "Working with the following vehicle: " << orcaice::toString(vehicleDescr_) << endl;
    descrStream << "And the following range scanner: " << orcaice::toString(scannerDescr_) << endl;
    context_.tracer().info( descrStream.str() );

    driver_ = driverFactory_.createDriver( context_, vehicleDescr_, scannerDescr_ );
    pathMaintainer_ = new orcalocalnav::PathMaintainer( pathFollowerInterface_, clock_, context_ );
    speedLimiter_ = new orcalocalnav::SpeedLimiter( context_ );

    initInterfaces();
    ensureProxiesNotEmpty();
}

void
MainThread::sendCommandToPlatform( const orca::VelocityControl2dData& cmd )
{
    try {
        if ( testMode_ )
            testSimulator_->setCommand( cmd );
        else
            velControl2dPrx_->setCommand( cmd );
    }
    catch ( orca::HardwareFailedException &e )
    {
        stringstream ss;
        ss << e.what;
        context_.tracer().warning( ss.str() );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While giving command to platform: " << e;
        context_.tracer().warning( ss.str() );
    }    
}

void
MainThread::walk()
{
    try {
        setup();
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop.cpp): caught exception during setup/init: " << e;
        context_.tracer().error( ss.str() );
        subStatus().fault( ss.str() );
		exit(1);
    }
    catch ( std::exception &e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught exception during setup/init: " << e.what();
        context_.tracer().error( ss.str() );
        subStatus().fault( ss.str() );
		exit(1);
    }
    catch ( std::string &e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught exception during setup/init: " << e;
        context_.tracer().error( ss.str() );
        subStatus().fault( ss.str() );
		exit(1);
    }
    catch ( char *e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught exception during setup/init: " << e;
        context_.tracer().error( ss.str() );
        subStatus().fault( ss.str() );
		exit(1);
    }
    catch ( ... )
    {
        context_.tracer().error( "Caught unknown exception during setup/init." );
        exit(1);
    }

    const int TIMEOUT_MS = 1000;

    std::vector<orcalocalnav::Goal> currentGoals;
    bool obsoleteStall = false;
    orca::VelocityControl2dData velocityCmd;

    subStatus().setMaxHeartbeatInterval( 2.0 );

    while ( !isStopping() )
    {
        try 
        {
            // The rangeScanner provides the 'clock' which is the trigger for this loop
            int sensorRet = obsStore_->getNext( rangeData_, TIMEOUT_MS );

            if ( sensorRet != 0 )
            {
                stringstream ss;
                ss << "Timeout waiting for range data: no data for " << TIMEOUT_MS << "ms.  Stopping.";
                context_.tracer().error( ss.str() );
                subStatus().warning( ss.str() );
                getStopCommand( velocityCmd );
                sendCommandToPlatform( velocityCmd );
                subscribeForObservations();
                continue;
            }
            
            // Time how long it takes us to make a decision and send the command
            hydroutil::RealTimeStopwatch timer;

            // Tell everyone what time it is, boyeee
            clock_.setTime( rangeData_->timeStamp );

            locStore_->get( localiseData_ );
            odomStore_->get( odomData_ );

            const double THRESHOLD = 1.0; // seconds
            if ( areTimestampsDodgy( rangeData_, localiseData_, odomData_, THRESHOLD ) )
            {
                stringstream ss;
                ss << "Timestamps are more than "<<THRESHOLD<<"sec apart: " << endl
                   << "\t rangeData:    " << orcaice::toString(rangeData_->timeStamp) << endl
                   << "\t localiseData: " << orcaice::toString(localiseData_.timeStamp) << endl
                   << "\t odomData:     " << orcaice::toString(odomData_.timeStamp) << endl
                   << "Maybe something is wrong: Stopping.";
                context_.tracer().error( ss.str() );
                subStatus().warning( ss.str() );
                getStopCommand( velocityCmd );
                sendCommandToPlatform( velocityCmd );
                subscribeForOdometry();
                subscribeForLocalisation();
                continue;
            }

            orca::Time now = orcaice::getNow();
            stringstream ss;
            ss << "Timestamps: " << endl
               << "\t rangeData:    " << orcaice::toString(rangeData_->timeStamp) << endl
               << "\t localiseData: " << orcaice::toString(localiseData_.timeStamp) << endl
               << "\t odomData:     " << orcaice::toString(odomData_.timeStamp) << endl
               << "\t now:          " << orcaice::toString(now);
            context_.tracer().debug( ss.str(), 3 );

            // grab the maximum likelihood pose of the vehicle
            hydronavutil::Pose pose = getMLPose( localiseData_ );
            
            bool uncertainLocalisation = orcaice::localisationIsUncertain( localiseData_ );
//             if ( uncertainLocalisation )
//                 context_.tracer().warning( "MainThread: Localisation is uncertain..." );

            // pathMaintainer knows about the whole path in global coords and where
            // we are in that path. So get the next set of current goals in local 
            // coord system for the pathplanner.
            // Also return a flag indicating if we have an active goal
			
			assert(driver_!=NULL && "Driver is not instantiated" );
            bool haveGoal = pathMaintainer_->getActiveGoals( currentGoals,
                                                             driver_->waypointHorizon(),
                                                             pose );

            if ( haveGoal )
            {
                // If we do have an active goal, limit the max speed for the current goal
                // and get the pathplanner to work out the next set of actions
                speedLimiter_->constrainMaxSpeeds( currentGoals.at(0) );
            }     
            
            // The actual driver which determines the path and commands to send to the vehicle.
            // The odometry is required for the velocity, which isn't contained
            // in Localise2d.
            driver_->getCommand( obsoleteStall,
                                 uncertainLocalisation,
                                 pose,
                                 odomData_.motion,
                                 localiseData_.timeStamp,
                                 rangeData_,
                                 currentGoals,
                                 velocityCmd );
            
            // For big debug levels, give feedback through tracer.
            {
                std::stringstream ss;
                ss << "MainThread: Setting command: " << orcaice::toString( velocityCmd );
                context_.tracer().debug( ss.str(), 5 );
            }

            // Only send the command if we're enabled.
            if ( pathFollowerInterface_.localIsEnabled() )
            {
                sendCommandToPlatform( velocityCmd );
            }
            else
            {
                context_.tracer().debug( "Doing nothing because disabled" );
                subStatus().ok();
                continue;
            }

            std::stringstream timerSS;
            timerSS << "MainThread: time to make and send decision: " << timer.elapsedSeconds()*1000.0 << "ms";
            context_.tracer().debug( timerSS.str(), 3 );

            checkWithOutsideWorld();

            if ( uncertainLocalisation )
                subStatus().warning( "Localisation is uncertain, but everything else is OK." );
            else
                subStatus().ok();
        }
        catch ( Ice::CommunicatorDestroyedException &e )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop.
        }
        catch ( Ice::Exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught std::exception: " << e.what();
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( std::string &e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught std::string: " << e;
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( char *e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught char*: " << e;
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer().error( ss.str() );
            subStatus().fault( ss.str() );
        }
    }
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    cout<<"TRACE(mainloop.cpp): Dropping out from run()" << endl;
}

void
MainThread::checkWithOutsideWorld()
{
    // (1): world->localnav: Have we received a new path?
    pathMaintainer_->checkForNewPath();

    // (2): world<-localnav: Have we modified our wp index?
    pathMaintainer_->checkForWpIndexChange();
}

bool
MainThread::areTimestampsDodgy( const orca::RangeScanner2dDataPtr &rangeData, 
                              const orca::Localise2dData&       localiseData, 
                              const orca::Odometry2dData&       odomData,
                              double                           threshold )
{
    if ( fabs( orcaice::timeDiffAsDouble( rangeData->timeStamp, localiseData.timeStamp ) ) >= threshold )
        return true;
    if ( fabs( orcaice::timeDiffAsDouble( rangeData->timeStamp, odomData.timeStamp ) ) >= threshold )
        return true;

    return false;
}
