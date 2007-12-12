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

#include <orcaice/orcaice.h>
#include "mainthread.h"

using namespace std;

namespace orcamon
{

class Component : public orcaice::Component
{
public:
    Component();

    // from Component
    virtual void start();
    virtual void stop();
    
private:
    hydroutil::Thread* mainLoop_;
};

Component::Component() : 
    orcaice::Component( "OrcaMon", orcaice::HomeInterface ),
    mainLoop_(0)
{
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void Component::start()
{
    //
    // MAIN DRIVER LOOP
    //    
    mainLoop_ = new MainThread( context() );
    mainLoop_->start();

    // the rest is handled by the application/service
}

void
Component::stop()
{
    tracer().debug( "stopping component", 5 );
    hydroutil::stopAndJoin( mainLoop_ );
    tracer().debug( "stopped component", 5 );
}

} // namespace

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    orcamon::Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
