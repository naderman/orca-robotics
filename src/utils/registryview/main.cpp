/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <QApplication>

#include <orcaice/application.h>
#include <orcaice/component.h>

#include <orcaqcm/networkhandler.h>
#include "mainwin.h"

#include <orcaice/orcaice.h>

using namespace std;

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
    : orcaice::Component( "RegistryView", orcaice::HomeInterface )
{
}

Component::~Component()
{
}

void Component::start()
{
    //
    // Read settings
    //
    std::string prefix = tag() + ".Config.";
    
    double refreshInterval = orcaice::getPropertyAsDoubleWithDefault( properties(),
            prefix+"RefreshInterval", 120.0 );
            
    //
    // enable network connections
    //
    // Home interface only
    // this may throw, but may as well quit right then
    activate();

    // this runs in its own thread
    qorcacm::NetworkHandler networkHandler( context() );
    networkHandler.start();
    
    // Set up QT stuff
    char **v = 0;
    int c = 0;
    QApplication qapp(c,v);

    MainWindow gui( &networkHandler, refreshInterval );
    gui.show();

    // note: this does not return!
    qapp.exec();

    // normally ctrl-c handler does this, now we have to because GUI keeps the thread
    context().communicator()->shutdown();

    // the rest is handled by the application/service
}

void Component::stop()
{
    tracer()->debug("stopping component",5);
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    Component component;
    // do not install ctrl-c handler
    bool installCtrlCHandler = false;
    orcaice::Application app( component, installCtrlCHandler );
    return app.main(argc, argv);
}
