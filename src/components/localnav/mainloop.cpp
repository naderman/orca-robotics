/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>
#include <orcaice/orcaice.h>
#include <orcanavutil/pose.h>
#include <orcalocalnav/pathfollower2dI.h>
#include <orcalocalnav/localiseutil.h>
#include <localnavutil/pose.h>
#include <orcamisc/realtimestopwatch.h>

#include "mainloop.h"
#include "testsim/simulator.h"

using namespace std;
using namespace orca;
using namespace orcaice;
using namespace orcalocalnav;

namespace localnav {

namespace {
    const char *SUBSYSTEM = "mainloop";
}

MainLoop::MainLoop( DriverFactory                    &driverFactory,
                    orcalocalnav::Clock              &clock,
                    orcalocalnav::PathFollower2dI    &pathFollowerInterface,
                    const orcaice::Context           &context )
    : speedLimiter_(NULL),
      pathMaintainer_(NULL),
      driver_(NULL),
      driverFactory_(driverFactory),
      obsConsumer_(new orcaifaceimpl::ProxiedRangeScanner2dConsumer(context)),
      locConsumer_(new orcaifaceimpl::ProxiedLocalise2dConsumer(context)),
      odomConsumer_(new orcaifaceimpl::ProxiedOdometry2dConsumer(context)),
      obsProxy_(NULL),
      locProxy_(NULL),
      odomProxy_(NULL),
      pathFollowerInterface_(pathFollowerInterface),
      clock_(clock),
      localisationLagSec_(0),
      testMode_(false),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );
}

MainLoop::MainLoop( DriverFactory                   &driverFactory,
                    orcalocalnav::Clock             &clock,
                    orcalocalnav::PathFollower2dI   &pathFollowerInterface,
                    Simulator                       &testSimulator,
                    const orcaice::Context          &context )
    : speedLimiter_(NULL),
      pathMaintainer_(NULL),
      driver_(NULL),
      driverFactory_(driverFactory),
      obsConsumer_(NULL),
      locConsumer_(NULL),
      odomConsumer_(NULL),
      obsProxy_(NULL),
      locProxy_(NULL),
      odomProxy_(NULL),
      pathFollowerInterface_(pathFollowerInterface),
      testSimulator_(&testSimulator),
      clock_(clock),
      localisationLagSec_(0),
      testMode_(true),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );
}

MainLoop::~MainLoop()
{
    if ( speedLimiter_ ) delete speedLimiter_;
    if ( pathMaintainer_ ) delete pathMaintainer_;
    if ( driver_ ) delete driver_;
}

void
MainLoop::ensureProxiesNotEmpty()
{
    // Ensure that there's something in our proxys
    while ( isActive() )
    {
        bool gotObs  = !obsProxy_->isEmpty();
        bool gotLoc  = !locProxy_->isEmpty();
        bool gotOdom = !odomProxy_->isEmpty();


        if ( gotObs && gotLoc && gotOdom )
        {
            context_.tracer()->info( "Received at least one data item from every provided interface." );
            return;
        }
        else
        {
            stringstream ss;
            ss << "Still waiting for intial data to arrive.  gotObs="<<gotObs<<", gotLoc="<<gotLoc<<", gotOdom="<<gotOdom;
            context_.tracer()->warning( ss.str() );
            context_.status()->initialising( SUBSYSTEM, ss.str() );
            sleep(1);
        }
    }
    // Set the initial time
    orca::RangeScanner2dDataPtr obs;
    obsProxy_->get( obs );
    clock_.setTime( obs->timeStamp );
}

void 
MainLoop::getStopCommand( orca::VelocityControl2dData& cmd )
{
    cmd.motion.v.x = 0.0;
    cmd.motion.v.y = 0.0;
    cmd.motion.w   = 0.0;
}

void
MainLoop::initInterfaces()
{
    while ( isActive() )
    {
        try {
            pathFollowerInterface_.initInterface();
            return;
        }
        catch ( orcaice::Exception &e )
        {
            stringstream ss;
            ss << "MainLoop: Failed to initialise PathFollower2d interface: " << e.what();
            context_.tracer()->warning( ss.str() );
            if ( testMode_ )
            {
                context_.tracer()->warning( "Continuing regardless..." );
                return;
            }
        }
        context_.status()->initialising( SUBSYSTEM, "initInterfaces()" );
        sleep(2);
    }
}

