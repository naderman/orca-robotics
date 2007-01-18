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
#include <localnavutil/idriver.h>
#include "localnavmanager.h"
#include "pathmaintainer.h"
#include "testsim/testsim.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

namespace localnav {

Component::Component()
    : orcaice::Component( "LocalNav" ),
      testSimulator_(NULL),
      localNavManager_(NULL),
      pathMaintainer_(NULL),
      mainLoop_(NULL),
      driver_(NULL),
      driverLib_(NULL),
      pathFollowerInterface_(NULL)
{
}

Component::~Component()
{
    if ( localNavManager_ != NULL )       delete localNavManager_;
    if ( pathMaintainer_ != NULL )        delete pathMaintainer_;
    if ( mainLoop_ != NULL )              delete mainLoop_;
    if ( testSimulator_ != NULL )         delete testSimulator_;
    if ( driver_ != NULL )                delete driver_;
    if ( driverLib_ != NULL )             delete driverLib_;
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

    bool testMode = orcaice::getPropertyAsIntWithDefault( prop, prefix+"TestMode", 0 );

    //
    // Create our provided interface
    //
    pathFollowerInterface_ = new PathFollower2dI( "PathFollower2d", context() );

    //
    // Connect to required interfaces
    //
    orca::VehicleDescription descr;
    if ( !testMode )
    {
        // connect to the controller
        orcaice::connectToInterfaceWithTag<VelocityControl2dPrx>( context(), velocityControl2dPrx_, "VelocityControl2d" );
        context().tracer()->debug("connected to a 'VelocityControl2d' interface",5);

        // Get the vehicle description
        descr = velocityControl2dPrx_->getDescription();

        // Instantiate everything
        obsConsumer_  = new orcaifaceimpl::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr>;
        locConsumer_  = new orcaifaceimpl::ProxiedConsumerI<orca::Localise2dConsumer,orca::Localise2dData>;
        odomConsumer_ = new orcaifaceimpl::ProxiedConsumerI<orca::Odometry2dConsumer,orca::Odometry2dData>;

        // subscribe for information from platform
        RangeScanner2dPrx obsPrx;
        Localise2dPrx   locPrx;
        Odometry2dPrx   odomPrx;
        orcaice::connectToInterfaceWithTag<orca::RangeScanner2dPrx>( context(), obsPrx, "Observations" );
        orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context(), locPrx, "Localisation" );
        orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context(), odomPrx, "Odometry2d" );

        Ice::ObjectPtr obsConsumerPtr = obsConsumer_;
        orca::RangeScanner2dConsumerPrx obsConsumerPrx =
            orcaice::createConsumerInterface<RangeScanner2dConsumerPrx>( context(), obsConsumerPtr );
        Ice::ObjectPtr locConsumerPtr = locConsumer_;
        orca::Localise2dConsumerPrx locConsumerPrx =
            orcaice::createConsumerInterface<Localise2dConsumerPrx>( context(), locConsumerPtr );
        Ice::ObjectPtr odomConsumerPtr = odomConsumer_;
        orca::Odometry2dConsumerPrx odomConsumerPrx =
            orcaice::createConsumerInterface<Odometry2dConsumerPrx>( context(), odomConsumerPtr );

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
        descr = testSimulator_->getVehicleDescription();
    }

    stringstream descrStream;
    descrStream << "Working with the following vehicle: " << orcaice::toString(descr);
    context().tracer()->info( descrStream.str() );

    ////////////////////////////////////////////////////////////////////////////////

    //
    // Instantiate the guys who do the work
    //

    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libOrcaLocalNavVfh.so" );
    try {
        // Dynamically load the driver from its library
        driverLib_ = new orcadynamicload::DynamicallyLoadedLibrary(driverLibName);
        DriverFactory *driverFactory = 
            orcadynamicload::dynamicallyLoadClass<localnav::DriverFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" );

        driver_ = driverFactory->createDriver( context(), descr );
        delete driverFactory;
    }
    catch (orcadynamicload::DynamicLoadException &e)
    {
        context().tracer()->error( e.what() );
        throw;
    }
    
//     if ( driverName == "vfhdriver" )
//     {
//         driver_  = new vfh::VfhDriver( context() );
//     }
//     else if ( driverName == "testvfhdriver" )
//     {
//         testMode = true;
//         driver_  = new vfh::VfhDriver( context() );
//     }
//     else
//     {
//         std::string errString = "Unknown driver type: "+driverName;
//         throw orcaice::Exception( ERROR_INFO, errString );
//     }

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
                                  velocityControl2dPrx_,
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
