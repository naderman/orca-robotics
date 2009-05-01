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
#include <orcaice/catchutils.h>
#include <orcaice/context.h>

#include <gbxutilacfr/gbxutilacfr.h>

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
}

void 
TestThread::initialise() 
{
    context_.tracer().info( "TestThread::initialise()" );
    try
    {
        throw string("Error in initialise()");
    }
    catch (...)
    {
        orcaice::catchExceptions( context_.tracer(), "initialising" );
    }
}

void 
TestThread::work() 
{
    context_.tracer().info( "TestThread::work()" );
    try
    {
        try
        {
            throw string("Error in work()");
        }
        catch ( const string& e )
        {
            context_.tracer().info( "caught string the first time. Rethrowing ..." );
            throw;
        }
    }
    catch (...)
    {
        orcaice::catchExceptions( context_.tracer(), "working" );
//         orcaice::catchExceptions( context_.tracer(), string(__func__) );
    }
}

void 
TestThread::finalise() 
{
    context_.tracer().info( "TestThread::finalise()" );

    try {
        throw gbxutilacfr::Exception( ERROR_INFO, "Error in finalise()" );
    }
    catch ( ... ) {
//         orcaice::catchExceptions( context_.tracer(), "finalising" );
//         orcaice::catchExceptionsWithStatus( "finalising", health() );
        orcaice::catchExceptionsWithStatus( "finalising", health(), gbxutilacfr::SubsystemWarning );
//         orcaice::catchExceptionsWithStatusAndSleep( "finalising", health(), gbxutilacfr::SubsystemWarning );
    }

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
//     if ( config_ & ThrowCompConstructor )
//         throw "throwing from component constructor";
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