void
MainLoop::connectToController()
{
    while ( isActive() )
    {
        try 
        {
            // connect to the controller
            orcaice::connectToInterfaceWithTag<VelocityControl2dPrx>( context_, velControl2dPrx_, "VelocityControl2d" );
            context_.tracer()->debug("connected to a 'VelocityControl2d' interface",5);
            break;
        }
        catch ( Ice::Exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }
    while ( isActive() )
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
            context_.tracer()->error( ss.str() );
        }
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }
}

void
MainLoop::subscribeForOdometry()
{
    Odometry2dPrx   odomPrx;
    
    while ( isActive() )
    {
        try {
            orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, odomPrx, "Odometry2d" );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to odometry: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to odometry: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }    
    while ( isActive() )
    {
        try {
            odomPrx->subscribe( odomConsumer_->consumerPrx() );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to odometry: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to odometry: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }
    context_.tracer()->info( "Subscribed for odometry" );
}

void
MainLoop::subscribeForLocalisation()
{
    Localise2dPrx   locPrx;
        
    while ( isActive() )
    {
        try {
            orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context_, locPrx, "Localisation" );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( std::string &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e;
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }    
    while ( isActive() )
    {
        try {
            locPrx->subscribe( locConsumer_->consumerPrx() );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to localise2d: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to localise2d: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }
    context_.tracer()->info( "Subscribed for localisation" );
}

void
MainLoop::subscribeForObservations()
{
    RangeScanner2dPrx obsPrx;

    while ( isActive() )
    {
        try {
            orcaice::connectToInterfaceWithTag<orca::RangeScanner2dPrx>( context_, obsPrx, "Observations" );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to laser: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to laser: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }    
    while ( isActive() )
    {
        try {
            obsPrx->subscribe(  obsConsumer_->consumerPrx() );
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }
    while ( isActive() )
    {
        try {
            scannerDescr_ = obsPrx->getDescription();
            break;
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        context_.status()->heartbeat( SUBSYSTEM );
        sleep(2);
    }
    context_.tracer()->info( "Subscribed for laser" );
}

void
MainLoop::setup()
{
    if ( !testMode_ )
    {
        Ice::PropertiesPtr prop = context_.properties();
        std::string prefix = context_.tag() + ".Config.";
        onSameClock_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"OnSameClock", 1 );

        connectToController();

        subscribeForOdometry();
        subscribeForLocalisation();
        subscribeForObservations();

        obsProxy_  = &(obsConsumer_->proxy());
        locProxy_  = &(locConsumer_->proxy());
        odomProxy_ = &(odomConsumer_->proxy());
    }
    else
    {
        onSameClock_ = false;
        vehicleDescr_ = testSimulator_->getVehicleDescription();
        scannerDescr_ = testSimulator_->rangeScanner2dDescription();
        obsProxy_  = &(testSimulator_->obsProxy_);
        locProxy_  = &(testSimulator_->locProxy_);
        odomProxy_ = &(testSimulator_->odomProxy_);
    }

    stringstream descrStream;
    descrStream << "Working with the following vehicle: " << orcaice::toString(vehicleDescr_) << endl;
    descrStream << "And the following range scanner: " << orcaice::toString(scannerDescr_) << endl;
    context_.tracer()->info( descrStream.str() );

    driver_ = driverFactory_.createDriver( context_, vehicleDescr_, scannerDescr_ );
    pathMaintainer_ = new orcalocalnav::PathMaintainer( pathFollowerInterface_, clock_, context_ );
    speedLimiter_ = new orcalocalnav::SpeedLimiter( context_ );

    initInterfaces();
    ensureProxiesNotEmpty();
}

void
MainLoop::sendCommandToPlatform( const orca::VelocityControl2dData& cmd )
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
        context_.tracer()->warning( ss.str() );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While giving command to platform: " << e;
        context_.tracer()->warning( ss.str() );
    }    
}

void
MainLoop::run()
{
    try {
        setup();
        driver_->init( clock_.time() );
    }
    catch ( ... )
    {
        context_.tracer()->error( "Caught exception during setup/init.  This shouldn't happen." );
        exit(1);
    }

    const int TIMEOUT_MS = 1000;

    std::vector<orcalocalnav::Goal> currentGoals;
    bool obsoleteStall = false;
    orca::VelocityControl2dData velocityCmd;

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 2.0 );

    while ( isActive() )
    {
        try 
        {
            //cout<<"============================================="<<endl;

            // The rangeScanner provides the 'clock' which is the trigger for this loop
            int sensorRet = obsProxy_->getNext( rangeData_, TIMEOUT_MS );

            if ( sensorRet != 0 )
            {
                stringstream ss;
                ss << "Timeout waiting for range data: no data for " << TIMEOUT_MS << "ms.  Stopping.";
                context_.tracer()->error( ss.str() );
                context_.status()->warning( SUBSYSTEM, ss.str() );
                getStopCommand( velocityCmd );
                sendCommandToPlatform( velocityCmd );
                subscribeForObservations();
                continue;
            }
            
            // Time how long it takes us to make a decision and send the command
            orcamisc::RealTimeStopwatch timer;

            // Tell everyone what time it is, boyeee
            clock_.setTime( rangeData_->timeStamp );

            locProxy_->get( localiseData_ );
            odomProxy_->get( odomData_ );

            const double THRESHOLD = 1.0; // seconds
            if ( areTimestampsDodgy( rangeData_, localiseData_, odomData_, THRESHOLD ) )
            {
                stringstream ss;
                ss << "Timestamps are more than "<<THRESHOLD<<"sec apart: " << endl
                   << "\t rangeData:    " << orcaice::toString(rangeData_->timeStamp) << endl
                   << "\t localiseData: " << orcaice::toString(localiseData_.timeStamp) << endl
                   << "\t odomData:     " << orcaice::toString(odomData_.timeStamp) << endl
                   << "Maybe something is wrong: Stopping.";
                context_.tracer()->error( ss.str() );
                context_.status()->warning( SUBSYSTEM, ss.str() );
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
            context_.tracer()->debug( ss.str(), 2 );

//             if ( localiseData_.timeStamp != odomData_.timeStamp )
//             {
//                 stringstream warnSS;
//                 warnSS << "localiseData and odomData have different timestamps:" << endl << ss.str();
//                 context_.tracer()->warning( warnSS.str() );
//             }

            // grab the maximum likelihood pose of the vehicle
            orcanavutil::Pose pose = getMLPose( localiseData_ );
            
            bool uncertainLocalisation = orcalocalnav::localisationIsUncertain( localiseData_ );
            if ( uncertainLocalisation )
                context_.tracer()->warning( "MainLoop: Localisation is uncertain..." );

            // pathMaintainer knows about the whole path in global coords and where
            // we are in that path. So get the next set of current goals in local 
            // coord system for the pathplanner.
            // Also return a flag indicating if we have an active goal
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
                                 localisationLagSec_,
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
                ss << "MainLoop: Setting command: " << orcaice::toString( velocityCmd );
                context_.tracer()->debug( ss.str(), 5 );
            }

            // Only send the command if we're enabled.
            if ( pathFollowerInterface_.localIsEnabled() )
            {
                sendCommandToPlatform( velocityCmd );
            }
            else
            {
                context_.tracer()->debug( "Doing nothing because disabled" );
                context_.status()->ok( SUBSYSTEM );
                continue;
            }

            std::stringstream timerSS;
            timerSS << "MainLoop: time to make and send decision: " << timer.elapsedSeconds()*1000.0 << "ms";
            context_.tracer()->debug( timerSS.str() );

            localisationLagSec_ = timer.elapsedSeconds(); // decision time
            if ( onSameClock_ )
            {
                double rangeLocalisationLag = orcaice::timeDiffAsDouble( now, localiseData_.timeStamp );
                localisationLagSec_ += rangeLocalisationLag;
                // cout<<"TRACE(mainloop.cpp): rangeLocalisationLag: " << rangeLocalisationLag*1000.0 << "ms" << endl;
            }

            stringstream sss;
            sss << "localisationLagSec_: " << localisationLagSec_*1000.0 << "ms";
            context_.tracer()->debug( sss.str() );

            checkWithOutsideWorld();

            if ( uncertainLocalisation )
                context_.status()->warning( SUBSYSTEM, "Localisation is uncertain, but everything else is OK." );
            else
                context_.status()->ok( SUBSYSTEM );
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
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught std::exception: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( std::string &e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught std::string: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( char *e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught char*: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
    }
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    cout<<"TRACE(mainloop.cpp): Dropping out from run()" << endl;
}

void
MainLoop::checkWithOutsideWorld()
{
    // (1): world->localnav: Have we received a new path?
    pathMaintainer_->checkForNewPath();

    // (2): world<-localnav: Have we modified our wp index?
    pathMaintainer_->checkForWpIndexChange();
}

bool
MainLoop::areTimestampsDodgy( const orca::RangeScanner2dDataPtr &rangeData, 
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

}
