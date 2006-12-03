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

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <orcaice/thread.h>

#include <orcaice/orcaice.h>

using namespace std;

class Handler: public orcaice::Thread
{    	
public:
    Handler( const orcaice::Context & context )
    : context_(context) {}
    ~Handler() {};
    virtual void run();
private:
    orcaice::Context context_;
};

void
Handler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
        std::string prefix = context_.tag() + ".Config.";
        // if this negative, play indefinitely
        int replays = orcaice::getPropertyAsIntWithDefault( context_.properties(),
                prefix+"ReplayTimes", 1 );
        int count = 0;

        bool isNetworkUp, isRegistryUp;

        //
        // Main loop
        //   
        while( isActive() )
        {
            if ( count==replays && replays>0 ) {
                throw orcaice::Exception( ERROR_INFO, "we just want to quit" );
            }

            orcaice::getNetworkState( context_, isNetworkUp, isRegistryUp );
            cout<<"net: "<<(int)isNetworkUp<<" reg: "<<(int)isRegistryUp<<endl;

            ++count;
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        }

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "handler stopped", 5 );
}

class TestComponent : public orcaice::Component
{
public:
    TestComponent();
    virtual ~TestComponent();

    // component interface
    virtual void start();
    virtual void stop();
private:
    Handler* handler_;
};


TestComponent::TestComponent()
    : orcaice::Component( "NetTest" )
{
}

TestComponent::~TestComponent()
{
}

void TestComponent::start()
{
    handler_ = new Handler( context() );
    handler_->start();
}

void TestComponent::stop()
{
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
