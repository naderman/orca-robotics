/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <iostream>
#include "mainloop.h"

using namespace std;

class LaserTestComponent : public orcaice::Component
{
public:
    LaserTestComponent();
    virtual ~LaserTestComponent();

    // component interface
    virtual void start();
    virtual void stop();

private:
    MainLoop *mainLoop_;
};


LaserTestComponent::LaserTestComponent()
    : orcaice::Component( "LaserTest" )
{
}

LaserTestComponent::~LaserTestComponent()
{
    // do not delete inputLoop_!!! It derives from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanent must
//          be declared as member variables.
void 
LaserTestComponent::start()
{
    mainLoop_ = new MainLoop( context() );
    mainLoop_->start();
}

void 
LaserTestComponent::stop()
{
    hydroutil::stopAndJoin( mainLoop_ );
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    LaserTestComponent component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
