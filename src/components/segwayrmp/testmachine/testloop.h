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

#ifndef ORCA2_SEGWAYRMP_TESTMACHINE_TEST_LOOP_H
#define ORCA2_SEGWAYRMP_TESTMACHINE_TEST_LOOP_H

#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <orcaiceutil/thread.h>
#include <orcaiceutil/current.h>
#include <orca/position2d.h>
#include <orca/power.h>

#include <string>

class TestLoop : public orcaiceutil::Thread
{
public:

    TestLoop( const orca::Position2dConsumerPrx, const orca::PowerConsumerPrx );
    ~TestLoop();

    void setCurrent( const orcaiceutil::Current & current ) { current_=current; };

    virtual void run();

private:

    // remote object
    orca::Position2dPrx position2dPrx_;
    orca::PowerPrx powerPrx_;

    // local object to receive data
    orca::Position2dConsumerPrx position2dCallbackPrx_;
    orca::PowerConsumerPrx powerCallbackPrx_;

    // component current context
    orcaiceutil::Current current_;
};

#endif

