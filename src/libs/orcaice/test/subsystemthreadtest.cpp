/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>

using namespace std;

enum Config 
{
    ThrowCompConstructor    = 0x00001,
    ThrowSubsysConstructor  = 0x00010,
    ExitFromInitialise      = 0x00100,
    ExitFromWork            = 0x01000,
    ExitFromFinalise        = 0x10000
};

class TestThread : public orcaice::SubsystemThread
{
public: 
    TestThread( Config config, const orcaice::Context &context );

private:
    virtual void initialise();
    virtual void work();
    virtual void finalise();
    Config config_;
    orcaice::Context context_;
};

TestThread::TestThread( Config config, const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status() ),
    config_( config ),
    context_(context)
{
    if ( config_ & ThrowSubsysConstructor )
        throw "throwing from subsystem constructor";
}

void 
TestThread::initialise() 
{
    context_.tracer().info( "TestThread::initialise()" );
    if ( config_ & ExitFromInitialise )
        context_.shutdown();
}

void 
TestThread::work() 
{
    context_.tracer().info( "TestThread::work()" );
    if ( config_ & ExitFromWork )
        context_.shutdown();
}

void 
TestThread::finalise() 
{
    context_.tracer().info( "TestThread::finalise()" );
    if ( config_ & ExitFromFinalise )
        context_.shutdown();
}

//////////////////////////////////////

class TestComponent : public orcaice::Component
{
public:
    TestComponent( Config config );
    virtual void start();
    virtual void stop();

private:
    gbxiceutilacfr::ThreadPtr mainSubsys_;
    Config config_;
};

TestComponent::TestComponent( Config config ) : 
    orcaice::Component( "Test", orcaice::NoStandardInterfaces ),
    config_(config)
{
    if ( config_ & ThrowCompConstructor )
        throw "throwing from component constructor";
}

void 
TestComponent::start()
{
    mainSubsys_ = new TestThread( config_, context() );
    mainSubsys_->start();
}

void 
TestComponent::stop()
{
    gbxiceutilacfr::stopAndJoin( mainSubsys_ );

    // see comments in autogen component.cpp
    mainSubsys_ = 0;
}

//////////////////////////////////////

int main(int argc, char * argv[])
{
//     Config config = ThrowCompConstructor;  // do not use, this one is not caught in main()
//     Config config = ThrowSubsysConstructor;
//     Config config = ExitFromInitialise;
    Config config = ExitFromWork;

    TestComponent component( config );
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
