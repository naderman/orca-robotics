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

#include <orcaice/orcaice.h>
#include <iostream>

// implementations of Ice objects
#include "rangescannerconsumerI.h"


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
    cout<<"LaserMonComponent::start"<<endl;
    //
    // REQUIRED INTERFACE: Laser
    //

    // Connect directly to the interface
    orca::LaserPrx laserPrx;
    // TODO: this will not actually quit on ctrl-c
    while ( true ) // ( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::LaserPrx>( context(), laserPrx, "Laser" );
            break;
        }
        catch ( const orcaice::NetworkException & )
        {
            tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }
    
    // Get the geometry
    cout << "Laser Geometry: " << orcaice::toString( laserPrx->getGeometry() ) << endl;

    // Get the configuration
    cout << "Laser Config:   " << orcaice::toString( laserPrx->getConfig() ) << endl;

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
        catch ( const orca::DataNotExistException & )
        {
            tracer()->warning( "hardware failure reported when getting a scan." );
        }
        catch ( const orca::HardwareFailedException & )
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
    // TODO: this will not actually quit on ctrl-c
    while ( true ) // ( isActive() )
    {
        try
        {
            laserPrx->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & )
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
    orcaice::Application app( component );
    return app.main(argc, argv);
}
