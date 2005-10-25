/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#include "rmpmainloop.h"
#include <IceUtil/Time.h>

// segway rmp drivers
#ifdef HAVE_USB_DRIVER
    #include "rmpusb/rmpusbdriver.h"
#endif
#ifdef HAVE_CAN_DRIVER
    #include "rmpcan/rmpcandriver.h"
#endif
#ifdef HAVE_PLAYER_DRIVER
    #include "rmpfake/rmpplayerdriver.h"
#endif
#include "rmpfakedriver.h"

#include <orcaiceutil/orcaiceutil.h>
#include <orcaiceutil/mathdefs.h>
#include <orcaiceutil/exceptions.h>

using namespace std;
using namespace orca;
using orcaiceutil::operator<<;

RmpMainLoop::RmpMainLoop(
                 orcaiceutil::PtrBuffer<orca::Position2dDataPtr>    & position2dProxy,
                 orcaiceutil::PtrNotify                             & commandNotify,
                 orcaiceutil::PtrBuffer<orca::PowerDataPtr>         & powerProxy,
                 orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer,
                 orcaiceutil::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer,
                 const orca::Position2dConsumerPrx                  & position2dConsumer,
                 const orca::PowerConsumerPrx                       & powerConsumer ) :
        position2dProxy_(position2dProxy),
        commandNotify_(commandNotify),
        powerProxy_(powerProxy),
        setConfigBuffer_(setConfigBuffer),
        currentConfigBuffer_(currentConfigBuffer),
        position2dConsumer_(position2dConsumer),
        powerConsumer_(powerConsumer),
        driver_(0),
        driverType_(RmpDriver::UNKNOWN_DRIVER)
{
    commandNotify_.setNotifyHandler( this );
}

RmpMainLoop::~RmpMainLoop()
{
    delete driver_;
}

void RmpMainLoop::readConfigs()
{
    //
    // Read settings
    //
    config_.maxSpeed = orcaiceutil::getPropertyAsDoubleWithDefault( current_.properties(),
            "SegwayRmp.Config.MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaiceutil::getPropertyAsDoubleWithDefault( current_.properties(),
            "SegwayRmp.Config.MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    string driverName = orcaiceutil::getPropertyWithDefault( current_.properties(), 
            "SegwayRmp.Config.Driver", "usb" );

    if ( driverName == "usb" ) {
        driverType_ = RmpDriver::USB_DRIVER;
    }
    else if ( driverName == "fake" ) {
        driverType_ = RmpDriver::FAKE_DRIVER;
    }
    else {
        driverType_ = RmpDriver::UNKNOWN_DRIVER;
        string errorStr = "Unknown driver type. Cannot talk to hardware.";
        current_.logger()->trace("remote",errorStr);
        throw orcaiceutil::OrcaIceUtilHardwareException( ERROR_INFO, errorStr );
    }
        
}

void RmpMainLoop::run()
{
    readConfigs();

    // based on the config parameter, create the right driver
    switch ( driverType_ )
    {
        case RmpDriver::USB_DRIVER :
#ifdef HAVE_USB_DRIVER
            current_.logger()->trace("remote","loading USB driver");
            driver_ = new RmpUsbDriver( current_ );
#endif
            break;
        case RmpDriver::CAN_DRIVER :
#ifdef HAVE_CAN_DRIVER
            current_.logger()->trace("remote","loading CAN driver");
            driver_ = new RmpCanDriver;
#endif
            break;
        case RmpDriver::PLAYER_CLIENT_DRIVER :
#ifdef HAVE_PLAYER_DRIVER
            current_.logger()->trace("remote","loading Player-Client driver");
            driver_ = new RmpPlayerClientDriver;
#endif
            break;
        case RmpDriver::FAKE_DRIVER :
            current_.logger()->trace("remote","loading Fake driver");
            driver_ = new RmpFakeDriver;
            break;
        case RmpDriver::UNKNOWN_DRIVER :
            string errorStr = "Unknown driver type. Cannot talk to hardware.";
            current_.logger()->trace("remote",errorStr);
            throw orcaiceutil::OrcaIceUtilException( ERROR_INFO, errorStr );
    }
    while ( driver_->enable() ) {
        current_.logger()->trace("remote","failed to enable the driver");
        sleep(1);
    }
    current_.logger()->trace("remote","driver enabled");

    // init internal data storage
    orca::Position2dDataPtr position2dData = new Position2dData;
    orca::Velocity2dCommandPtr command = new Velocity2dCommand;
    orca::PowerDataPtr powerData = new PowerData;

    // set up data structure for 3 batteries
    BatteryData bd;
    for ( int i=0; i<3; ++i ) {
        powerData->batteries.push_back( bd );
    }
    powerData->batteries[0].name = "main-front";
    powerData->batteries[1].name = "main-rear";
    powerData->batteries[2].name = "io";

    while( isActive() )
    {
        // Read data from the hardware
        readTimer_.restart();
        if ( driver_->read( position2dData, powerData ) ) {
            current_.logger()->trace("remote","failed to read from Segway");
        }
        //cout<<"read: " << readTimer_.stop().toMilliSecondsDouble()<<endl;
        
        // push data to IceStorm
        try
        {
            position2dConsumer_->consumeData( position2dData );
            powerConsumer_->setData( powerData );
        }
        catch ( const Ice::ConnectionRefusedException & e )
        {
            current_.logger()->trace("remote","lost connection to IceStorm");
        }
        catch ( const Ice::CommunicatorDestroyedException & e )
        {
            // it's ok, the communicator may already be destroyed
        }

        // Stick it in the buffer so pullers can get it
        position2dProxy_.push( position2dData );
        powerProxy_.push( powerData );

        // Have any configuration requests arrived?
        if ( !setConfigBuffer_.isEmpty() )
        {
            // set configs
        }
    }

    // reset the hardware
    if ( driver_->disable() ) {
        current_.logger()->trace("remote","failed to disable driver");
    }
    current_.logger()->trace("remote","driver disabled");
}


void RmpMainLoop::handleData( const Ice::ObjectPtr & obj )
{
    //cout<<"write: " << writeTimer_.stop().toMilliSecondsDouble()<<endl;

    orca::Velocity2dCommandPtr command = Velocity2dCommandPtr::dynamicCast( obj );

    // apply max limits
    if ( fabs(command->motion.v.x) > config_.maxSpeed ) {
        command->motion.v.x =
                (command->motion.v.x / fabs(command->motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(command->motion.w) > config_.maxTurnrate ) {
        command->motion.w =
                (command->motion.w / fabs(command->motion.w)) * config_.maxTurnrate;
    }

    //cout<<"handling: "<<command<<endl;

    // write to hardware
    if( driver_->sendMotionCommand( command ) != 0 )
    {
        string errString = "failed to write to segway";
        throw orcaiceutil::OrcaIceUtilHardwareException( ERROR_INFO, errString );
    }

    writeTimer_.restart();
}
