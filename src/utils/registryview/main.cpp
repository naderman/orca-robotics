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
#include <hydroiceutil/jobqueue.h>

#include <QApplication>
#include "mainwin.h"

class Component : public orcaice::Component
{
public:
    Component() :
        orcaice::Component( "RegistryView", orcaice::NoStandardInterfaces ) {};

    // from orcaice::Component
    virtual void start();
    virtual void stop() {};
};

void 
Component::start()
{
    std::string prefix = tag() + ".Config.";
    
    double refreshInterval = orcaice::getPropertyAsDoubleWithDefault( properties(),
            prefix+"RefreshInterval", 120.0 );
            
    //
    // enable network connections
    //
    activate();

    //
    // Start job queue
    //
    // standard config is exactly what we want
    hydroiceutil::JobQueue jobQueue( context().tracer() ) ;
    
    // Set up QT stuff
    char **v = 0;
    int c = 0;
    QApplication qapp(c,v);

    MainWindow gui( &jobQueue, refreshInterval, context() );
    gui.show();

    // note: this does not return!
    qapp.exec();

    // normally ctrl-c handler does this, now we have to because GUI keeps the thread
    context().communicator()->shutdown();
}

int main(int argc, char * argv[])
{
    Component component;
    // do not install ctrl-c handler
    bool installCtrlCHandler = false;
    orcaice::Application app( component, installCtrlCHandler );
    return app.main(argc, argv);
}
