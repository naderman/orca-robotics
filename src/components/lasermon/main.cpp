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

// implementations of Ice objects
#include "laserconsumer_i.h"

#include <orcaiceutil/connectutils.h>

class LaserMonComponent : public orcaiceutil::Component
{
public:
    LaserMonComponent();
    virtual ~LaserMonComponent();

    // component interface
    virtual void start();
    virtual void stop();
};


LaserMonComponent::LaserMonComponent()
    : orcaiceutil::Component( "LaserMon" )
{
}

LaserMonComponent::~LaserMonComponent()
{
    // do not delete inputLoop_!!! It derives from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void LaserMonComponent::start()
{
    //
    // PROVIDED : LaserConsumer
    //
    // create servant and tell adapter about it (let it make up a globally unique name)
    orca::LaserConsumerPtr statusConsumer = new LaserConsumerI;
    orcaiceutil::subscribeToIceStormTopic(
                    current(), (Ice::ObjectPtr&) statusConsumer, "Laser" );

    //
    // ENABLE ADAPTER
    //
    adapter()->activate();
    
    // the rest is handled by the application/service
}

void LaserMonComponent::stop()
{
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    LaserMonComponent component;
    orcaiceutil::Application app( argc, argv, component );
    return app.main(argc, argv);
}
