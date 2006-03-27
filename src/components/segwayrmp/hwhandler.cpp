/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>
#include <sstream>

#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

#include "hwhandler.h"
#include "fakedriver.h"
// segway rmp drivers
#ifdef HAVE_USB_DRIVER
    #include "usb/usbdriver.h"
#endif
//#ifdef HAVE_CAN_DRIVER
//    #include "can/candriver.h"
//#endif
#ifdef HAVE_PLAYERCLIENT_DRIVER
    #include "playerclient/playerclientdriver.h"
#endif

using namespace std;
using namespace orca;
using namespace segwayrmp;
using orcaice::operator<<;

HwHandler::HwHandler(
                 orcaice::PtrProxy<orca::Position2dDataPtr>    & position2dPipe,
                 orcaice::PtrNotify<orca::Velocity2dCommandPtr>& commandPipe,
                 orcaice::PtrProxy<orca::PowerDataPtr>         & powerPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & setConfigPipe,
                 orcaice::PtrProxy<orca::Platform2dConfigPtr>  & currentConfigPipe,
                 const orcaice::Context                        & context )
      : position2dPipe_(position2dPipe),
        powerPipe_(powerPipe),
        setConfigPipe_(setConfigPipe),
        currentConfigPipe_(currentConfigPipe),
        position2dData_(new Position2dData),
        powerData_(new PowerData),
        driver_(0),
        context_(context)
{
    // we'll handle incoming messages
    commandPipe.setNotifyHandler( this );

    // unsure about write status until we enable the driver
    writeStatusPipe_.set( false );

    // set up data structure for 3 batteries
    BatteryData bd;
    for ( int i=0; i<3; ++i ) {
        powerData_->batteries.push_back( bd );
    }
    powerData_->batteries[0].name = "main-front";
    powerData_->batteries[1].name = "main-rear";
    powerData_->batteries[2].name = "ui";

    // this is the last place we can throw exceptions from.
    // after this the thread will be launched
    init();
}

HwHandler::~HwHandler()
{
    delete driver_;
}

void
HwHandler::init()
{
    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";
    
    config_.maxSpeed = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"MaxTurnrate", 40.0 )*DEG2RAD_RATIO;

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(),
            prefix+"Driver", "segwayrmpusb" );
    if ( driverName == "segwayrmpusb" )
    {
#ifdef HAVE_USB_DRIVER
        context_.tracer()->debug( "loading USB driver",3);
        driver_ = new UsbDriver();
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'usb' because it was not built!" );
#endif
    }
    else if ( driverName == "playerclient" )
    {
#ifdef HAVE_PLAYERCLIENT_DRIVER
        context_.tracer()->debug( "loading Player-Client driver",3);

    // experiment
//         driver_ = new PlayerClientDriver(
//                 context_.properties()->getPropertiesForPrefix( context_.tag()+".Config.PlayerClient" ) );

        std::string playerclientPrefix = prefix + "PlayerClient.";
        std::string playerHost = orcaice::getPropertyWithDefault( context_.properties(),
                playerclientPrefix+"Host", "localhost" );
        int playerPort = orcaice::getPropertyAsIntWithDefault( context_.properties(),
                playerclientPrefix+"Port", 6665 );
        driver_ = new PlayerClientDriver( playerHost.c_str(), playerPort );
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver 'playerclient' because it was not built!" );
#endif
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading Fake driver",3);
        driver_ = new FakeDriver;
    }
    else {
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'segwayrmpusb', 'playerclient', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
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
    // presumably we can write to the hardware
    writeStatusPipe_.set( true );
    context_.tracer()->debug("driver enabled",5);

    std::string driverStatus = "";
    std::string currDriverStatus = "";
    
    int readStatus = -1;
    bool writeStatus = false;

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
        readStatus = driver_->read( position2dData_, powerData_, currDriverStatus );
        // cout<<"read: " << readTimer_.elapsed().toMilliSecondsDouble()<<endl;
    
        if ( readStatus==0 ) {
            // Stick it in the buffer so pullers can get it
            position2dPipe_.set( position2dData_ );
            powerPipe_.set( powerData_ );

            if ( driverStatus != currDriverStatus ) {
                context_.tracer()->status( currDriverStatus );
                driverStatus = currDriverStatus;
            }
        } else {
            context_.tracer()->error("failed to read data from Segway hardware. Repairing....");
            driver_->repair();
        }

        // Have any configuration requests arrived?
        if ( !setConfigPipe_.isNewData() )
        {
            // set configs
        }
        
    } // while

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
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception.");
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
    // otherwise there's a possibility of lock up in Thread::stop()
    while ( isActive() ) {
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(10));
    }
}

//
// This is the only direct incoming link from the outside.
// Here we handle command arriving through Platform2d interface.
//
void
HwHandler::handleData( const orca::Velocity2dCommandPtr & obj )
{
    //cout<<"handling: "<<orcaice::toString(obj)<<endl;

/*
    // if we know we can't write, don't try again
    bool writeStatus = false;
    writeStatusPipe_.get( writeStatus );
    if ( !writeStatus ) {
        return;
    }
*/

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
    if ( fabs(obj->motion.v.x) > config_.maxSpeed ) {
        obj->motion.v.x =
                (obj->motion.v.x / fabs(obj->motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(obj->motion.w) > config_.maxTurnrate ) {
        obj->motion.w =
                (obj->motion.w / fabs(obj->motion.w)) * config_.maxTurnrate;
    }

    //
    // write to hardware
    //
    if( driver_->write( obj ) == 0 ) {
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
