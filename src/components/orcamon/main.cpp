/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <iostream>

// definition of Ice objects
#include <orca/gps.h>
#include <orca/localise2d.h>
#include <orca/ogmap.h>
#include <orca/position2d.h>
#include <orca/particle2d.h>
#include <orca/power.h>
#include <orca/rangescanner.h>
#include <orca/status.h>

#include <orcaice/orcaice.h>

#include "subscriber.h"
//specialisations of subscriber
#include "special.h"

using namespace std;
using namespace orca;



class OrcaMonComponent : public orcaice::Component
{
public:
    OrcaMonComponent() : orcaice::Component( "OrcaMon", orcaice::HomeInterface ) {};

    // component interface
    virtual void start();
    virtual void stop();
    
};


// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void OrcaMonComponent::start()
{
    //
    // REQUIRED INTERFACE: Generic
    //

    // This may throw ConfigFileException, but we don't catch it.
    // If this happens we may as well quit.
    std::string proxyString = 
                    orcaice::getRequiredInterfaceAsString( context(), "Generic" );
    Ice::ObjectPrx obj = context().communicator()->stringToProxy( proxyString );
    
    // connect to it and get its object ID, aka interface type.
    std::string objId;
    // TODO: this will not actually quit on ctrl-c
    while ( true ) // ( isActive() )
    {
        try
        {
            // this returns the most derived interface. see also ice_ids()
            objId = obj->ice_id();
            tracer()->debug( "interface type: "+objId,2 );
            break;
        }
        catch ( const Ice::CommunicatorDestroyedException & e )
        {
            // we must be shutting down
            return;
        }
        catch( const Ice::LocalException & e )
        {
            tracer()->error( "failed to lookup ID for "+proxyString+". Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
//     if ( !isActive() ) {
//         return;
//     }

    // now that we know the interface type, we can create an appropriate consumer and subscribe ourselves
    if ( objId=="::orca::Laser" || objId=="::orca::RangeScanner" )
    {
        attach<LaserPrx,RangeScannerConsumerPrx,RangeScannerConsumer,RangeScannerDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Localise2d" )
    {
        attach<Localise2dPrx,Localise2dConsumerPrx,Localise2dConsumer,Localise2dDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::OgMap" )
    {
        attach<OgMapPrx,OgMapConsumerPrx,OgMapConsumer,OgMapDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Platform2d" || objId=="::orca::Position2d" )
    {
        attach<Position2dPrx,Position2dConsumerPrx,Position2dConsumer,Position2dDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Power" )
    {
        attach<PowerPrx,PowerConsumerPrx,PowerConsumer,PowerDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Status" )
    {
        attach<StatusPrx,StatusConsumerPrx,StatusConsumer,StatusDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Particle2d" )
    {
        // Pretend it's just localise2d
        attach<Localise2dPrx,Localise2dConsumerPrx,Localise2dConsumer,Localise2dDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Gps" )
    {
        cout<<"****************** Gps interface **************"<<endl;
        //GPS has three object types and consumers
        attach<GpsPrx,GpsConsumerPrx,GpsConsumer,GpsDataPtr>
                ( context(), proxyString );
//         attach<GpsPrx,GpsMapGridConsumerPrx,GpsMapGridConsumer,GpsMapGridDataPtr>
        attachGpsMapGrid( context(), proxyString );
//         attach<GpsPrx,GpsTimeConsumerPrx,GpsTimeConsumer,GpsTimeDataPtr>
        attachGpsTime( context(), proxyString );
    }
    else
    {
        tracer()->error( "unsupported interface type: "+objId+". Quitting..." );
        context().communicator()->destroy();
    }

    
    // the rest is handled by the application/service
}

void OrcaMonComponent::stop()
{
    tracer()->debug( "stopping orcamon...", 5 );
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    OrcaMonComponent component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
