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

#include "mainloop.h"
#include "pathmaintainer.h"
#include "pathfollower2dI.h"
#include "testsim/simulator.h"
#include "localnavutil/pose.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace localnav {

MainLoop::MainLoop( DriverFactory          &driverFactory,
                    Clock                  &clock,
                    PathFollower2dI        &pathFollowerInterface,
                    const orcaice::Context &context )
    : localNavManager_(NULL),
      pathMaintainer_(NULL),
      driver_(NULL),
      driverFactory_(driverFactory),
      obsConsumer_(new orcaifaceimpl::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr>),
      locConsumer_(new orcaifaceimpl::ProxiedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>),
      odomConsumer_(new orcaifaceimpl::ProxiedConsumerI<orca::Odometry2dConsumer,orca::Odometry2dData>),
      obsProxy_(NULL),
      locProxy_(NULL),
      odomProxy_(NULL),
      pathFollowerInterface_(pathFollowerInterface),
      clock_(clock),
      testMode_(false),
      context_(context)
{
    Ice::ObjectPtr odomConsumerPtr = odomConsumer_;
    odomConsumerPrx_ = orcaice::createConsumerInterface<Odometry2dConsumerPrx>( context_, odomConsumerPtr );

    Ice::ObjectPtr locConsumerPtr = locConsumer_;
    locConsumerPrx_ = orcaice::createConsumerInterface<Localise2dConsumerPrx>( context_, locConsumerPtr );
    
    Ice::ObjectPtr obsConsumerPtr = obsConsumer_;
    obsConsumerPrx_ = orcaice::createConsumerInterface<RangeScanner2dConsumerPrx>( context_, obsConsumerPtr );
}

MainLoop::MainLoop( DriverFactory          &driverFactory,
                    Clock                  &clock,
                    PathFollower2dI        &pathFollowerInterface,
                    Simulator              &testSimulator,
                    const orcaice::Context &context )
    : localNavManager_(NULL),
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
      testMode_(true),
      context_(context)
{
}

MainLoop::~MainLoop()
{
    if ( obsConsumer_ ) delete obsConsumer_;
    if ( odomConsumer_ ) delete odomConsumer_;
    if ( locConsumer_ ) delete locConsumer_;
    if ( localNavManager_ ) delete localNavManager_;
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
            sleep(1);
        }
    }
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
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( Ice::Exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e;
            context_.tracer()->error( ss.str() );
        }
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
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( Ice::Exception &e )
        {
            stringstream ss; ss << "Error when connecting to VelocityControl2d interface: " << e;
            context_.tracer()->error( ss.str() );
        }
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
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to odometry: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to odometry: " << e;
            context_.tracer()->error( ss.str() );
        }
        sleep(2);
    }    
    while ( isActive() )
    {
        try {
            odomPrx->subscribe( odomConsumerPrx_ );
            break;
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to odometry: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to odometry: " << e;
            context_.tracer()->error( ss.str() );
        }
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
            orca::Frame2d offset = locPrx->getDescription().offset;
            if ( offset.p.x != 0 || offset.p.y != 0 || offset.o != 0 )
            {
                stringstream ss;
                ss << "Handler: Can only handle localisers with zero offset.  Found: " << orcaice::toString(offset);
                throw ss.str();
            }
            break;
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch( std::string &e )
        {
            stringstream ss; ss << "Error while connecting to localise2d: " << e;
            context_.tracer()->error( ss.str() );
        }
        sleep(2);
    }    
    while ( isActive() )
    {
        try {
            locPrx->subscribe( locConsumerPrx_ );
            break;
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to localise2d: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to localise2d: " << e;
            context_.tracer()->error( ss.str() );
        }
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
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while connecting to laser: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while connecting to laser: " << e;
            context_.tracer()->error( ss.str() );
        }
        sleep(2);
    }    
    while ( isActive() )
    {
        try {
            obsPrx->subscribe(  obsConsumerPrx_ );
            break;
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e;
            context_.tracer()->error( ss.str() );
        }
        sleep(2);
    }
    while ( isActive() )
    {
        try {
            scannerDescr_ = obsPrx->getDescription();
            break;
        }
        catch( std::exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch( Ice::Exception &e )
        {
            stringstream ss; ss << "Error while subscribing to laser: " << e;
            context_.tracer()->error( ss.str() );
        }
        sleep(2);
    }
    context_.tracer()->info( "Subscribed for laser" );
}

