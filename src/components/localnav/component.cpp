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
#include <orcalocalnav/localnavmanager.h>
#include <orcalocalnav/pathmaintainer.h>
#include "testsim/testsim.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

namespace localnav {

Component::Component()
    : orcaice::Component( "LocalNav" ),
      testSimulator_(NULL),
      mainLoop_(NULL),
      driverLib_(NULL),
      clock_(NULL),
      pathFollowerInterface_(NULL)
{
    orca::Time t; t.seconds=0; t.useconds=0;
    clock_ = new orcalocalnav::Clock( t );
}

Component::~Component()
{
    if ( mainLoop_  )              delete mainLoop_;
    if ( testSimulator_  )         delete testSimulator_;
    if ( driverLib_  )             delete driverLib_;
    if ( pathFollowerInterface_  ) delete pathFollowerInterface_;
    if ( clock_ )                  delete clock_;
    if ( driverFactory_ )          delete driverFactory_;
}

void
Component::start()
{
    context().tracer()->info( "start()" );

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
    pathFollowerInterface_ = new orcalocalnav::PathFollower2dI( "PathFollower2d", *clock_, context() );

    //
    // Instantiate bogus info sources in test mode
    //
    if ( testMode )
    {
        int numWaypoints = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Test.NumWaypoints", 10 );
        cout<<"TRACE(component.cpp): Using " << numWaypoints << " waypoints" << endl;
        orca::PathFollower2dData testPath;
        getTestPath( testPath, numWaypoints );
        pathFollowerInterface_->setData( testPath, true );
        testSimulator_ = new Simulator( context(), testPath );
    }

    ////////////////////////////////////////////////////////////////////////////////

    //
    // Instantiate the guys who do the work
    //

    context().tracer()->debug( "Component: Loading driver library.", 4 );
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libOrcaLocalNavVfh.so" );
    try {
        // Dynamically load the driver from its library
        driverLib_ = new orcadynamicload::DynamicallyLoadedLibrary(driverLibName);
        driverFactory_ = 
            orcadynamicload::dynamicallyLoadClass<DriverFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" );
    }
    catch (orcadynamicload::DynamicLoadException &e)
    {
        context().tracer()->error( e.what() );
        throw;
    }

    context().tracer()->debug( "Component: Instantiating main loop.", 3 );
    if ( !testMode )
    {
        mainLoop_ = new MainLoop( *driverFactory_,
                                  *clock_,
                                  *pathFollowerInterface_,
                                  context() );
    }
    else
    {
        mainLoop_ = new MainLoop( *driverFactory_,
                                  *clock_,
                                  *pathFollowerInterface_,
                                  *testSimulator_,
                                  context() );
    }

    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    context().tracer()->debug( "Component: Entering main loop.", 3 );

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
