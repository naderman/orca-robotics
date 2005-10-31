/*
 *  Orca: Components for robotics.
 *
 *  Copyright (C) 2004-2005
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

#include <orcaiceutil/application.h>
#include <orcaiceutil/component.h>
#include <iostream>

// implementations of Ice objects
#include "powerI.h"
#include "position2dI.h"
#include <orca/platform2d.h>

#include "testloop.h"

#include <orcaiceutil/connectutils.h>

using namespace std;

class SegwayTestMachine : public orcaiceutil::Component
{
public:
    SegwayTestMachine();
    virtual ~SegwayTestMachine();

    // component interface
    virtual void start();
    virtual void stop();

private:

    TestLoop* testLoop_;
};


SegwayTestMachine::SegwayTestMachine()
    : orcaiceutil::Component( "SegwayTestMachine" ), testLoop_(0)
{
}

SegwayTestMachine::~SegwayTestMachine()
{
    // do not delete testLoop_!!! It derives from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void SegwayTestMachine::start()
{
    //
    // PROVIDED : Position2dConsumer
    //
    // create servant and tell adapter about it (let it make up a globally unique name)
    Ice::ObjectPrx obj = adapter()->addWithUUID( new Position2dConsumerI );
    // make a direct proxy
    Ice::ObjectPrx prx = adapter()->createDirectProxy( obj->ice_getIdentity() );
    orca::Position2dConsumerPrx position2dCallbackPrx = orca::Position2dConsumerPrx::uncheckedCast( prx );

    //
    // Required : Power
    //
    // create servant and tell adapter about it (let it make up a globally unique name)
    obj = adapter()->addWithUUID( new PowerConsumerI );
    // make a direct proxy
    prx = adapter()->createDirectProxy( obj->ice_getIdentity() );
    orca::PowerConsumerPrx powerCallbackPrx = orca::PowerConsumerPrx::uncheckedCast( prx );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // MAIN DRIVER LOOP
    //
    testLoop_ = new TestLoop( position2dCallbackPrx, powerCallbackPrx );
    testLoop_->setCurrent( current() );
    testLoop_->start();

    // the rest is handled by the application/service
}

void SegwayTestMachine::stop()
{
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    SegwayTestMachine component;
    orcaiceutil::Application app( argc, argv, component );
    return app.main(argc, argv);
}
