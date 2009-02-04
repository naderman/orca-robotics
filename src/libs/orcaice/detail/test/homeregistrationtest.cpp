/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/application.h>
#include <orcaice/component.h>
#include "../privateutils.h"

#include <orcaice/orcaice.h>

using namespace std;

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "Test", orcaice::NoStandardInterfaces ) {};
    virtual void start();
};

void 
TestComponent::start()
{
    cout<<"testing registerHomeInterface() ... ";
    try {
        orcaice::detail::registerHomeInterface( context() );
    }
    catch ( const Ice::Exception& e ) {
        cout<<"failed: "<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const std::exception& e ) {
        cout<<"failed: "<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    context().shutdown();
}

int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
