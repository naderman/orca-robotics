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
#include "fakedriver.h"
#include "rmpexception.h"

// segway rmp drivers
#include "rmpdriver/rmpdriver.h"
#ifdef HAVE_USB_DRIVER
    #include "rmpdriver/usb/rmpusbioftdi.h"
#endif
#ifdef HAVE_CAN_DRIVER
    #include "rmpdriver/can/peakcandriver.h"
#endif
#ifdef HAVE_PLAYERCLIENT_DRIVER
    #include "playerclient/playerclientdriver.h"
#endif

using namespace std;
using namespace segwayrmp;

namespace {
    const char *SUBSYSTEM = "hardware";
}

void 
HwHandler::convert( const HwDriver::SegwayRmpData& internal, orca::Odometry2dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.o = internal.yaw;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = 0.0;
    network.motion.w = internal.dyaw;
}

void 
HwHandler::convert( const HwDriver::SegwayRmpData& internal, orca::Odometry3dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.p.z = 0.0;

    network.pose.o.r = internal.roll;
    network.pose.o.p = internal.pitch;
    network.pose.o.y = internal.yaw;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = 0.0;
    network.motion.v.z = 0.0;

    network.motion.w.x = internal.droll;
    network.motion.w.y = internal.dpitch;
    network.motion.w.z = internal.dyaw;
}

void 
HwHandler::convert( const HwDriver::SegwayRmpData& internal, orca::PowerData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.batteries[0].voltage = internal.mainvolt;
    network.batteries[1].voltage = internal.mainvolt;
    network.batteries[2].voltage = internal.uivolt;
}

void 
HwHandler::convert( const orca::VelocityControl2dData& network, HwDriver::SegwayRmpCommand& internal )
{
    internal.vx = network.motion.v.x;
    internal.w = network.motion.w;
}

