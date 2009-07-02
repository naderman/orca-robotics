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
#include <orcaice/orcaice.h>
#include "mainthread.h"
#include <orcaobj/vehicledescription.h>
#include <orcaobjutil/vehicleutil.h>
#include <gbxutilacfr/mathdefs.h>

using namespace std;

namespace segwayrmp {

namespace {

    void convert( const orca::VehicleControlVelocityDifferentialDescription& descr, 
                  hydrointerfaces::SegwayRmp::Capabilities &c )
    {
        c.maxForwardSpeed        = descr.maxForwardSpeed;
        c.maxReverseSpeed        = descr.maxReverseSpeed;
        c.maxTurnrate            = descr.maxTurnrate;
        c.maxLateralAcceleration = descr.maxLateralAcceleration;
    }

    void update( const hydrointerfaces::SegwayRmp::Capabilities &c,
                 orca::VehicleControlVelocityDifferentialDescription& descr )
    {
        descr.maxForwardSpeed        = c.maxForwardSpeed;
        descr.maxReverseSpeed        = c.maxReverseSpeed;
        descr.maxTurnrate            = c.maxTurnrate;
        descr.maxLateralAcceleration = c.maxLateralAcceleration;
    }

    void 
    convert( const orca::VelocityControl2dData& network, hydrointerfaces::SegwayRmp::Command& internal )
    {
        internal.vx = network.motion.v.x;
        internal.w = network.motion.w;
    }

    bool commandPossible( const hydrointerfaces::SegwayRmp::Command      &cmd,
                          const hydrointerfaces::SegwayRmp::Capabilities &capabilities )
    {
        if ( cmd.vx >  capabilities.maxForwardSpeed ) return false;
        if ( cmd.vx < -capabilities.maxReverseSpeed ) return false;
        if ( cmd.w >  capabilities.maxTurnrate ) return false;
        if ( cmd.w < -capabilities.maxTurnrate ) return false;        

        const double lateralAcceleration = cmd.vx*cmd.w;
        if ( lateralAcceleration > capabilities.maxLateralAcceleration ) return false;

        return true;
    }

    void
    limit( hydrointerfaces::SegwayRmp::Command      &cmd,
           const hydrointerfaces::SegwayRmp::Capabilities &capabilities )
    {
        // Note that maxReverseSpeed is a positive number.
        CLIP_TO_LIMITS( -capabilities.maxReverseSpeed, cmd.vx, capabilities.maxForwardSpeed );
        CLIP_TO_LIMITS( -capabilities.maxTurnrate, cmd.w, capabilities.maxTurnrate );

        const double maxTurnrateToSatisfyLateralAcc = capabilities.maxLateralAcceleration / fabs(cmd.vx);
        CLIP_TO_LIMITS( -maxTurnrateToSatisfyLateralAcc, cmd.w, maxTurnrateToSatisfyLateralAcc );
    }
    
}

MainThread::MainThread( const orcaice::Context &context ) 
    : SubsystemThread( context.tracer(), context.status(), "MainThread" ),
      context_(context)
{    
    // this subsystem is designed to terminate early (before the component itself)
    // we have to let Status know so it can use this info in summarizing the component's
    // state.
    setSubsystemType( gbxutilacfr::SubsystemEarlyExit );
}

MainThread::~MainThread()
{
    gbxiceutilacfr::stopAndJoin( segwayRmpDriverThread_ );
    gbxiceutilacfr::stopAndJoin( publisherThread_ );
}

void
MainThread::initialise()
{
    setMaxHeartbeatInterval( 10.0 );

    //
    // INITIAL CONFIGURATION
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // Create hardware driver
    //
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroSegwayRmpAcfrCan.so" );
    instantiateHydroDriver(driverLibName);

    //
    // Read (user-configured) vehicle description
    //
    orca::VehicleDescription descr;
    orcaobjutil::readVehicleDescription( context_.properties(), prefix, descr );
    stringstream ss;
    ss<<"TRACE(component.cpp): Read vehicle description from configuration: " 
        << endl << orcaobj::toString(descr) << endl;
    context_.tracer().info( ss.str() );

    //
    // Check vehicle desciption sanity
    //
    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw gbxutilacfr::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw gbxutilacfr::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    //
    // Set constraints
    //
    // convert the user-configured description into capabilities
    convert( *controlDescr, capabilities_ );
    // constrain based on hardware capabilities
    hydrointerfaces::constrain( capabilities_, hydroDriver_->capabilities() );
    // convert back to orca format
    update( capabilities_, *controlDescr );

    //
    // Instantiate the segwayRmpDriverThread
    //
    segwayrmpdriverthread::DriverThread::Config cfg;
    cfg.driveInReverse = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"DriveInReverse", 0 );
    cfg.isMotionEnabled = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableMotion", 0 );
    cfg.maxForwardAcceleration = controlDescr->maxForwardAcceleration;
    cfg.maxReverseAcceleration = controlDescr->maxReverseAcceleration;
    std::string stallPrefix = prefix+"StallSensor.";
    cfg.stallSensorConfig.torqueThreshold = orcaice::getPropertyAsDoubleWithDefault( prop, stallPrefix+"TorqueThreshold", 3.0 );
    cfg.stallSensorConfig.speedThreshold = orcaice::getPropertyAsDoubleWithDefault( prop, stallPrefix+"speedThreshold", 0.5 );
    cfg.stallSensorConfig.timeThreshold = orcaice::getPropertyAsDoubleWithDefault( prop, stallPrefix+"TimeThreshold", 0.5 );
    
    segwayRmpDriverThread_ = new segwayrmpdriverthread::DriverThread( cfg,
                                                                      *hydroDriver_,
                                                                      context_.tracer(),
                                                                      context_.status(),
                                                                      *this );
    segwayRmpDriverThreadPtr_ = segwayRmpDriverThread_;

