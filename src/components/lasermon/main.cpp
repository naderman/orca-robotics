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

#include <orcaice/orcaice.h>
#include <iostream>

// implementations of Ice objects
#include "rangescannerconsumer_i.h"


using namespace std;

class LaserMonComponent : public orcaice::Component
{
public:
    LaserMonComponent();
    virtual ~LaserMonComponent();

    // component interface
    virtual void start();
    virtual void stop();
};


LaserMonComponent::LaserMonComponent()
    : orcaice::Component( "LaserMon", orcaice::HomeInterface  )
{
}

LaserMonComponent::~LaserMonComponent()
{
    // do not delete inputLoop_!!! It derives from Ice::Thread and deletes itself.
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void
LaserMonComponent::start()
{
    //
    // REQUIRED INTERFACE: Laser
    //

    // Connect directly to the interface
    orca::LaserPrx laserPrx;
    while( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::LaserPrx>( context(), laserPrx, "Laser" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }
    
    // Get the geometry
    cout << "Laser Geometry: " << laserPrx->getGeometry() << endl;

    // Get the configuration
    cout << "Laser Config:   " << laserPrx->getConfig() << endl;

    // Get laser data once
    int count = 5;
    tracer()->info( "Trying to get one scan as a test" );
    while ( count-- )
    {
        try
        {
            tracer()->print( orcaice::toString( laserPrx->getData() ) );
            break;
        }
        catch ( const orca::DataNotExistException & e )
        {
            tracer()->warning( "hardware failure reported when getting a scan." );
        }
        catch ( const orca::HardwareFailedException & e )
        {
            tracer()->warning( "hardware failure reported when getting a scan." );
        }
    }
    
    // Can also set the configuration like so:
//     orca::RangeScannerConfigPtr cfg = new orca::RangeScannerConfig;
//     cfg->rangeResolution = 9999;
//     cfg->isEnabled = true;
//     try
//     {
//         laserPrx->setConfig( cfg );
//     }
//     catch ( orca::ConfigurationNotExistException & e )
//     {
//         tracer()->warning( "failed to set configuration. Will subscribe anyway." );
//     }

    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new RangeScannerConsumerI;
    orca::RangeScannerConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::RangeScannerConsumerPrx>( context(), consumer );
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();
    
    //
    // Subscribe for data
    //
    // will try forever until the user quits with ctrl-c
    while ( isActive() )
    {
        try
        {
            laserPrx->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // the rest is handled by the application/service
}

void
LaserMonComponent::stop()
{
    // nothing to do
}

//
// Build the component into a stand-alone application
//
int
main(int argc, char * argv[])
{
    LaserMonComponent component;
    orcaice::Application app( argc, argv, component );
    return app.main(argc, argv);
}
