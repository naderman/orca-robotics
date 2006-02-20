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

#include "hwhandler.h"
#include <IceUtil/Time.h>

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
#include "fakedriver.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;
using orcaice::operator<<;

HwHandler::HwHandler(
                 orcaice::PtrBuffer<orca::Position2dDataPtr>    & position2dProxy,
                 orcaice::PtrNotify                             & commandNotify,
                 orcaice::PtrBuffer<orca::PowerDataPtr>         & powerProxy,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer,
                 const orcaice::Context                         & context ) :
        position2dProxy_(position2dProxy),
        commandNotify_(commandNotify),
        powerProxy_(powerProxy),
        setConfigBuffer_(setConfigBuffer),
        currentConfigBuffer_(currentConfigBuffer),
        position2dData_(new Position2dData),
        commandData_(new Velocity2dCommand),
        powerData_(new PowerData),
        driver_(0),
        context_(context),
        writeStatus_(-1)
{
    // we'll handle incoming messages
    commandNotify_.setNotifyHandler( this );

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

void HwHandler::init()
{
    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";
    
    config_.maxSpeed = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            prefix+"MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    string driverName = orcaice::getPropertyWithDefault( context_.properties(),
            prefix+"Driver", "usb" );

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

        std::string prefix = context_.tag() + ".Config.PlayerClient.";
        std::string playerHost = orcaice::getPropertyWithDefault( context_.properties(),
                prefix+"Host", "localhost" );
        int playerPort = orcaice::getPropertyAsIntWithDefault( context_.properties(),
                prefix+"Port", 6665 );
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
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
}

void HwHandler::run()
{
    //
    // Enable driver
    //
    while ( isActive() && driver_->enable() ) {
        context_.tracer()->warning("failed to enable the driver; will try again in 2 seconds.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
    //writeStatus_ = 0;
    context_.tracer()->debug("driver enabled",5);

    int readStatus = -1;

    while( isActive() )
    {
    /*
        if ( writeStatus_==0 ) {
            // carry on
        } else {
            context_.tracer()->error("failed to write data to Segway hardware.");
            driver_->repair();
        }
    */
        // Read data from the hardware
//         readTimer_.restart();
        readStatus = driver_->read( position2dData_, powerData_, rmpStatus_ );
//         cout<<"read: " << readTimer_.elapsed().toMilliSecondsDouble()<<endl;
    
        if ( readStatus==0 ) {
            // Stick it in the buffer so pullers can get it
            position2dProxy_.push( position2dData_ );
            powerProxy_.push( powerData_ );
        } else {
            context_.tracer()->error("failed to read data from Segway hardware. Repairing....");
            driver_->repair();
        }

        // Have any configuration requests arrived?
        if ( !setConfigBuffer_.isEmpty() )
        {
            // set configs
        }
        
    }

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("failed to disable driver");
    }
    context_.tracer()->debug("driver disabled",5);
}

void HwHandler::handleData( const Ice::ObjectPtr & obj )
{
/*
    if ( writeStatus_ != 0 ) {
        return;
    }
*/
    //cout<<"write: " << writeTimer_.elapsed().toMilliSecondsDouble()<<endl;
    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    // this will certainly be 'late' when we throw an exception below
    if ( msecs>300 ) {
        cout<<"late: " << msecs <<endl;
    }
    orca::Velocity2dCommandPtr commandData_ = orca::Velocity2dCommandPtr::dynamicCast( obj );

    // apply max limits
    if ( fabs(commandData_->motion.v.x) > config_.maxSpeed ) {
        commandData_->motion.v.x =
                (commandData_->motion.v.x / fabs(commandData_->motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(commandData_->motion.w) > config_.maxTurnrate ) {
        commandData_->motion.w =
                (commandData_->motion.w / fabs(commandData_->motion.w)) * config_.maxTurnrate;
    }

    //cout<<"handling: "<<orcaice::toString(commandData_)<<endl;

    // write to hardware
    if( driver_->write( commandData_ ) == 0 ) {
        //writeStatus_ = 0;
    }
    else {
        context_.tracer()->error("failed to write data to Segway hardware.");
        // set local state to failure
        //writeStatus_ = -1;

        // inform remote client of hardware failure
        throw orca::HardwareFailedException( "failed to write command data to hardware." );
    }

    writeTimer_.restart();
}