    //
    // EXTERNAL PROVIDED INTERFACES
    //
    // Initialise external interfaces
    odometry2dI_ = new orcaifaceimpl::Odometry2dImpl( descr, "Odometry2d", context_ );
    odometry2dI_->initInterface( this, subsysName() );
    
    odometry3dI_ = new orcaifaceimpl::Odometry3dImpl( descr, "Odometry3d", context_ );
    odometry3dI_->initInterface( this, subsysName() );

    powerI_ = new orcaifaceimpl::PowerImpl( "Power", context_ );
    powerI_->initInterface( this, subsysName() );

    velocityControl2dI_ = new orcaifaceimpl::VelocityControl2dImpl( descr, "VelocityControl2d", context_ );
    velocityControl2dI_->initInterface( this, subsysName() );
    // register ourselves as data handlers (it will call the handleData() callback).
    velocityControl2dI_->setNotifyHandler( this );

    // Set up the publisher
    publisherThread_ = new PublisherThread( orcaice::getPropertyAsDoubleWithDefault( 
                                                context_.properties(), prefix+"Odometry2dPublishInterval", 0.1 ),
                                            orcaice::getPropertyAsDoubleWithDefault( 
                                                context_.properties(), prefix+"Odometry3dPublishInterval", 0.1 ),
                                            orcaice::getPropertyAsDoubleWithDefault( 
                                                context_.properties(), prefix+"PowerPublishInterval", 20.0 ),
                                            odometry2dI_,
                                            odometry3dI_,
                                            powerI_,
                                            context_.tracer(),
                                            context_.status(),
                                            context_.history() );
    publisherThreadPtr_ = publisherThread_;
    publisherThread_->start();

    //
    // (optionally) required e-stop interface
    //
    const bool isEStopEnabled = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableEStopInterface", 0 );
    if ( isEStopEnabled )
    {
        while ( !isStopping() )
        {
            try {
                orca::EStopPrx eStopPrx;
                orcaice::connectToInterfaceWithTag( context_, eStopPrx, "EStop" );
                orca::EStopDescription descr = eStopPrx->getDescription();
                eStopMonitor_.reset( new orcarobotdriverutil::EStopMonitor(descr) );

                eStopConsumerI_ = new orcaifaceimpl::NotifyingEStopConsumerImpl(context_);
                eStopConsumerI_->setNotifyHandler( this );
                eStopConsumerI_->subscribeWithTag( "EStop", this, subsysName() );

                break;
            }
            catch ( ... ) {
                orcaice::catchExceptionsWithStatusAndSleep( "connecting to e-stop", health() );
            }
        }
    }

    //
    // Finally, start the driver thread rolling
    //
    segwayRmpDriverThread_->start();
}

void
MainThread::instantiateHydroDriver( const std::string &driverLibName )
{
    context_.tracer().debug( "HwThread: Loading hydro driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::SegwayRmpFactory> driverFactory;
    try {
        hydroDriverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::SegwayRmpFactory,DriverFactoryMakerFunc>
            ( *hydroDriverLib_, "createDriverFactory" ) );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer().error( e.what() );
        throw;
    }    

    // create the driver
    try {
        context_.tracer().info( "HwThread: Creating hydro driver..." );
        hydroDriver_.reset( driverFactory->createDriver( context_.toHydroContext() ) );
    }
    catch ( ... )
    {
        stringstream ss;
        ss << "MainThread: Caught exception while creating hydro driver.";
        context_.tracer().error( ss.str() );
        throw;
    }
}

void
MainThread::handleData( const orca::VelocityControl2dData &incomingCommand )
{
    hydrointerfaces::SegwayRmp::Command internalCommand;
    convert( incomingCommand, internalCommand );

    if ( !commandPossible( internalCommand,
                           capabilities_ ) )
    {
        hydrointerfaces::SegwayRmp::Command originalCommand = internalCommand;
        limit( internalCommand,
               capabilities_ );

        stringstream ss;
        ss << "Requested command ("<<originalCommand.toString()<<") can not be achieved.  " << endl
           << "Capabilities: " << capabilities_.toString() << endl
           << "    --> limiting command to: " << internalCommand.toString();
        health().warning( ss.str() );
    }
    else
    {
        health().ok();
    }

    if ( internalCommand.vx != 0.0 || internalCommand.w != 0.0 )
    {
        std::string reason;
        if ( eStopMonitor_.get() && eStopMonitor_->isEStopTriggered(reason) )
        {
            segwayRmpDriverThread_->setDesiredSpeed( hydrointerfaces::SegwayRmp::Command(0,0) );
            throw orca::EStopTriggeredException( reason );
        }
    }

    segwayRmpDriverThread_->setDesiredSpeed( internalCommand );
}

void
MainThread::handleData( const orca::EStopData &incomingEStopData )
{
    assert( eStopMonitor_.get() );

    eStopMonitor_->addData( incomingEStopData );

    // Check right now in case is was just triggered
    if ( incomingEStopData.isEStopActivated )
    {
        segwayRmpDriverThread_->setDesiredSpeed( hydrointerfaces::SegwayRmp::Command(0,0) );
        context_.tracer().debug( "received e-stop data with e-stop triggered, disallowing motion: "+incomingEStopData.info );
    }
}

void
MainThread::hardwareInitialised()
{
    publisherThread_->hardwareInitialised();
}

void
MainThread::receiveData( const hydrointerfaces::SegwayRmp::Data &data )
{
    //cout<<"TRACE(mainthread.cpp): data: " << data.toString() << endl;
    publisherThread_->publish( data );
}

void
MainThread::finalise()
{
}

}
