/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
#include <orca/imu.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/ogmap.h>
#include <orca/featuremap2d.h>
#include <orca/odometry2d.h>
#include <orca/particle2d.h>
#include <orca/power.h>
#include <orca/rangescanner2d.h>
#include <orca/status.h>

#include <orcaice/orcaice.h>

#include "subscriber.h"
//specialisations of subscriber

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

    cout<<"TRACE(main.cpp): Connecting to " << proxyString << endl;

    // now that we know the interface type, we can create an appropriate consumer and subscribe ourselves
    if ( objId=="::orca::Laser" || objId=="::orca::RangeScanner" )
    {
        attach<LaserScanner2dPrx,RangeScanner2dConsumerPrx,RangeScanner2dConsumer,RangeScanner2dDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Imu" )
    {
        attach<ImuPrx,ImuConsumerPrx,ImuConsumer,ImuData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Localise2d" )
    {
        attach<Localise2dPrx,Localise2dConsumerPrx,Localise2dConsumer,Localise2dData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Localise3d" )
    {
        attach<Localise3dPrx,Localise3dConsumerPrx,Localise3dConsumer,Localise3dData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::OgMap" )
    {
        attach<OgMapPrx,OgMapConsumerPrx,OgMapConsumer,OgMapData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::FeatureMap2d" )
    {
        attach<FeatureMap2dPrx,FeatureMap2dConsumerPrx,FeatureMap2dConsumer,FeatureMap2dDataPtr>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Odometry2d" )
    {
        attach<Odometry2dPrx,Odometry2dConsumerPrx,Odometry2dConsumer,Odometry2dData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::Odometry3d" )
    {
        attach<Odometry3dPrx,Odometry3dConsumerPrx,Odometry3dConsumer,Odometry3dData>
                ( context(), proxyString );
    }
//     else if ( objId=="::orca::Power" )
//     {
//         attach<PowerPrx,PowerConsumerPrx,PowerConsumer,PowerDataPtr>
//                 ( context(), proxyString );
//     }
    else if ( objId=="::orca::Status" )
    {
        attach<StatusPrx,StatusConsumerPrx,StatusConsumer,StatusData>
                ( context(), proxyString );
    }
//     else if ( objId=="::orca::Particle2d" )
//     {
//         attach<Particle2dPrx,Particle2dConsumerPrx,Particle2dConsumer,Particle2dData>
//                 ( context(), proxyString );
//     }
    else if ( objId=="::orca::Gps" )
    {
        attach<GpsPrx,GpsConsumerPrx,GpsConsumer,GpsData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::GpsMapGrid" )
    {
        attach<GpsMapGridPrx,GpsMapGridConsumerPrx,GpsMapGridConsumer,GpsMapGridData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::GpsTime" )
    {
        attach<GpsTimePrx,GpsTimeConsumerPrx,GpsTimeConsumer,GpsTimeData>
                ( context(), proxyString );
    }
    else if ( objId=="::orca::GpsTime" )
    {
        attach<GpsPrx,GpsConsumerPrx,GpsConsumer,GpsData>
                ( context(), proxyString );
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
