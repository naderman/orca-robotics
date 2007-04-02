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
#include <localnavutil/isensordata.h>
#include <localnavutil/isensordescription.h>
#include <localnavutil/icontrol.h>
#include <localnavutil/icontroldata.h>
#include <localnavutil/istate.h>
#include <localnavutil/istatedata.h>
#include <localnavutil/rangescannersensordata.h>

#include "mainloop.h"
#include "testsim/simulator.h"
#include <localnavutil/rangescannersensormodel.h>
#include <localnavutil/ogmapsensormodel.h>
#include <localnavutil/velocitycontrol2dcontrol.h>
#include <localnavutil/velocitycontrol2dstate.h>
#include <localnavutil/velocitycontrol2dstatedata.h>
#include <localnavutil/drivebicyclecontrol.h>
#include <localnavutil/drivebicyclestate.h>
#include <localnavutil/drivebicyclestatedata.h>

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
      sensorModel_(0),
      controlInterface_(NULL),
      stateInterface_(NULL),
      locConsumer_(new orcaifaceimpl::proxiedLocalise2dConsumer(context)),
      locProxy_(NULL),
      pathFollowerInterface_(pathFollowerInterface),
      sensorData_(0),
      cmd_(NULL),
      state_(NULL),
      clock_(clock),
      testMode_(false),
      context_(context)
{

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );

    // the observation consumer is dependent on the type of
    // observations the driver expects. So it is set up later in driver_->subscribeForInfo().
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
      sensorModel_(0),
      controlInterface_(NULL),
      stateInterface_(NULL),
      locConsumer_(NULL),
      locProxy_(NULL),
      pathFollowerInterface_(pathFollowerInterface),
      sensorData_(0),
      cmd_(NULL),
      state_(NULL),
      testSimulator_(&testSimulator),
      clock_(clock),
      testMode_(true),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );
}

MainLoop::~MainLoop()
{
    if ( sensorModel_ ) delete sensorModel_;
    if ( sensorData_ ) delete sensorData_;
    if ( speedLimiter_ ) delete speedLimiter_;
    if ( pathMaintainer_ ) delete pathMaintainer_;
    if ( controlInterface_ ) delete controlInterface_;
    if ( stateInterface_ ) delete stateInterface_;
    if ( cmd_ ) delete cmd_;
    if ( state_ ) delete state_;
    if ( driver_ ) delete driver_;
}

void
MainLoop::ensureProxiesNotEmpty()
{
    // Ensure that there's something in our proxys
    while ( isActive() )
    {
        bool gotObs  = !sensorModel_->isProxyEmpty();
        bool gotLoc  = !locProxy_->isEmpty();
        bool gotState= !stateInterface_->isProxyEmpty();

        if ( gotObs && gotLoc && gotState )
        {
            context_.tracer()->info( "Received at least one data item from every provided interface." );
            return;
        }
        else
        {
            stringstream ss;
            ss << "Still waiting for intial data to arrive.  gotObs="<<gotObs<<", gotLoc="<<gotLoc<<", gotState="<<gotState;
            context_.tracer()->warning( ss.str() );
            context_.status()->initialising( SUBSYSTEM, ss.str() );
            sleep(1);
        }
    }
}

