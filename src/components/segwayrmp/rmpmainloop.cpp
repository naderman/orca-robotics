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

#include <orcaice/orcaice.h>
#include <orcaice/mathdefs.h>
#include <orcaice/exceptions.h>

using namespace std;
using namespace orca;

RmpMainLoop::RmpMainLoop(
                 orcaice::PtrBuffer<orca::Position2dDataPtr>    & position2dProxy,
                 orcaice::PtrNotify                             & commandNotify,
                 orcaice::PtrBuffer<orca::PowerDataPtr>         & powerProxy,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & setConfigBuffer,
                 orcaice::PtrBuffer<orca::Platform2dConfigPtr>  & currentConfigBuffer,
                 const orca::Position2dConsumerPrx                  & position2dPublisher,
                 const orca::PowerConsumerPrx                       & powerPublisher ) :
        position2dProxy_(position2dProxy),
        commandNotify_(commandNotify),
        powerProxy_(powerProxy),
        setConfigBuffer_(setConfigBuffer),
        currentConfigBuffer_(currentConfigBuffer),
        position2dPublisher_(position2dPublisher),
        powerPublisher_(powerPublisher),
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
    config_.maxSpeed = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.MaxSpeed", 1.0 );
    config_.maxTurnrate = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    config_.position2dPublishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.position2dPublishInterval", 0.1 );
    config_.powerPublishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.powerPublishInterval", 10.0 );
    config_.statusPublishInterval = orcaice::getPropertyAsDoubleWithDefault( context_.properties(),
            "SegwayRmp.Config.statusPublishInterval", 60.0 );
    string driverName = orcaice::getPropertyWithDefault( context_.properties(),
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
        context_.tracer()->error( errorStr);
        throw orcaice::HardwareException( ERROR_INFO, errorStr );
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
            context_.tracer()->debug( "loading USB driver",3);
            driver_ = new RmpUsbDriver();
#endif
            break;
        case RmpDriver::CAN_DRIVER :
#ifdef HAVE_CAN_DRIVER
            context_.tracer()->debug( "loading CAN driver",3);
            driver_ = new RmpCanDriver;
#endif
            break;
        case RmpDriver::PLAYER_CLIENT_DRIVER :
#ifdef HAVE_PLAYER_DRIVER
            context_.tracer()->debug( "loading Player-Client driver",3);
            driver_ = new RmpPlayerClientDriver;
#endif
            break;
        case RmpDriver::FAKE_DRIVER :
            context_.tracer()->debug( "loading Fake driver",3);
            driver_ = new RmpFakeDriver;
            break;
        case RmpDriver::UNKNOWN_DRIVER :
            string errorStr = "Unknown driver type. Cannot talk to hardware.";
            context_.tracer()->error(errorStr);
            throw orcaice::Exception( ERROR_INFO, errorStr );
    }
    
    //
    // Enable driver
    //
    while ( isActive() && driver_->enable() ) {
        context_.tracer()->warning("failed to enable the driver; will try again.");
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
    // check again to make sure we are not being terminated
    if ( !isActive() ) {
        return;
    }
    context_.tracer()->debug("driver enabled",5);

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
    powerData->batteries[2].name = "ui";

    int readStatus = 1;

    while( isActive() )
    {
        // debug
        //cout<<"frame: " << readTimer_.elapsed().toMilliSecondsDouble()<<endl;

        // Read data from the hardware
        readTimer_.restart();
       
        if ( (readStatus = driver_->read( position2dData, powerData )) ) {
            context_.tracer()->error("failed to read data from Segway hardware.");
            // indicate some fault state here
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        }
        
        //cout<<"read: " << readTimer_.elapsed().toMilliSecondsDouble()<<endl;


        // only we maganed to read successfuly
        if ( readStatus==0 ) {
//debug
std::ostringstream os;
os << "cu/ui :"<<powerData->batteries[0].voltage<<" "
   <<powerData->batteries[2].voltage;
context_.tracer()->debug( os.str() );
        
            // push data to IceStorm
            try
            {
                if ( position2dPublishTimer_.elapsed().toSecondsDouble()>config_.position2dPublishInterval ) {
                    position2dPublisher_->setData( position2dData );
                    position2dPublishTimer_.restart();
                }
                if ( powerPublishTimer_.elapsed().toSecondsDouble()>config_.powerPublishInterval ) {
                    powerPublisher_->setData( powerData );    
                    std::ostringstream os;
                    os << "cu/ui :"<<powerData->batteries[0].voltage<<" "
                                   <<powerData->batteries[2].voltage;
                    context_.tracer()->debug( os.str() );
                    powerPublishTimer_.restart();
                }
                if ( statusPublishTimer_.elapsed().toSecondsDouble()>config_.statusPublishInterval ) {
                    //cout<<"sending heartbeat"<<endl;
                    context_.tracer()->heartbeat("status OK");
                    statusPublishTimer_.restart();
                }
            }
            catch ( const Ice::ConnectionRefusedException & e )
            {
                context_.tracer()->warning("lost connection to IceStorm");
            }
            catch ( const Ice::CommunicatorDestroyedException & e )
            {
                // it's ok, the communicator may already be destroyed
            }

            // we are still inside if(readStatus==0)
            // Stick it in the buffer so pullers can get it (do it after IceStorm 'cause it's less time sensitive)
            position2dProxy_.push( position2dData );
            powerProxy_.push( powerData );
        }

        // Have any configuration requests arrived?
        if ( !setConfigBuffer_.isEmpty() )
        {
            // set configs
        }

        // time the loop rate here for now
        //IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(100));
    }

    // reset the hardware
    if ( driver_->disable() ) {
        context_.tracer()->warning("failed to disable driver");
    }
    context_.tracer()->debug("driver disabled",5);
}


void RmpMainLoop::handleData( const Ice::ObjectPtr & obj )
{
    //cout<<"write: " << writeTimer_.elapsed().toMilliSecondsDouble()<<endl;
    double msecs=writeTimer_.elapsed().toMilliSecondsDouble();
    if ( msecs>300 ) {
        cout<<"late: " << msecs <<endl;
    }
    orca::Velocity2dCommandPtr command = orca::Velocity2dCommandPtr::dynamicCast( obj );

    // apply max limits
    if ( fabs(command->motion.v.x) > config_.maxSpeed ) {
        command->motion.v.x =
                (command->motion.v.x / fabs(command->motion.v.x)) * config_.maxSpeed;
    }
    if ( fabs(command->motion.w) > config_.maxTurnrate ) {
        command->motion.w =
                (command->motion.w / fabs(command->motion.w)) * config_.maxTurnrate;
    }

    //cout<<"handling: "<<orcaice::toString(command)<<endl;

    // write to hardware
    if( driver_->sendMotionCommand( command ) != 0 )
    {
        orca::HardwareFailedException e;
        e.what = "failed to write command to segway hardware.";
        throw e;
    }

    writeTimer_.restart();
}
