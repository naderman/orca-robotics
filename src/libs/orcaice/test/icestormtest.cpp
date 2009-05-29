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
// #include <orcaice/orcaice.h>
#include <orcaice/icestormutils.h>
#include <orca/odometry2d.h>

using namespace std;

class Odometry2dConsumerI : virtual public orca::Odometry2dConsumer
{
public:
    virtual void setData( const orca::Odometry2dData& data, const Ice::Current& )
    {
//         context_.tracer().debug( "Received data from provider", 8 );
    }
};

class TestComponent : public orcaice::Component
{
public:
    TestComponent() : orcaice::Component( "IceStormTest", orcaice::NoStandardInterfaces ) {};
    virtual void start();
};

void 
TestComponent::start()
{
    cout<<"testing createConsumerInterface() before activate ... ";
    try {
        Ice::ObjectPtr consumer = new Odometry2dConsumerI;
        orca::Odometry2dConsumerPrx consumerPrx =
            orcaice::createConsumerInterface<orca::Odometry2dConsumerPrx>( context(), consumer );
    }
    catch ( const std::exception& e ) {
        cout<<"failed, the adapter should be in Holding state: "<<endl<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;


// not sure how to test without relying on the IceStorm being there.
// should we define a orcaice::isIceStormReachable() ???

//     cout<<"testing connectToTopicWithString() ... ";
//     orca::HomePrx homePrx;
//     try {
//         orcaice::connectToTopicWithString<orca::HomePrx>( context(), homePrx, "homeless/*@local/connecttest" );
//     }
//     catch ( const orcaice::NetworkException & e ) {
//         cout<<"failed"<<endl<<e.what()<<endl;
//         exit(EXIT_FAILURE);
//     }
//     cout<<"ok"<<endl;

    context().shutdown();

    cout<<"testing createConsumerInterface() after shutdown ... ";
    try {
        Ice::ObjectPtr consumer = new Odometry2dConsumerI;
        orca::Odometry2dConsumerPrx consumerPrx =
            orcaice::createConsumerInterface<orca::Odometry2dConsumerPrx>( context(), consumer );
        cout<<"failed, the adapter should be in Shutdown state."<<endl;
        exit(EXIT_FAILURE);
    }
    catch ( const orcaice::ComponentDeactivatingException& e ) {
        // this is expected
    }
    catch ( const std::exception& e ) {
        cout<<"failed, unexpected exception type: "<<e.what()<<endl;
        exit(EXIT_FAILURE);
        
    }
    catch (...) {
        cout<<"failed, unexpected exception"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
}

int 
main(int argc, char * argv[])
{
    TestComponent component;
    orcaice::Application app( component );
    return app.orcaMain(argc, argv);
}
