/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/orcaice.h>

#include "browserhandler.h"
#include "iostream/iostreamdriver.h"

using namespace std;

namespace probe
{

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();
};


Component::Component()
    : orcaice::Component( "Probe", orcaice::HomeInterface )
{
}

Component::~Component()
{
}

void Component::start()
{
    //
    // enable network connections
    //
    activate();

    orcaice::Buffer<BrowserEvent> eventPipe;

    std::string prefix = tag()+".Config.";

    // which driver to load?
    std::string driverName = orcaice::getPropertyWithDefault( properties(),
                                prefix+"Driver", "iostream" );

    // generic interface to the user interface
    UserDriver* userDriver;
    // generic interface to the display interface
    DisplayDriver* displayDriver;
    
    if ( driverName == "qt" ) 
    {
#ifdef HAVE_QT_DRIVER        
        tracer()->print( "loading Qt driver");
        userDriver = new UserQtDriver();
#else
        throw orcaice::Exception( ERROR_INFO, "Can't instantiate driver type 'qt' because it was not compiled." );
#endif
    }
    else if ( driverName == "iostream" ) 
    {
        tracer()->print( "loading iostream driver");
        // this driver is not interactive, so it does not need networkHandler
        IostreamDriver* iostreamDriver = new IostreamDriver( &eventPipe );
        userDriver = iostreamDriver;
        displayDriver = iostreamDriver;
    }
    else {
        std::string errorStr = "Unknown driver type. Cannot talk to hardware.";
        tracer()->error( errorStr);
        throw orcaice::HardwareException( ERROR_INFO, errorStr );
    }


    BrowserHandler browserHandler( eventPipe, *displayDriver, context() );
    browserHandler.start();

    // for Qt driver, this will not return
    userDriver->enable();

                                
    // normally ctrl-c handler does this, now we have to because MainBubble keeps the thread 
    context().communicator()->shutdown();

    // the rest is handled by the application/service
}

void Component::stop()
{
    // this component cannot quit nicely
}

} // namespace

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    probe::Component component;
    // do not install ctrl-c handler
    bool installCtrlCHandler = false;
    orcaice::Application app( component, installCtrlCHandler );
    return app.main(argc, argv);
}
