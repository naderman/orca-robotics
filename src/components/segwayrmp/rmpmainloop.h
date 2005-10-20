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

#ifndef ORCA2_RMP_MAIN_LOOP_H
#define ORCA2_RMP_MAIN_LOOP_H

#include <orcaiceutil/thread.h>
#include <orcaiceutil/ptrbuffer.h>

#include "rmpdriver.h"

#include <orca/platform2d.h>
#include <orca/power.h>


//! Note: this thing self-destructs when run() returns.
class RmpMainLoop : public orcaiceutil::Thread
{
public:

    RmpMainLoop( orcaiceutil::PtrBuffer<orca::Position2dDataPtr>    *position2dProxy,
                 orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> *commandProxy,
                 orcaiceutil::PtrBuffer<orca::PowerDataPtr>         *powerProxy,
                 const orca::Position2dConsumerPrx & position2dConsumer,
                 const orca::PowerConsumerPrx & powerConsumer );
    virtual ~RmpMainLoop();

    void setupConfigs( const Ice::PropertiesPtr & );

    virtual void run();

private:

    // network/driver interface
    orcaiceutil::PtrBuffer<orca::Position2dDataPtr>    * position2dProxy_;
    orcaiceutil::PtrBuffer<orca::Velocity2dCommandPtr> * commandProxy_;
    orcaiceutil::PtrBuffer<orca::PowerDataPtr>         * powerProxy_;

    // IceStorm consumers
    orca::Position2dConsumerPrx position2dConsumer_;
    orca::PowerConsumerPrx powerConsumer_;

    // generic interface to the hardware
    RmpDriver* driver_;

    enum DriverType
    {
        USB_DRIVER,
        CAN_DRIVER,
        PLAYER_CLIENT_DRIVER,
        FAKE_DRIVER,
        UNKNOWN_DRIVER
    };

    DriverType driverType_;

    // Maximum allowd speeds [m/s], [rad/s]
    double maxSpeed_;
    double maxTurnrate_;

};

#endif
