/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "component.h"
#include "mainloop.h"
#include "idriver.h"
#include "localnavmanager.h"
#include "vfhdriver/vfhdriver.h"
#include "pathmaintainer.h"
#include "testsim/testsim.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

namespace localnav {

Component::Component()
    : orcaice::Component( "LocalNav" ),
      platform2dPrx_(NULL),
      testSimulator_(NULL),
      driver_(NULL),
      localNavManager_(NULL),
      pathMaintainer_(NULL),
      mainLoop_(NULL),
      pathFollowerInterface_(NULL)
{
}

Component::~Component()
{
    if ( platform2dPrx_ != NULL )         delete platform2dPrx_;
    if ( driver_ != NULL )                delete driver_;
    if ( localNavManager_ != NULL )       delete localNavManager_;
    if ( pathMaintainer_ != NULL )        delete pathMaintainer_;
    if ( mainLoop_ != NULL )              delete mainLoop_;
    if ( testSimulator_ != NULL )         delete testSimulator_;
    if ( pathFollowerInterface_ != NULL ) delete pathFollowerInterface_;
}

void
Component::start()
{
    cout<<"TRACE(localnavcomponent.cpp): start()" << endl;
    //
    // INITIAL CONFIGURATION
    //

    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    bool testMode = false;

    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "vfhdriver" );
    if ( driverName == "vfhdriver" )
    {
        driver_  = new vfh::VfhDriver( context() );
    }
    else if ( driverName == "testvfhdriver" )
    {
        testMode = true;
        driver_  = new vfh::VfhDriver( context() );
    }
    else
    {
        std::string errString = "Unknown driver type: "+driverName;
        throw orcaice::Exception( ERROR_INFO, errString );
    }

    //
    // Create our provided interface
    //
    pathFollowerInterface_ = new PathFollower2dI( "PathFollower2d", context() );

    //
    // Connect to required interfaces
    //

    if ( !testMode )
    {
        // connect to the platform
        platform2dPrx_ = new orca::Platform2dPrx;
        orcaice::connectToInterfaceWithTag<Platform2dPrx>( context(), *platform2dPrx_, "Platform2d" );
        context().tracer()->debug("connected to a 'Platform2d' interface",5);

        // Instantiate everything
        obsConsumer_  = new orcaice::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr>;
        locConsumer_  = new orcaice::ProxiedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>;
        odomConsumer_ = new orcaice::ProxiedConsumerI<orca::Position2dConsumer,orca::Position2dData>;

        // subscribe for information from platform
        RangeScanner2dPrx obsPrx;
        Localise2dPrx   locPrx;
        Position2dPrx   odomPrx;
        orcaice::connectToInterfaceWithTag<orca::RangeScanner2dPrx>( context(), obsPrx, "Observations" );
        orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context(), locPrx, "Localisation" );
        orcaice::connectToInterfaceWithTag<orca::Position2dPrx>( context(), odomPrx, "Platform2d" );

        Ice::ObjectPtr obsConsumerPtr = obsConsumer_;
        orca::RangeScanner2dConsumerPrx obsConsumerPrx =
            orcaice::createConsumerInterface<RangeScanner2dConsumerPrx>( context(), obsConsumerPtr );
        Ice::ObjectPtr locConsumerPtr = locConsumer_;
        orca::Localise2dConsumerPrx locConsumerPrx =
            orcaice::createConsumerInterface<Localise2dConsumerPrx>( context(), locConsumerPtr );
        Ice::ObjectPtr odomConsumerPtr = odomConsumer_;
        orca::Position2dConsumerPrx odomConsumerPrx =
            orcaice::createConsumerInterface<Position2dConsumerPrx>( context(), odomConsumerPtr );

        obsPrx->subscribe(  obsConsumerPrx );
        locPrx->subscribe(  locConsumerPrx );
        odomPrx->subscribe( odomConsumerPrx );
    }
    else
    {
        orca::PathFollower2dData testPath;
        getTestPath( testPath );
        pathFollowerInterface_->setData( testPath, true );
        testSimulator_ = new Simulator( context(), testPath );
    }

    ////////////////////////////////////////////////////////////////////////////////

    //
    // Instantiate the guys who do the work
    //

    pathMaintainer_ = new PathMaintainer(*pathFollowerInterface_,
                                         context());

    localNavManager_ = new LocalNavManager( *driver_, *pathMaintainer_, context() );

    if ( !testMode )
    {
        mainLoop_ = new MainLoop( *localNavManager_,
                                  obsConsumer_->proxy_,
                                  locConsumer_->proxy_,
                                  odomConsumer_->proxy_,
                                  *pathFollowerInterface_,
                                  *platform2dPrx_,
                                  *pathMaintainer_,
                                  pathPublisher_,
                                  context() );
    }
    else
    {
        mainLoop_ = new MainLoop( *localNavManager_,
                                  testSimulator_->obsProxy_,
                                  testSimulator_->locProxy_,
                                  testSimulator_->odomProxy_,
                                  *pathFollowerInterface_,
                                  *testSimulator_,
                                  *pathMaintainer_,
                                  pathPublisher_,
                                  context() );
    }

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    cout<<"TRACE(localnavcomponent.cpp): entering main loop..." << endl;

    //
    // MAIN DRIVER LOOP
    //

    mainLoop_->start();    
}

void Component::stop()
{
    orcaice::Thread::stopAndJoin( mainLoop_ );
}

}
