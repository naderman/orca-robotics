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
#include <orcaice/orcaice.h>
#include "mainthread.h"

namespace player2orca
{

class Component : public orcaice::Component
{
public:
    Component();
    virtual void start();
    virtual void stop();
private:
    // main loop
    MainThread* mainThread_;
};


Component::Component() : 
    orcaice::Component( "Player2Orca" )
{
}

void
Component::start()
{
    //start the main loop
    mainThread_ = new MainThread( context() );
    mainThread_->start();
    // the rest is handled by the application/service
}

void
Component::stop()
{
    hydroutil::stopAndJoin( mainThread_ );
}

} // namespace

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    player2orca::Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
