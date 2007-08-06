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
#   include "playerclient/playerclientdriver.h"
#endif

using namespace std;
using namespace robot2d;

HwHandler::HwHandler(
                 orcaice::Proxy<Data>&          dataPipe,
                 orcaice::Notify<Command>&      commandPipe,
                 const orca::VehicleDescription& descr,
                 const orcaice::Context&        context ) :
    dataPipe_(dataPipe),
    driver_(0),
    context_(context)
{
    // we'll handle incoming messages
    commandPipe.setNotifyHandler( this );

    // unsure about write status until we enable the driver
    writeStatusPipe_.set( false );

    //
    // configure settings
    //
    std::string prefix = context_.tag() + ".Config.";

    // we support only one type of vehicle control
    orca::VehicleControlVelocityDifferentialDescriptionPtr controlDescr = 
            orca::VehicleControlVelocityDifferentialDescriptionPtr::dynamicCast( descr.control );
    if ( !controlDescr )
        throw orcaice::Exception( ERROR_INFO, "Can only handle vehicles with differential drive." );
    if ( controlDescr->maxForwardSpeed != controlDescr->maxReverseSpeed ) 
        throw orcaice::Exception( ERROR_INFO, "Can't handle max forward speed != max reverse speed." );

    config_.maxSpeed = controlDescr->maxForwardSpeed;
    config_.maxSideSpeed = 0.0;
    config_.maxTurnrate = controlDescr->maxTurnrate;
    config_.isMotionEnabled = (bool)orcaice::getPropertyAsIntWithDefault( context_.properties(),
                                                                          prefix+"EnableMotion", 1 );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( 
            context_.properties(), prefix+"Driver", "playerclient" );
            
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
        context_.tracer()->warning("HwHandler: failed to enable the driver; will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // make we actually managed to enable the driver and are not shutting down
    if ( isActive() ) {
        context_.tracer()->debug("driver enabled",2);
    }
    
    // presumably we can write to the hardware
    writeStatusPipe_.set( true );
    std::string driverStatus = "";
    std::string currDriverStatus = "";
    
    int readStatus = -1;
    bool writeStatus = false;

    // temp storage object
    robot2d::Data data;

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
        readStatus = driver_->read( data, currDriverStatus );
        // cout<<"read: " << readTimer_.elapsed().toMilliSecondsDouble()<<endl;
    
        if ( readStatus==0 ) {        
            // Stick it in the buffer so pullers can get it
            dataPipe_.set( data );

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
HwHandler::handleData( const Command& command )
{
//    cout<<"handling: "<<toString(command)<<endl;

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
    if( driver_->write( limitedCommand ) == 0 ) {
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

    stringstream ssDebug;
    ssDebug << "HwHandler: Received and wrote command: vx = " << command.vx << " m/s, w = " << command.w*180.0/M_PI << " deg/s";
    context_.tracer()->debug( ssDebug.str() );
}
