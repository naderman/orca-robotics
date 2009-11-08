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
#include <hydroutil/stringutils.h>
#include <orcaifaceutil/velocitycontrol2d.h>

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
    powerbaseManagers_.clear();
    gbxiceutilacfr::stopAndJoin( publisherThread_ );
    // note: hydroDriverLib_ gets destroyed last
}

void
MainThread::initialise()
{
    setMaxHeartbeatInterval( 10.0 );

    //
    // INITIAL CONFIGURATION
    //
    std::string prefix = context_.tag() + ".Config.";
    hydroutil::Properties prop = context_.toHydroContext(prefix).properties();

    //
    // Create hardware driver factory
    //
    std::string driverLibName = 
        prop.getPropertyWithDefault( "DriverLib", "libHydroSegwayRmpAcfrCan.so" );
    hydroDriverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
    std::auto_ptr<hydrointerfaces::SegwayRmpFactory> driverFactory( 
        hydrodll::dynamicallyLoadClass<hydrointerfaces::SegwayRmpFactory,SegwayRmpDriverFactoryMakerFunc>
        ( *hydroDriverLib_, "createDriverFactory" ) );

    //
    // Create powerbase managers
    //
    std::vector<std::string> powerbaseNames = 
        hydroutil::toStringSeq( prop.getPropertyWithDefault( "PowerbaseNames", "" ), ' ' );
    if ( powerbaseNames.size() == 0 )
        powerbaseNames.push_back( "Main" );

    for ( size_t i=0; i < powerbaseNames.size(); i++ )
    {
        std::string stripPrefix = "";
        if ( powerbaseNames.size() > 1 )
        {
            stripPrefix = powerbaseNames[i]+".";
        }
        std::auto_ptr<hydrointerfaces::SegwayRmp> hydroDriver( 
            driverFactory->createDriver( powerbaseNames[i], context_.toHydroContext(stripPrefix) ) );
        powerbaseManagers_.push_back( orcarmputil::PowerbaseManagerPtr( 
                                          new orcarmputil::PowerbaseManager( powerbaseNames[i],
                                                                             i,
                                                                             *this,
                                                                             hydroDriver,
                                                                             context_.toHydroContext(stripPrefix)) ) );
    }

    //
    // Read (user-configured) vehicle description
    //
    orca::VehicleDescription descr;
    orcaobjutil::readVehicleDescription( context_.properties(), context_.tag()+".Config.", descr );
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
    hydrointerfaces::constrain( capabilities_, powerbaseManagers_.back()->capabilities() );
    // convert back to orca format
    update( capabilities_, *controlDescr );

    //
    // Initialise the powerbase managers
    //
    orcarmputil::DriverThread::Config cfg;
    cfg.driveInReverse = (bool)prop.getPropertyAsIntWithDefault( "DriveInReverse", 0 );
    cfg.isMotionEnabled = (bool)prop.getPropertyAsIntWithDefault( "EnableMotion", 0 );
    cfg.maxForwardAcceleration = controlDescr->maxForwardAcceleration;
    cfg.maxReverseAcceleration = controlDescr->maxReverseAcceleration;
    std::string stallPrefix = "StallSensor.";
    cfg.stallSensorConfig.torqueThreshold = prop.getPropertyAsDoubleWithDefault( stallPrefix+"TorqueThreshold", 3.0 );
    cfg.stallSensorConfig.speedThreshold = prop.getPropertyAsDoubleWithDefault( stallPrefix+"speedThreshold", 0.5 );
    cfg.stallSensorConfig.timeThreshold = prop.getPropertyAsDoubleWithDefault( stallPrefix+"TimeThreshold", 0.5 );
    
    for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
    {
        try {
            powerbaseManagers_[i]->init( cfg );
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "Failed to init powerbase '"<<powerbaseManagers_[i]->name()<<": " << e.what();
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
    }

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

    velocityControl2dI_ = new orcaifaceimpl::VelocityControl2dImpl( *this, descr, "VelocityControl2d", context_ );
    velocityControl2dI_->initInterface( this, subsysName() );

    // Set up the publisher
    publisherThread_ = new PublisherThread( prop.getPropertyAsDoubleWithDefault( "Odometry2dPublishInterval", 0.1 ),
                                            prop.getPropertyAsDoubleWithDefault( "Odometry3dPublishInterval", 0.1 ),
                                            prop.getPropertyAsDoubleWithDefault( "PowerPublishInterval", 20.0 ),
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
    const bool isEStopEnabled = (bool)prop.getPropertyAsIntWithDefault( "EnableEStopInterface", 0 );
    if ( isEStopEnabled )
    {
        while ( !isStopping() )
        {
            try {
                orca::EStopPrx eStopPrx;
                orcaice::connectToInterfaceWithTag( context_, eStopPrx, "EStop" );
                orca::EStopDescription descr = eStopPrx->getDescription();
                eStopMonitor_.reset( new orcaestoputil::EStopMonitor(descr) );

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
    // Finally, start the powerbase threads rolling
    //
    for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
    {
        powerbaseManagers_[i]->startThread();
    }
}

void
MainThread::setCommand( const orca::VelocityControl2dData &incomingCommand )
{
    if ( context_.tracer().verbosity( gbxutilacfr::DebugTrace ) >= 5 )
    {
        stringstream ss;
        ss << __func__ << "(" << ifaceutil::toString(incomingCommand) << ")";
        context_.tracer().debug( "MainThread", ss.str(), 5 );
    }

    hydrointerfaces::SegwayRmp::Command internalCommand;
    convert( incomingCommand, internalCommand );

    if ( !hydrointerfaces::commandPossible( internalCommand,
                                            powerbaseManagers_.back()->capabilities() ) )
    {
        hydrointerfaces::SegwayRmp::Command originalCommand = internalCommand;
        hydrointerfaces::limit( internalCommand,
                                powerbaseManagers_.back()->capabilities() );

        stringstream ss;
        ss << "Requested command ("<<originalCommand.toString()<<") can not be achieved.  " << endl
           << "Capabilities: " << powerbaseManagers_.back()->capabilities().toString() << endl
           << "    --> limiting command to: " << internalCommand.toString();
        cout << ss.str() << endl;
        // health().warning( ss.str() );
    }

    const double EPS = 1e-9;
    if ( fabs(internalCommand.vx) > EPS || fabs(internalCommand.w) > EPS )
    {
        std::string reason;
        if ( eStopMonitor_.get() && eStopMonitor_->isEStopTriggered(reason) )
        {
            for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
                powerbaseManagers_[i]->setDesiredSpeed( hydrointerfaces::SegwayRmp::Command(0,0) );
            throw orca::EStopTriggeredException( reason );
        }
    }

    bool leftSideStalled = false;
    bool rightSideStalled = false;
    for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
    {
        powerbaseManagers_[i]->setDesiredSpeed( internalCommand );

        orcarmputil::StallType stallType = powerbaseManagers_[i]->stallType();
        if ( stallType == orcarmputil::LeftMotorStall ||
             stallType == orcarmputil::BothMotorsStall )
            leftSideStalled = true;
        if ( stallType == orcarmputil::RightMotorStall ||
             stallType == orcarmputil::BothMotorsStall )
            rightSideStalled = true;
    }
    if ( leftSideStalled || rightSideStalled )
    {
        stringstream ss;
        ss << "Stall side(s): [";
        if ( leftSideStalled )
            ss << "left";
        if ( leftSideStalled && rightSideStalled ) 
            ss << ",";
        if ( rightSideStalled )
            ss << "right";
        ss << "]";
        throw orca::MotorStalledException( ss.str() );
    }
}

void
MainThread::handleData( const orca::EStopData &incomingEStopData )
{
    assert( eStopMonitor_.get() );

    eStopMonitor_->addData( incomingEStopData );

    // Check right now in case is was just triggered
    if ( incomingEStopData.isEStopActivated )
    {
        for ( size_t i=0; i < powerbaseManagers_.size(); i++ )
            powerbaseManagers_[i]->setDesiredSpeed( hydrointerfaces::SegwayRmp::Command(0,0) );
        context_.tracer().debug( "received e-stop data with e-stop triggered, disallowing motion: "+incomingEStopData.info );
    }
}

void
MainThread::hardwareInitialised( int powerbaseID )
{
    if ( powerbaseID == 0 )
        publisherThread_->hardwareInitialised();
}

void
MainThread::receiveData( int                                     powerbaseID,
                         const hydrointerfaces::SegwayRmp::Data &data )
{
    //cout<<"TRACE(mainthread.cpp): data: " << data.toString() << endl;
    if ( powerbaseID == 0 )
        publisherThread_->publish( data );
}

void
MainThread::finalise()
{
}

}
