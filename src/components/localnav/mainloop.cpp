/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>
#include <orcaice/orcaice.h>

#include "mainloop.h"
#include "pathmaintainer.h"
#include "pathfollower2dI.h"
#include "testsim/simulator.h"

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
      obsConsumer_(NULL),
      locConsumer_(NULL),
      odomConsumer_(NULL),
      obsProxy_(NULL),
      locProxy_(NULL),
      odomProxy_(NULL),
      pathFollowerInterface_(pathFollowerInterface),
      clock_(clock),
      testMode_(false),
      context_(context)
{
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


        if ( !( gotObs && gotLoc && gotOdom ) )
        {
            stringstream ss;
            ss << "Still waiting for intial data to arrive.  gotObs="<<gotObs<<", gotLoc="<<gotLoc<<", gotOdom="<<gotOdom;
            context_.tracer()->warning( ss.str() );
            sleep(1);
        }
        else
            return;
            
        context_.tracer()->info( "Received at least one data item from every provided interface." );
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
            descr_ = velControl2dPrx_->getDescription();
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
    Ice::ObjectPtr odomConsumerPtr = odomConsumer_;
    orca::Odometry2dConsumerPrx odomConsumerPrx =
        orcaice::createConsumerInterface<Odometry2dConsumerPrx>( context_, odomConsumerPtr );
    while ( isActive() )
    {
        try {
            odomPrx->subscribe( odomConsumerPrx );
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
    Ice::ObjectPtr locConsumerPtr = locConsumer_;
    orca::Localise2dConsumerPrx locConsumerPrx =
        orcaice::createConsumerInterface<Localise2dConsumerPrx>( context_, locConsumerPtr );
    while ( isActive() )
    {
        try {
            locPrx->subscribe(  locConsumerPrx );
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
    Ice::ObjectPtr obsConsumerPtr = obsConsumer_;
    orca::RangeScanner2dConsumerPrx obsConsumerPrx =
        orcaice::createConsumerInterface<RangeScanner2dConsumerPrx>( context_, obsConsumerPtr );
    while ( isActive() )
    {
        try {
            obsPrx->subscribe(  obsConsumerPrx );
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
}

void
MainLoop::setup()
{
    if ( !testMode_ )
    {
        connectToController();

        odomConsumer_ = new orcaifaceimpl::ProxiedConsumerI<orca::Odometry2dConsumer,orca::Odometry2dData>;
        subscribeForOdometry();

        locConsumer_  = new orcaifaceimpl::ProxiedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>;
        subscribeForLocalisation();

        obsConsumer_  = new orcaifaceimpl::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr>;
        subscribeForObservations();

        obsProxy_  = &(obsConsumer_->proxy_);
        locProxy_  = &(locConsumer_->proxy_);
        odomProxy_ = &(odomConsumer_->proxy_);
    }
    else
    {
        descr_ = testSimulator_->getVehicleDescription();
        obsProxy_  = &(testSimulator_->obsProxy_);
        locProxy_  = &(testSimulator_->locProxy_);
        odomProxy_ = &(testSimulator_->odomProxy_);
    }

    stringstream descrStream;
    descrStream << "Working with the following vehicle: " << orcaice::toString(descr_);
    context_.tracer()->info( descrStream.str() );

    driver_ = driverFactory_.createDriver( context_, descr_ );
    pathMaintainer_ = new PathMaintainer( pathFollowerInterface_, clock_, context_ );
    localNavManager_ = new LocalNavManager( *driver_, *pathMaintainer_, context_ );

    initInterfaces();
    ensureProxiesNotEmpty();
}

void
MainLoop::run()
{
    setup();

    const int TIMEOUT_MS = 1000;

    try 
    {
        while ( isActive() )
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
                getStopCommand( velocityCmd_ );
            }
            else
            {
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
                    getStopCommand( velocityCmd_ );
                }
                else
                {
//                     cout<<"TRACE(mainloop.cpp): localNavManager_.getCommand:"<<endl
//                         <<"    localiseData: " << orcaice::toString(localiseData_) << endl
//                         <<"    odomData: " << orcaice::toString(odomData_) << endl;

                    localNavManager_->getCommand( rangeData_,
                                                  localiseData_,
                                                  odomData_,
                                                  velocityCmd_ );
                }
            }
            
            // Send the command to the platform
            try {
                if ( testMode_ )
                    testSimulator_->setCommand( velocityCmd_ );
                else
                    velControl2dPrx_->setCommand( velocityCmd_ );
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
            
            checkWithOutsideWorld();
        }
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
