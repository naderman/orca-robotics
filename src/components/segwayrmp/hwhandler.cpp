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
#include <sstream>

#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

#include "hwhandler.h"
#include "rmpexception.h"

// segway rmp drivers
#include "fakedriver.h"
#include "rmpdriver/rmpdriver.h"
#ifdef HAVE_USB_DRIVER
#   include "rmpdriver/usb/rmpusbioftdi.h"
#endif
#ifdef HAVE_CAN_DRIVER
#   include "rmpdriver/can/peakcandriver.h"
#endif
#ifdef HAVE_PLAYERCLIENT_DRIVER
#   include "playerclient/playerclientdriver.h"
#endif

using namespace std;
using namespace segwayrmp;

namespace {
    const char *SUBSYSTEM_NAME = "hardware";
}

HwHandler::HwHandler(
        orcaice::Proxy<Data>&       dataPipe,
        orcaice::Notify<Command>&   commandPipe,
        orca::VehicleDescription&   descr,
        const orcaice::Context&     context ) :
    dataPipe_(dataPipe),
    rmpIo_(0),
    driver_(0),
    context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM_NAME, 10.0 );
    context_.status()->initialising( SUBSYSTEM_NAME );

    // we'll handle incoming messages
    commandPipe.setNotifyHandler( this );

    // unsure about faults until we enable the driver
    faultProxy_.set( FaultInfo( true, "Initialising" ) );

    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";

    orca::VehicleControlVelocityDifferentialDescription *controlDescr =
        dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(descr.control)));
    if ( controlDescr == NULL )
        throw orcaice::Exception( ERROR_INFO, "Can only deal with differential drive vehicles." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw orcaice::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    config_.maxSpeed = controlDescr->maxForwardSpeed;
    config_.maxTurnrate = controlDescr->maxTurnrate;
    config_.isMotionEnabled = (bool)orcaice::getPropertyAsIntWithDefault( 
            context_.properties(), prefix+".EnableMotion", 1 );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( 
            context_.properties(), prefix+"Driver", "segwayrmpusb" );
            
    if ( driverName == "segwayrmpusb" )
    {

#ifdef HAVE_USB_DRIVER
        context_.tracer()->debug( "HwHandler: loading 'segwayrmpusb' driver",3);
        rmpIo_  = new RmpUsbIoFtdi;
        driver_ = new RmpDriver( context_, *rmpIo_ );
#else
        throw orcaice::Exception( ERROR_INFO, "HwHandler: Can't instantiate driver 'usb' because it was not built!" );
#endif
    
    }
    else if ( driverName == "segwayrmpcan" )
    {
#ifdef HAVE_CAN_DRIVER
        context_.tracer()->debug( "HwHandler: loading 'peakcan' driver",3);
        
        //Get the port name that we are being asked to open
        string portName;
        if( orcaice::getProperty( context_.properties(), prefix+"SegwayRmpCan.PortName", portName ) !=0 ){
            throw orcaice::Exception( ERROR_INFO, "HwHandler::HwHandler() Config.SegwayRmpCan.PortName not specified" );
        }

        rmpIo_ = new PeakCanDriver ( portName );
        driver_ = new RmpDriver( context_, *rmpIo_ );
#else
        throw orcaice::Exception( ERROR_INFO, "HwHandler: Can't instantiate driver 'peakcan' because it was not built!" );
#endif

    }
    else if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context_.tracer()->debug( "HwHandler: loading 'playerclient' driver",3);
        driver_ = new PlayerClientDriver( context_ );
#else
        throw orcaice::Exception( ERROR_INFO, "HwHandler: Can't instantiate driver 'playerclient' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "HwHandler: loading 'fake' driver",3);
        driver_ = new FakeDriver( context_ );
    }
    else {
        string errorStr = "HwHandler: Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "HwHandler: Valid driver values are {'segwayrmpcan', 'segwayrmpusb', 'playerclient', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    // Apply physical limits of this hardare.
    driver_->applyHardwareLimits( controlDescr->maxForwardSpeed, 
                        controlDescr->maxReverseSpeed,
                        controlDescr->maxTurnrate,
                        controlDescr->maxTurnrateAtMaxSpeed );

    context_.tracer()->debug("HwHandler: driver instantiated",3);
}

HwHandler::~HwHandler()
{
    if ( rmpIo_ )  delete rmpIo_;
    if ( driver_ ) delete driver_;
}

void
HwHandler::enableDriver()
{
    while ( isActive() ) 
    {
        try {
            context_.tracer()->info("(Re-)Enabling driver...");
            driver_->enable();
            context_.tracer()->info( "Enable succeeded." );
            return;
        }
        catch ( RmpException &e )
        {
            stringstream ss;
            ss << "HwHandler::enableDriver(): Failed to enable: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM_NAME, ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
HwHandler::run()
{
    IceUtil::Time startTime = IceUtil::Time::now();
    
    std::string driverStatus = "";
    std::string currDriverStatus = "";

    // temp data object
    Data data;

    //
    // Main loop
    //
    while( isActive() )
    {
        try 
        {
            //
            // Make sure we're OK
            //
            FaultInfo faultInfo;
            faultProxy_.get( faultInfo );
            if ( faultInfo.isError() )
            {
                context_.status()->setMaxHeartbeatInterval( SUBSYSTEM_NAME, 5.0 );    
                enableDriver();

                // we enabled, so presume we're OK.
                faultProxy_.set( FaultInfo(false) );
                context_.status()->setMaxHeartbeatInterval( SUBSYSTEM_NAME, 2.0 );

                // but make sure we're not shutting down.
                if ( !isActive() )
                    break;
            }

            //
            // Read data from the hardware
            //
            try {
                bool stateChanged = driver_->read( data );

                // stick it in the proxy, so that NetHandler can distribute it                
                dataPipe_.set( data );

                // Update status
                if ( stateChanged ) 
                {
                    string status;
                    bool isWarn, isFault;
                    driver_->getStatus( status, isWarn, isFault );
                    stringstream ss;
                    ss << "Saw state change: " << status;
                    if ( isFault )
                    {
                        context_.tracer()->error( ss.str() );
                        context_.status()->fault( SUBSYSTEM_NAME, status );
                    }
                    else if ( isWarn )
                    {
                        context_.tracer()->warning( ss.str() );
                        context_.status()->warning( SUBSYSTEM_NAME, status );
                    }
                    else
                    {
                        context_.tracer()->info( ss.str() );
                        context_.status()->ok( SUBSYSTEM_NAME, status );
                    }
                }
                else
                    context_.status()->ok( SUBSYSTEM_NAME );
            }
            catch ( RmpException &e )
            {
                stringstream ss;
                ss << "HwHandler: Failed to read: " << e.what();
                context_.tracer()->error( ss.str() );
                context_.status()->fault( SUBSYSTEM_NAME, ss.str() );
                faultProxy_.set( FaultInfo( true, ss.str() ) );
            }

        } // try
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }
        catch ( const std::exception & e )
        {
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }
        catch ( ... )
        {
            context_.tracer()->error( "unexpected exception from somewhere.");
            if ( context_.isApplication() ) {
                context_.tracer()->info( "this is an stand-alone component. Quitting...");
                context_.communicator()->destroy();
            }
        }

    } // while

    // exited main loop

    // reset the hardware
    try {
        driver_->disable();
    }
    catch ( RmpException &e )
    {
        stringstream ss;
        ss << "HwHandler: failed to disable driver: " << e.what();
        context_.tracer()->warning( ss.str() );
    }

// AlexB: This is not implemented, so I'm commenting it out.
//     // log run statistics
//     Ice::PropertiesPtr prop = context_.properties();
//     std::string prefix = context_.tag() + ".Config.";
//     bool logStats = orcaice::getPropertyAsIntWithDefault( prop, prefix+"LogStats", 1 );
//     if ( logStats ) 
//     {
//         string statsFilename = orcaice::getPropertyWithDefault( prop, prefix+"LogStatsFilename", "segwayrmp-stats.txt" );
//         std::ofstream file( statsFilename.c_str() );
//         if ( !file.is_open() ) {
//             context_.tracer()->error( "Stats were not written. Could not create file " + statsFilename );
//         }
//         // get stats
//         HwDriver::SegwayRmpStats stats;
//         driver_->get( stats );
//         file << startTime.toDateTime() << " " << stats.distanceTravelled << endl;
//         context_.tracer()->info( "Wrote stats to file " + statsFilename );
//     }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting HwHandler thread...",5);
}

//
// This is the only direct incoming link from the outside.
// Here we handle command arriving through Platform2d interface.
//
void
HwHandler::handleData( const Command& command )
{
    // if we know we can't write, don't try: inform remote component of problem
    FaultInfo faultInfo;
    faultProxy_.get( faultInfo );
    if ( faultInfo.isError() )
    {
        throw orca::HardwareFailedException( faultInfo.diagnostics() );
    }

    // check that platform motion is enabled
    if ( config_.isMotionEnabled==false ) {
        return;
    }

    // debug
    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    writeTimer_.restart();
    // Check how long since we received our last command
    if ( msecs>300 ) 
    {
        stringstream ss;
        ss << "HwHandler: It's been " << msecs << "ms since we last received a command!";
        context_.tracer()->debug( ss.str() );
    }
    
    //
    // apply max limits
    //

    // make a copy so we can apply limits
    Command limitedCommand = command;

    if ( fabs(command.vx) > config_.maxSpeed ) {
        limitedCommand.vx =
                (command.vx / fabs(command.vx)) * config_.maxSpeed;
    }
    if ( fabs(command.w) > config_.maxTurnrate ) {
        limitedCommand.w =
                (command.w / fabs(command.w)) * config_.maxTurnrate;
    }

    //
    // write to hardware
    //
    try {
        driver_->write( limitedCommand );
    }
    catch ( RmpException &e )
    {
        stringstream ss;
        ss << "HwHandler: Failed to write command to hardware: " << e.what();
        context_.tracer()->error( ss.str() );
        // set local state to failure
        faultProxy_.set( FaultInfo( true, ss.str() ) );

        // inform remote client of hardware failure
        throw orca::HardwareFailedException( ss.str() );
    }

    stringstream ssDebug;
    ssDebug << "HwHandler: Received and wrote command: vx = " << command.vx << " m/s, w = " << command.w*180.0/M_PI << " deg/s";
    context_.tracer()->debug( ssDebug.str() );
}