void
MainLoop::setup()
{
    if ( !testMode_ )
    {
        connectToController();

        subscribeForOdometry();
        subscribeForLocalisation();
        subscribeForObservations();

        obsProxy_  = &(obsConsumer_->proxy_);
        locProxy_  = &(locConsumer_->proxy_);
        odomProxy_ = &(odomConsumer_->proxy_);
    }
    else
    {
        vehicleDescr_ = testSimulator_->getVehicleDescription();
        scannerDescr_ = testSimulator_->getRangeScanner2dDescription();
        obsProxy_  = &(testSimulator_->obsProxy_);
        locProxy_  = &(testSimulator_->locProxy_);
        odomProxy_ = &(testSimulator_->odomProxy_);
    }

    stringstream descrStream;
    descrStream << "Working with the following vehicle: " << orcaice::toString(vehicleDescr_) << endl;
    descrStream << "And the following range scanner: " << orcaice::toString(scannerDescr_) << endl;
    context_.tracer()->info( descrStream.str() );

    driver_ = driverFactory_.createDriver( context_, vehicleDescr_, scannerDescr_ );
    pathMaintainer_ = new PathMaintainer( pathFollowerInterface_, clock_, context_ );
    localNavManager_ = new LocalNavManager( *driver_, context_ );

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
    setup();
    driver_->init( clock_.time() );
    driver_->reset();

    const int TIMEOUT_MS = 1000;

    std::vector<Goal> currentGoals;
    bool obsoleteStall = false;
    bool uncertainLocalisation;
    orca::VelocityControl2dData velocityCmd;
    
    
    while ( isActive() )
    {
        try 
        {
            //cout<<"============================================="<<endl;

            // The rangeScanner provides the 'clock' which is the trigger for this loop
            int sensorRet = obsProxy_->getNext( rangeData_, TIMEOUT_MS );

            // Before we do anything, check whether we're enabled.
            if ( !pathFollowerInterface_.localIsEnabled() )
            {
                context_.tracer()->debug( "Doing nothing because disabled" );
                continue;
            }

            if ( sensorRet != 0 )
            {
                stringstream ss;
                ss << "Timeout waiting for range data: no data for " << TIMEOUT_MS << "ms.  Stopping.";
                context_.tracer()->error( ss.str() );
                getStopCommand( velocityCmd );
                sendCommandToPlatform( velocityCmd );
                subscribeForObservations();
                continue;
            }

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
                getStopCommand( velocityCmd );
                sendCommandToPlatform( velocityCmd );
                subscribeForOdometry();
                subscribeForLocalisation();
                continue;
            }

//                     cout<<"TRACE(mainloop.cpp): localNavManager_.getCommand:"<<endl
//                         <<"    localiseData: " << orcaice::toString(localiseData_) << endl
//                         <<"    odomData: " << orcaice::toString(odomData_) << endl;

            // grab the maximum likelihood pose of the vehicle
            orcanavutil::Pose pose = getMLPose( localiseData_ );

            // pathMaintainer knows about the whole path in global coords and where
            // we are in that path. So get the next set of (current) goals (in local 
            // coord system) for the pathplanner.
            pathMaintainer_->getActiveGoals( currentGoals,
                                             driver_->waypointHorizon(),
                                             pose );

            // TODO: rename to goalTransformer
            // Check if there is a goal. If not, set all commands to zero and reset the pathplanning driver.
            bool haveGoal = localNavManager_->checkNextGoal( localiseData_,
                                                             currentGoals,
                                                             uncertainLocalisation,
                                                             velocityCmd );

            // if there is an active goal, get the pathplanner to work out the next set of actions
            if ( haveGoal )
            {
                // The actual driver which determines the path and commands to send to the vehicle.
                // The odometry is required for the velocity, which isn't contained
                // in Localise2d.
                driver_->getCommand( obsoleteStall,
                                     uncertainLocalisation,
                                     odomData_.motion,
                                     rangeData_,
                                     currentGoals,
                                     velocityCmd );
            }

            sendCommandToPlatform( velocityCmd );
            checkWithOutsideWorld();

        }
        catch ( Ice::CommunicatorDestroyedException &e )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop.
            //
            // Could probably handle it better for an Application by stopping the component on Ctrl-C
            // before shutting down communicator.
        }
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught std::exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( std::string &e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught std::string: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( char *e )
        {
            std::stringstream ss;
            ss << "mainloop.cpp: caught char*: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( Ice::Exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
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
