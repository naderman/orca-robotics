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
#include <orcaice/subsystem.h>

#include <orcaice/orcaice.h>

using namespace std;

class TestSubsystem : public orcaice::Subsystem
{
public: 
    TestSubsystem( const orcaice::Context &context );

    virtual void doActivating();
    virtual void doActive();
    virtual void doReactivating() {};
    virtual void doDeactivating();
};

TestSubsystem::TestSubsystem( const orcaice::Context &context ) :
    Subsystem( context )
{
}

void 
TestSubsystem::doActivating() 
{
    cout<<"TestSubsystem::doActivating()"<<endl;
    gbxiceutilacfr::checkedSleep( this, IceUtil::Time::seconds(2), 500 );

    postEventWithType( orcaice::SubsystemActivated );
}

void 
TestSubsystem::doActive() 
{
    cout<<"TestSubsystem::doActive()"<<endl;
    gbxiceutilacfr::checkedSleep( this, IceUtil::Time::seconds(3), 500 );

    postEventWithType( orcaice::SubsystemDeactivate );
}

void 
TestSubsystem::doDeactivating() 
{
    cout<<"TestSubsystem::doDeactivating()"<<endl;
    gbxiceutilacfr::checkedSleep( this, IceUtil::Time::seconds(2), 500 );

    postEventWithType( orcaice::SubsystemDeactivated );
}

//////////////////////////////////////

class TestComponent : public orcaice::Component
{
public:
    TestComponent();

    // component interface
    virtual void start();
    virtual void stop();

private:
    gbxiceutilacfr::ThreadPtr mainSubsys_;
};

TestComponent::TestComponent() : 
    orcaice::Component( "Test", orcaice::NoStandardInterfaces )
{
}

void 
TestComponent::start()
{
    mainSubsys_ = new TestSubsystem( context() );
    mainSubsys_->start();
}

void 
TestComponent::stop()
{
    // nothing to do
}

//////////////////////////////////////

int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
