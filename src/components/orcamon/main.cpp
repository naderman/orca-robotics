/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <orcaice/application.h>
#include <orcaice/component.h>
#include <iostream>

// definition of Ice objects
#include <orca/gps.h>
#include <orca/localise2d.h>
#include <orca/ogmap.h>
#include <orca/position2d.h>
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
    std::string proxyString = orcaice::toString(
                    orcaice::getRequiredInterface( context(), "Generic" ) );
    Ice::ObjectPrx obj = communicator()->stringToProxy( proxyString );
    
    // connect to it and get its object ID, aka interface type.
    std::string objId;
    while( isActive() )
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
            stop();
        }
        catch( const Ice::LocalException & e )
        {
            tracer()->error( "failed to lookup ID for "+proxyString+". Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    if ( !isActive() ) {
        return;
    }

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
    else if ( objId=="::orca::Gps" )
    {
        //GPS has three object types and consumers
        attach<GpsPrx,GpsConsumerPrx,GpsConsumer,GpsDataPtr>
                ( context(), proxyString );
        attach<GpsPrx,GpsMapGridConsumerPrx,GpsMapGridConsumer,GpsMapGridDataPtr>
                ( context(), proxyString );
        attach<GpsPrx,GpsTimeConsumerPrx,GpsTimeConsumer,GpsTimeDataPtr>
                ( context(), proxyString );
    }
    else
    {
        tracer()->error( "unsupported interface type: "+objId+". Quitting..." );
        communicator()->destroy();
    }

    
    // the rest is handled by the application/service
}

void OrcaMonComponent::stop()
{
    tracer()->debug( "stopping orcamon...", 5 );
    Component::stop();
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int main(int argc, char * argv[])
{
    OrcaMonComponent component;
    orcaice::Application app( argc, argv, component );
    return app.main(argc, argv);
}
