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

// segway rmp drivers
#include "rmpusb/rmpusbdriver.h"
//#include "rmpcan/rmpcandriver.h"
#include "rmpfake/rmpfakedriver.h"

#include <orcaiceutil/orcaiceutil.h>
#include <orcaiceutil/mathdefs.h>

using namespace std;
using namespace orca;
using orcaiceutil::operator<<;

RmpMainLoop::RmpMainLoop(
                 orcaiceutil::PtrBuffer* position2dProxy,
                 orcaiceutil::PtrBuffer* commandProxy,
                 orcaiceutil::PtrBuffer* powerProxy,
                 const orca::Position2dConsumerPrx & position2dConsumer,
                 const orca::PowerConsumerPrx & powerConsumer ) :
        position2dProxy_(position2dProxy), commandProxy_(commandProxy), powerProxy_(powerProxy),
        position2dConsumer_(position2dConsumer), powerConsumer_(powerConsumer)
{
}

RmpMainLoop::~RmpMainLoop()
{
}

void RmpMainLoop::setupConfigs( const Ice::PropertiesPtr & properties )
{
    //
    // Read settings
    //
    maxSpeed_ = orcaiceutil::getPropertyAsDoubleWithDefault( properties,
            "SegwayRmp.Config.MaxSpeed", 1.0 );
    maxTurnrate_ = orcaiceutil::getPropertyAsDoubleWithDefault( properties,
            "SegwayRmp.Config.MaxTurnrate", 40.0 )*DEG2RAD_RATIO;
    string driverName = properties->getPropertyWithDefault(
            "SegwayRmp.Config.DriverName", "RmpUsb" );
    cout<<"properties: maxspeed="<<maxSpeed_<<", maxturn="<<maxTurnrate_<<
            ", driver="<<driverName<<endl;
}

void RmpMainLoop::run()
{
    cout<<"RmpMainLoop::run: starting nicely"<<endl;

    // based on some config parameter, create the right driver
    if ( 0 ) {
        driver_ = new RmpUsbDriver();
    }
    else {
        driver_ = new RmpFakeDriver();
    }

    // init internal data storage
    orca::Position2dDataPtr position2dData = new Position2dData;
    orca::Velocity2dCommandPtr commandData = new Velocity2dCommand;
    // set up data structure for 3 batteries
    orca::PowerDataPtr powerData = new PowerData;
    BatteryData bd;
    for ( int i=0; i<3; ++i ) {
        powerData->batteries.push_back( bd );
    }
    powerData->batteries[0].name = "main-front";
    powerData->batteries[1].name = "main-rear";
    powerData->batteries[2].name = "io";

    while( isActive() )
    {
        // Read from the hardware
        driver_->read( position2dData, powerData );      
        
        // push it to IceStorm
        position2dConsumer_->consumeData( position2dData );
        powerConsumer_->setData( powerData );

        // Stick it in the buffer so pullers can get it
        position2dProxy_->push( position2dData );
        powerProxy_->push( powerData );        

        // Have any configuration requests arrived?
        //! @todo
    }

    // Shut down the hardware
    driver_->disable();
}
