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
// hardware drivers
#include "fakedriver.h"
#ifdef HAVE_PLAYERCLIENT_DRIVER
    #include "playerclient/playerclientdriver.h"
#endif

using namespace std;
using namespace robot2d;


void 
HwHandler::convert( const HwDriver::Robot2dData& internal, orca::Odometry2dData& network )
{
    network.timeStamp.seconds = internal.seconds;
    network.timeStamp.useconds = internal.useconds;

    network.pose.p.x = internal.x;
    network.pose.p.y = internal.y;
    network.pose.o = internal.o;
    
    network.motion.v.x = internal.vx;
    network.motion.v.y = internal.vy;
    network.motion.w = internal.w;
}

void 
HwHandler::convert( const orca::VelocityControl2dData& network, HwDriver::Robot2dCommand& internal )
{
    internal.vx = network.motion.v.x;
    internal.vy = network.motion.v.y;
    internal.w = network.motion.w;
}

HwHandler::HwHandler(
                 orcaice::Buffer<orca::Odometry2dData>& odometryPipe,
                 orcaice::Notify<orca::VelocityControl2dData>& commandPipe,
                 const orcaice::Context& context ) :
    odometryPipe_(odometryPipe),
    driver_(0),
    context_(context)
{
    // we'll handle incoming messages
    commandPipe.setNotifyHandler( this );

    // unsure about write status until we enable the driver
    writeStatusPipe_.set( false );

    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";
    
    config_.maxSpeed = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"MaxSpeed", 1.0 );
    config_.maxSideSpeed = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"MaxSideSpeed", 1.0 );
    config_.maxTurnrate = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    config_.isMotionEnabled = (bool)orcaice::getPropertyAsIntWithDefault( context_.properties(),
            prefix+"EnableMotion", 1 );   

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(),
            prefix+"Driver", "playerclient" );
            
    if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context_.tracer()->debug( "loading Player-Client driver",3);

        driver_ = new PlayerClientDriver( context_ );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'playerclient' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading Fake driver",3);
        driver_ = new FakeDriver( context_ );
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'playerclient', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
}

HwHandler::~HwHandler()
{
    delete driver_;
}

void
HwHandler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {

    //
    // Enable driver
    //
    while ( isActive() && driver_->enable() ) {
        context_.tracer()->warning("failed to enable the driver; will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // make we actually managed to enable the driver and are not shutting down
    if ( isActive() ) {
        context_.tracer()->debug("driver enabled",5);
    }
    
    // presumably we can write to the hardware
    writeStatusPipe_.set( true );
    std::string driverStatus = "";
    std::string currDriverStatus = "";
    
    int readStatus = -1;
    bool writeStatus = false;
    HwDriver::Robot2dData robot2dData;
    orca::Odometry2dData odometry2dData;

    //
    // This is the main loop
    // All operations inside are local.
    // We do not expect to catch any exceptions.
    //
    while( isActive() )
    {
        writeStatusPipe_.get( writeStatus );
        if ( writeStatus == false ) {
            context_.tracer()->error("failed to write data to Segway hardware. Repairing....");
            context_.tracer()->debug( "\n"+driver_->toString(), 1 );
            int repairStatus = driver_->repair();
            if ( repairStatus ) {
                context_.tracer()->error("failed to repair hardware. Moving on....");
            }
        }

        //
        // Read data from the hardware
        //
        // readTimer_.restart();
        readStatus = driver_->read( robot2dData, currDriverStatus );
        // cout<<"read: " << readTimer_.elapsed().toMilliSecondsDouble()<<endl;
    
        if ( readStatus==0 ) {
            // convert internal to network format
            convert( robot2dData, odometry2dData );
        
            // Stick it in the buffer so pullers can get it
            odometryPipe_.push( odometry2dData );

            if ( driverStatus != currDriverStatus ) {
//                 context_.status()->status( currDriverStatus );
                driverStatus = currDriverStatus;
            }
        } else {
            context_.tracer()->error("failed to read data from Segway hardware. Repairing....");
            driver_->repair();
        }
        
    } // while

    // we are no longer active

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("failed to disable driver");
    }
    else {
        context_.tracer()->debug("driver disabled",5);
    }

    //
    // unexpected exceptions
    //
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
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
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
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "exiting HwHandler thread...",5);
}

//
// This is the only direct incoming link from the outside.
// Here we handle command arriving through Platform2d interface.
//
void
HwHandler::handleData( const orca::VelocityControl2dData& origObj )
{
    //cout<<"handling: "<<orcaice::toString(obj)<<endl;
    
    // make a copy so we can apply limits
    orca::VelocityControl2dData obj = origObj;

/*
    // if we know we can't write, don't try again
    bool writeStatus = false;
    writeStatusPipe_.get( writeStatus );
    if ( !writeStatus ) {
        return;
    }
*/

    // check that platform motion is enabled
    if ( config_.isMotionEnabled==false ) {
        return;
    }

    // debug
    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    writeTimer_.restart();
    // this will certainly be 'late' when we throw an exception below
    if ( msecs>300 ) {
        cout<<"late: " << msecs <<endl;
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
    HwDriver::Robot2dCommand robot2dCommand;
    convert( obj, robot2dCommand );
    //
    // write to hardware
    //
    if( driver_->write( robot2dCommand ) == 0 ) {
        writeStatusPipe_.set( true );
    }
    else {
        std::string errorStr = "failed to write command data to hardware.";
        context_.tracer()->error( errorStr );
        // set local state to failure
        writeStatusPipe_.set( false );

        // debug
        bool statusCheck;
        writeStatusPipe_.get( statusCheck );
        assert( statusCheck == false && "write status should be set to FALSE" );

        // inform remote client of hardware failure
        throw orca::HardwareFailedException( errorStr );
    }
}