/// we pass in the currentCommand as a pointer to allow passing NULL
void 
MainLoop::getStopCommand( IControlData* cmd, IStateData* currentState )
{
    if( cmd == NULL )
    {
        // now how did that happen?
        throw orca::DataNotExistException( "Passed a NULL IControlData command to fill out with data!" );
    }
    cmd->setStopCommand( currentState );
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
    if ( controlInterface_ == NULL )
    {
      throw orca::DataNotExistException( "ERROR: Control Interface has not been created yet..." );
    }
    if ( !controlInterface_->connectToInterface() )
    {
      sleep(2);
    }
    else
    {
      break;
    }
  }
  // now get vehicle description
  while ( isActive() )
  {
    try 
    {
      //  Get the vehicle description
      vehicleDescr_ = controlInterface_->description();
      break;
    }
    catch ( Ice::Exception &e )
    {
      stringstream ss; ss << "Error when retrieving control model description: " << e;
      context_.tracer()->error( ss.str() );
    }
    catch ( std::exception &e )
    {
      stringstream ss; ss << "Error when retrieving control model description: " << e.what();
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
            orca::Frame2d offset = locPrx->getDescription().offset;
            if ( offset.p.x != 0 || offset.p.y != 0 || offset.o != 0 )
            {
                stringstream ss;
                ss << "Handler: Can only handle localisers with zero offset.  Found: " << orcaice::toString(offset);
                throw ss.str();
            }
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
    while ( isActive() )
    {
        if ( !sensorModel_->connectToInterface() )
        {
            context_.status()->heartbeat( SUBSYSTEM );
            sleep(2);
        }
        else
        {
            break;
        }
    }

    while ( isActive() )
    {
        if ( !sensorModel_->subscribe() )
        {
            context_.status()->heartbeat( SUBSYSTEM );
            sleep(2);
        }
        else
        {
            break;
        }
    }
    context_.tracer()->info( "Subscribed for observations" );
}

void
MainLoop::subscribeForState()
{
  while ( isActive() )
  {
    if ( !stateInterface_->connectToInterface() )
    {
      sleep(2);
    }
    else
    {
      break;
    }
  }

  while ( isActive() )
  {
    if ( !stateInterface_->subscribe() )
    {
      sleep(2);
    }
    else
    {
      break;
    }
  }
  context_.tracer()->info( "Subscribed for VehicleState (i.e., odometry)" );
}


void
MainLoop::setup()
{
  driver_ = driverFactory_.createDriver( context_ );
  
    //
    // connect to the vehicle control, get the vehicle description,
    // and create the driver for the path planner
    //
  
    // TODO Should incorporate this logic into a IControl factory for
    // cleanliness's sake.
    ControlType controlType = driver_->controlType( );
    if( controlType == velocityControl2d )
    {
      controlInterface_ = new VelocityControl2dControl( context_ );
      stateInterface_   = new VelocityControl2dState( context_ );
      cmd_              = new VelocityControl2dControlData( );
      state_            = new VelocityControl2dStateData( );
    }
    else if( controlType == driveBicycle )
    {
      controlInterface_ = new DriveBicycleControl( context_ );
      stateInterface_   = new DriveBicycleState( context_ );
      cmd_              = new DriveBicycleControlData( );
      state_            = new DriveBicycleStateData( );
    }
    else
    {
      throw orca::OrcaException( "An appropriate controlType was not selected. You should choose between velocityControl2d and driveBicycle" );
    }
    
    if ( !testMode_ )
    {
        connectToController( );
    }
    else
    {
        vehicleDescr_ = testSimulator_->getVehicleDescription();
    }
        
    driver_->setVehicleDescription( vehicleDescr_ );

    //
    // instantiate the sensor model
    //
    
    // query driver for the type of sensor model it requires
     SensorModelType modelType = driver_->sensorModelType();
    
    // set up the type of sensor model according to the driver's requirements
    if( modelType == rangeModel )
    {
        sensorModel_ = new RangeScannerSensorModel( context_ );
        sensorData_ = new RangeScannerSensorData();
    }
    else if( modelType == ogMapModel )
    {
        sensorModel_ = new OgMapSensorModel( context_ );
        sensorData_ = new OgMapSensorData();
    }
    else
    {
        context_.tracer()->error( "Mainloop.cpp: Unknown sensor model... exiting" );
        assert( modelType==rangeModel || modelType==ogMapModel );
        exit(1);
    }

    //
    // set up all the interface subscriptions and tell the driver the sensor description
    //
    
    if ( !testMode_ )
    {
        subscribeForLocalisation();
        subscribeForObservations();
        subscribeForState();
        // tell the driver the sensor description
        driver_->setSensorModelDescription( sensorModel_->description() );
        
        // obsProxy_ is set up in subscribeForObservations()
        locProxy_  = &(locConsumer_->proxy());
        // odomProxy_ is setup in subscribeForState()
    }
    else
    {
        // tell the driver the simulated sensor description
        driver_->setSensorModelDescription( testSimulator_->rangeScanner2dDescription() );
        
        // TODO: setSimProxy() only works for range data... make general 
        sensorModel_->setSimProxy( &(testSimulator_->obsProxy_) );
        locProxy_  = &(testSimulator_->locProxy_);
        // TODO: setup odomProxy() for the simulator somehow through the state interface...
        stateInterface_->setSimProxy( &(testSimulator_->odomProxy_) );
        
        // Send an initial command
        // testSimulator_->setCommand( cmd );
      std::cout << "delete me..." << std::endl;
    }

    pathMaintainer_ = new orcalocalnav::PathMaintainer( pathFollowerInterface_, clock_, context_ );
    speedLimiter_ = new orcalocalnav::SpeedLimiter( context_ );

    driver_->printConfiguration();
    
    initInterfaces();
    ensureProxiesNotEmpty();
}

void
//     MainLoop::sendCommandToPlatform( const orca::VelocityControl2dData& cmd )
MainLoop::sendCommandToPlatform( const IControlData& cmd )
{
    try {
        if ( testMode_ )
        {
          // TODO FIXME testSimulator only works for VelocityControl2d driver
          localnav::VelocityControl2dControlData *cmdVelocityControl2d = 
            dynamic_cast<localnav::VelocityControl2dControlData *>( const_cast<IControlData*>(&cmd) );
          if( cmdVelocityControl2d == NULL )
          {
            throw orca::DataNotExistException("Test mode can only send commands using VelocityControl2d driver..." );
          }
          orca::VelocityControl2dData cmdData = cmdVelocityControl2d->velocityControl2dData();
          testSimulator_->setCommand( cmdData );
        }
        else
        {
            controlInterface_->setCommand( cmd );
        }
    }
    catch ( orca::HardwareFailedException &e )
    {
        stringstream ss;
        ss << e.what;
        context_.tracer()->warning( ss.str() );
        context_.status()->warning( SUBSYSTEM, ss.str() );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "While giving command to platform: " << e;
        context_.tracer()->warning( ss.str() );
        context_.status()->warning( SUBSYSTEM, ss.str() );
    }    
}

void
MainLoop::run()
{
    try {
        setup();
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
        //
        // Could probably handle it better for an Application by stopping the component on Ctrl-C
        // before shutting down communicator.
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop.cpp): Caught unexpected exception during setup(): " << e;
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
        exit(1);
    }
    catch ( std::exception &e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught std::exception during setup():" << e.what();
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
        exit(1);
    }
    catch ( std::string &e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught std::string during setup(): " << e;
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
        exit(1);
    }
    catch ( char *e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught char* during setup(): " << e;
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
        exit(1);
    }
    catch ( ... )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception during setup().";
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
        exit(1);
    }

    driver_->init( clock_.time() );

    const int TIMEOUT_MS = 1000;

    std::vector<orcalocalnav::Goal> currentGoals;
    bool obsoleteStall = false;

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 2.0 );

    while ( isActive() )
    {
        try 
        {
            //cout<<"============================================="<<endl;
            
            // TODO: Ben - copy sensorModel_->getNext() for stateInterface_->getNext()
            // IControlData &currentState = stateInterface_->getState( TIMEOUT_MS );
            state_ = stateInterface_->getNext( TIMEOUT_MS );
            if( state_ == NULL )
            {
              // yikes, something's wrong.
                // we pass in our current state, in case that informs us on a better
                // way to issue a "stop" command (e.g., knowing our current velocity
                // or steering angle in a bicycle)
                getStopCommand( cmd_ , state_ );
                sendCommandToPlatform( *cmd_ );
                // now we try again to subscribe?
                stateInterface_->subscribe();
                continue;
            }
            

            // The incoming sensor data provides the 'clock' which is the trigger for this loop
            sensorData_ = sensorModel_->getNext( TIMEOUT_MS );
            if ( sensorData_ == 0 )
            {
                // we pass in our current state, in case that informs us on a better
                // way to issue a "stop" command (e.g., knowing our current velocity
                // or steering angle in a bicycle)
                context_.status()->warning( SUBSYSTEM, "Failed to get sensor data" );
                getStopCommand( cmd_ , state_ );
                sendCommandToPlatform( *cmd_ );
                sensorModel_->subscribe();
                continue;
            }
            
            // Time how long it takes us to make a decision and send the command
            orcamisc::RealTimeStopwatch timer;

            // Tell everyone what time it is, boyeee
            clock_.setTime( sensorData_->timeStamp() );

            locProxy_->get( localiseData_ );

            const double THRESHOLD = 1.0; // seconds
            if ( areTimestampsDodgy( *sensorData_, localiseData_, *state_, THRESHOLD ) )
            {
                stringstream ss;
                ss << "Timestamps are more than "<<THRESHOLD<<"sec apart: " << endl
                   << "\t rangeData:    " << orcaice::toString(sensorData_->timeStamp()) << endl
                   << "\t localiseData: " << orcaice::toString(localiseData_.timeStamp) << endl
                   << "\t state:     " << orcaice::toString(state_->timeStamp()) << endl
                   << "Maybe something is wrong: Stopping.";
                context_.tracer()->error( ss.str() );
                context_.tracer()->warning( ss.str() );
                context_.status()->warning( SUBSYSTEM, ss.str() );
                getStopCommand( cmd_ , state_ );
                sendCommandToPlatform( *cmd_ );
                subscribeForState();
                subscribeForLocalisation();
                continue;
            }

//             stringstream ss;
//             ss << "Timestamps: " << endl
//                << "\t rangeData:    " << orcaice::toString(rangeData_->timeStamp) << endl
//                << "\t localiseData: " << orcaice::toString(localiseData_.timeStamp) << endl
//                << "\t odomData:     " << orcaice::toString(odomData_.timeStamp) << endl
//                << "\t now:          " << orcaice::toString(orcaice::getNow());
//             context_.tracer()->info( ss.str() );

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
            // TODO: wrap up odom in currentState
            driver_->getCommand( obsoleteStall,
                                 uncertainLocalisation,
                                 pose,
                                 *sensorData_,
                                 currentGoals,
                                 *state_,
                                 *cmd_ );
            
            // For big debug levels, give feedback through tracer.
            {
                std::stringstream ss;
                ss <<  "MainLoop: Setting command: " << *cmd_ ;
                // TODO: add orcaice::toString( const IControlData& ) method 
                 context_.tracer()->debug( ss.str(), 5 );
            }

            // Only send the command if we're enabled.
            if ( pathFollowerInterface_.localIsEnabled() )
            {
                sendCommandToPlatform( *cmd_ );
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

            // TODO FIXME Jon--make sure Alex B. sees this and is happy. :)
            // Because the cmd_ is now persistent, we can't count on the stack to create one
            // with default constructor values anymore, so we issue a 'stop' command to our 
            // persistent value here, in case anything goes wrong and we can't correct it
            // before sending it on to the platform.
            getStopCommand( cmd_, state_ );
          
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
            //
            // Could probably handle it better for an Application by stopping the component on Ctrl-C
            // before shutting down communicator.
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
MainLoop::areTimestampsDodgy( const ISensorData&                sensorData, 
                              const orca::Localise2dData&       localiseData, 
                              const IStateData&                 stateData,
                              double                           threshold )
{
    if ( fabs( orcaice::timeDiffAsDouble( sensorData.timeStamp(), localiseData.timeStamp ) ) >= threshold )
        return true;
    if ( fabs( orcaice::timeDiffAsDouble( sensorData.timeStamp(), stateData.timeStamp() ) ) >= threshold )
        return true;

    return false;
}

}