HwHandler::HwHandler(
                 orcaice::Proxy<orca::Odometry2dData>& odometry2dPipe,
                 orcaice::Proxy<orca::Odometry3dData>& odometry3dPipe,
                 orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
                 orcaice::Proxy<orca::PowerData>& powerPipe,
                 orca::VehicleDescription&               descr,
                 const orcaice::Context& context ) :
    odometry2dPipe_(odometry2dPipe),
    odometry3dPipe_(odometry3dPipe),
    powerPipe_(powerPipe),
    rmpIo_(0),
    driver_(0),
    context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );

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
    config_.isMotionEnabled = (bool)orcaice::getPropertyAsIntWithDefault( context_.properties(),
                                                                          prefix+".EnableMotion", 1 );
    config_.provideOdometry3d = (bool)orcaice::getPropertyAsIntWithDefault( context_.properties(),
                                                                            prefix+".ProvideOdometry3d", 1 );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(), prefix+"Driver", "segwayrmpusb" );
            
    if ( driverName == "segwayrmpusb" )
    {

#ifdef HAVE_USB_DRIVER
        context_.tracer()->debug( "loading 'segwayrmpusb' driver",3);
        rmpIo_  = new RmpUsbIoFtdi;
        driver_ = new RmpDriver( context_, *rmpIo_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'usb' because it was not built!" );
#endif
    
    }
    else if ( driverName == "segwayrmpcan" )
    {
#ifdef HAVE_CAN_DRIVER
        context_.tracer()->debug( "loading 'peakcan' driver",3);
        rmpIo_ = new PeakCanDriver ( context_ );
        driver_ = new RmpDriver( context_, *rmpIo_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'peakcan' because it was not built!" );
#endif
    }
    else if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context_.tracer()->debug( "loading 'playerclient' driver",3);
        driver_ = new PlayerClientDriver( context_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'playerclient' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading 'fake' driver",3);
        driver_ = new FakeDriver( context_ );
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'segwayrmpusb', 'playerclient', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    // Apply physical limits of this hardare.
    driver_->applyHardwareLimits( controlDescr->maxForwardSpeed, 
                        controlDescr->maxReverseSpeed,
                        controlDescr->maxTurnrate,
                        controlDescr->maxTurnrateAtMaxSpeed );

    context_.tracer()->debug("driver instantiated",3);
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
            context_.tracer()->warning( ss.str() );
            context_.status()->warning( SUBSYSTEM, ss.str() );
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

    HwDriver::SegwayRmpData segwayRmpData;
    orca::Odometry2dData odometry2dData;
    orca::Odometry3dData odometry3dData;
    orca::PowerData powerData;

    // set up data structure for 3 batteries
    powerData.batteries.resize(3);
    powerData.batteries[0].name = "main-front";
    powerData.batteries[1].name = "main-rear";
    powerData.batteries[2].name = "ui";


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
                context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 5.0 );    
                enableDriver();

                // we enabled, so presume we're OK.
                faultProxy_.set( FaultInfo(false) );
                context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 2.0 );

                // but make sure we're not shutting down.
                if ( !isActive() )
                    break;
            }

            //
            // Read data from the hardware
            //
            try {
                std::string status;
                bool stateChanged = driver_->read( segwayRmpData, status );

                // convert internal to network format, and
                // stick it in the proxies so pullers can get it
                
                convert( segwayRmpData, odometry2dData );
                odometry2dPipe_.set( odometry2dData );

                if ( config_.provideOdometry3d )
                {
                    convert( segwayRmpData, odometry3dData );
                    odometry3dPipe_.set( odometry3dData );
                }

                convert( segwayRmpData, powerData );
                powerPipe_.set( powerData );

                // Update status
                if ( stateChanged ) 
                {
                    stringstream ss;
                    ss << "Saw state change: " << status;
                    context_.tracer()->info( ss.str() );
                    context_.status()->ok( SUBSYSTEM, status );
                }
                else
                    context_.status()->heartbeat( SUBSYSTEM );
            }
            catch ( RmpException &e )
            {
                stringstream ss;
                ss << "HwHandler: Failed to read: " << e.what();
                context_.tracer()->error( ss.str() );
                context_.status()->fault( SUBSYSTEM, ss.str() );
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
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
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

    // log run statistics
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
    bool logStats = orcaice::getPropertyAsIntWithDefault( prop, prefix+"LogStats", 1 );
    if ( logStats ) {
        string statsFilename = orcaice::getPropertyWithDefault( prop, prefix+"LogStatsFilename", "segwayrmp-stats.txt" );
        std::ofstream file( statsFilename.c_str() );
        if ( !file.is_open() ) {
            context_.tracer()->error( "Stats were not written. Could not create file " + statsFilename );
        }
        // get stats
        HwDriver::SegwayRmpStats stats;
        driver_->get( stats );
        file << startTime.toDateTime() << " " << stats.distanceTravelled << endl;
        context_.tracer()->info( "Wrote stats to file " + statsFilename );
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting HwHandler thread...",5);
}

//
// This is the only direct incoming link from the outside.
// Here we handle command arriving through Platform2d interface.
//
void
HwHandler::handleData( const orca::VelocityControl2dData & origObj )
{
    //cout<<"handling: "<<orcaice::toString(obj)<<endl;

    // make a copy so we can apply limits
    orca::VelocityControl2dData obj = origObj;

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
    // this will certainly be 'late' when we throw an exception below
    if ( msecs>300 ) {
        cout<<"HwHandler: late: " << msecs <<endl;
    }
    
    //
    // apply max limits
    //
    if ( fabs(obj.motion.v.x) > config_.maxSpeed ) {
        obj.motion.v.x =
                (obj.motion.v.x / fabs(obj.motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(obj.motion.w) > config_.maxTurnrate ) {
        obj.motion.w =
                (obj.motion.w / fabs(obj.motion.w)) * config_.maxTurnrate;
    }

    // convert from network to internal format
    HwDriver::SegwayRmpCommand segwayRmpCommand;
    convert( obj, segwayRmpCommand );

    //
    // write to hardware
    //
    try {
        driver_->write( segwayRmpCommand );
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
}
