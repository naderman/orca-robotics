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

#include "handler.h"

namespace registrylist
{

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();
private:
    Handler* handler_;
};

// don't need any standard interfaces because this component quits righ after it finished
// with its job.
Component::Component()
    : orcaice::Component( "RegistryList", orcaice::NoStandardInterfaces )
{
}

Component::~Component()
{
}

void Component::start()
{
    handler_ = new Handler( context() );
    handler_->start();
}

void Component::stop()
{
    // nothing to do
}

} // namespace

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    registrylist::Component component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
