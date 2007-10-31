/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <iostream>
#include "tracerconsumerI.h"
#include "component.h"


using namespace std;

namespace sysmon {

Component::Component()
    : orcaice::Component( "SystemTracerMon" )
{
}

void
Component::connectForTracingInfo( orca::TracerPrx tracerPrx )
{
    //
    // Subscribe for data
    //
    // will try forever until the user quits with ctrl-c
    while ( true )
    {
        try
        {
            tracerPrx->subscribeForPlatformInfoMessages(    callbackPrx_ );
            tracerPrx->subscribeForPlatformWarningMessages( callbackPrx_ );
            tracerPrx->subscribeForPlatformErrorMessages(   callbackPrx_ );
            break;
        }
        catch ( const orca::SubscriptionFailedException &e )
        {
            stringstream ss;
            ss << "Failed to subscribe for data updates: "<<e.what<<endl<<"Will try again after 2 seconds.";
            tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }
    
}

// NOTE: this function returns after it's done, all variable that need to be permanent must
//       be declared as member variables.
void
Component::start()
{
    cout<<"Component::start"<<endl;

    //
    // ENABLE NETWORK
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();

    // create a callback object to recieve data
    tracerConsumer_ = new TracerConsumerI;
    Ice::ObjectPtr obj = tracerConsumer_;
    callbackPrx_ = orcaice::createConsumerInterface<orca::TracerConsumerPrx>( context(), obj );

    // Connect to required interfaces
    int i=-1;
    // loop over interfaces
    bool noMoreInterfaces=false;
    while ( !noMoreInterfaces )
    {
        i++;
        // Try to connect in a loop
        while ( true )
        {
            try
            {
                orca::TracerPrx tracerPrx;
                stringstream tagStream;
                tagStream << "Tracer" << i;
                
                Ice::PropertiesPtr prop = properties();
                std::string key = tag()+".Requires."+tagStream.str()+".Proxy";
                std::string endpoint;
                // cout<<"TRACE(component.cpp): Looking for: " << key << endl;

                int ret = orcaice::getProperty( prop, key, endpoint );
                if ( ret != 0 ) 
                {
                    noMoreInterfaces = true;
                    break;
                }

                cout<<"TRACE(component.cpp): Connecting to endpoint: " << endpoint << endl;
                orcaice::connectToInterfaceWithTag<orca::TracerPrx>( context(), tracerPrx, tagStream.str() );
                connectForTracingInfo( tracerPrx );
                cout<<"                        -> Connected." << endl;
                break;
            }
            catch ( const orcaice::NetworkException &e )
            {
                std::stringstream ss;
                ss << "Failed to connect to remote object: "<<e.what()<<endl<<"Will try again after 2 seconds.";
                tracer()->error( ss.str() );
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
            }
        }
    }

    // the rest is handled by the application/service
}

void
Component::stop()
{
    // nothing to do
}

}
