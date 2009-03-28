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
#include <assert.h>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <hydroutil/hydroutil.h>
#include "mainthread.h"
#include "pathplanner2dI.h"
#include <orcaogmap/orcaogmap.h>
#if QT4_FOUND
#include "qgraphicspublisher.h"
#endif

using namespace std;

namespace pathplanner {

MainThread::MainThread( const orcaice::Context & context )
    : orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
      pathPlannerTaskBuffer_( 100, gbxiceutilacfr::BufferTypeQueue ),
      context_(context)
{
}

void 
MainThread::initialise()
{
    subStatus().setMaxHeartbeatInterval( 30.0 );
    
    activate( context_, this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

    initNetwork();
    if ( isStopping() )
        return;

    // Initialising the driver may require a long time to process the map.
    // (this may still lead to a timeout if somebody is waiting for a status update.
    // ideally, map processing should be done in a separate thread.)
    subStatus().setMaxHeartbeatInterval( -1 );

    initDriver();
    assert( driver_.get() );
}

void 
MainThread::work()
{
    subStatus().setMaxHeartbeatInterval( 5 );

    orca::PathPlanner2dTask task; 
    orca::PathPlanner2dData pathData;   

    while ( !isStopping() )
    {
        try
        {
            
            //
            //  ======== waiting for a task (blocking) =======
            //
            context_.tracer().info("waiting for a new task");
            bool haveTask=false;
            
            while ( !isStopping() )
            {
                int timeoutMs = 1000;
                int ret = pathPlannerTaskBuffer_.getAndPopWithTimeout( task, timeoutMs );
                if ( ret==0 ) {
                    haveTask = true;
                    context_.tracer().info("task arrived");  
                    
                    std::string sketchReason;
                    if ( orcaobj::isPathSketchy( task.coarsePath, sketchReason ) )
                    {
                        stringstream ss;
                        ss << "Incoming task is sketchy: " << orcaobj::toVerboseString(task)
                           << endl << sketchReason;
                        subStatus().warning( ss.str() );
                    }

                    break;
                }
                subStatus().ok();
            }
            
            // the only way of getting out of the above loop without a task
            // is if the user pressed Ctrl+C, ie we have to quit
            if (!haveTask) break;
            
            //
            // ===== tell driver to compute the path ========
            //
            
            try 
            {
                context_.tracer().info("telling driver to compute the path now");
                pathData.timeStamp = task.timeStamp;
                driver_->computePath( task, pathData.path );
                pathData.result = orca::PathOk;
                pathData.resultDescription = "All good";
            }
            catch ( hydrointerfaces::PathPlanner2d::PathStartNotValidException &e )
            {
                std::stringstream ss;
                ss << "Couldn't compute path: " << orcaobj::toVerboseString(task) << endl << "Problem was: " << e.what();
                context_.tracer().error( ss.str() );
                pathData.resultDescription = ss.str();
                pathData.result = orca::PathStartNotValid;
            }
            catch ( hydrointerfaces::PathPlanner2d::PathDestinationNotValidException &e )
            {
                std::stringstream ss;
                ss << "Couldn't compute path: " << orcaobj::toVerboseString(task) << endl << "Problem was: " << e.what();
                context_.tracer().error( ss.str() );
                pathData.resultDescription = ss.str();
                pathData.result = orca::PathDestinationNotValid;
            }
            catch ( hydrointerfaces::PathPlanner2d::PathDestinationUnreachableException &e )
            {
                std::stringstream ss;
                ss << "Couldn't compute path: " << orcaobj::toVerboseString(task) << endl << "Problem was: " << e.what();
                context_.tracer().error( ss.str() );
                pathData.resultDescription = ss.str();
                pathData.result = orca::PathDestinationUnreachable;
            }
            catch ( hydrointerfaces::PathPlanner2d::Exception &e )
            {
                std::stringstream ss;
                ss << "Couldn't compute path: " << orcaobj::toVerboseString(task) << endl << "Problem was: " << e.what();
                context_.tracer().error( ss.str() );
                pathData.resultDescription = ss.str();
                pathData.result = orca::PathOtherError;
            }

            //
            // ======= send result (including error code) ===============
            //
            context_.tracer().info("MainThread: sending off the resulting path");
            context_.tracer().debug(orcaobj::toVerboseString(pathData));
    
            // There are three methods to let other components know about the computed path:
            // 1. using the proxy
            if (task.prx!=0)
            {
                task.prx->setData( pathData );
            }
            else
            {
                context_.tracer().warning( "MainThread: task.prx was zero!" );
            }
            // 2. and 3.: use getData or icestorm
            pathPlannerI_->localSetData( pathData );
    
            // resize the pathData: future tasks might not compute a path successfully and we would resend the old path
            pathData.path.resize( 0 );
    
            int numTasksWaiting = pathPlannerTaskBuffer_.size();
            if ( numTasksWaiting > 1 )
            {
                stringstream ss;
                ss << "MainThread: Tasks are piling up: there are " << numTasksWaiting << " in the queue.";
                subStatus().warning( ss.str() );
            }
            else
            {
                subStatus().ok();
            }
        } // try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );
        }
    } // end of while
}

/////////////

void
MainThread::initNetwork()
{
    //
    // REQUIRED INTERFACE: OgMap
    //
    subStatus().ok("Connecting to OgMap" );
    orca::OgMapPrx ogMapPrx;    
    orcaice::connectToInterfaceWithTag<orca::OgMapPrx>( context_, ogMapPrx, "OgMap", this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

    // get the og map once
    subStatus().ok("Getting Og Map" );
    orca::OgMapData ogMapSlice;
    try
    {
        ogMapSlice = ogMapPrx->getData();
        stringstream ss;
        ss << "MainThread::"<<__func__<<"(): got ogMap: " << orcaobj::toString( ogMapSlice );
        context_.tracer().info( ss.str() );
    }
    catch ( const orca::DataNotExistException & e )
    {
        std::stringstream ss;
        ss << "MainThread::"<<__func__<<": DataNotExistException: "<<e.what;
        context_.tracer().warning( ss.str() );
    }
    // convert into internal representation
    orcaogmap::convert(ogMapSlice,ogMap_);

    // Store in the conifg structure
    hydroDriverConfig_.reset( new hydrointerfaces::PathPlanner2d::Config(ogMap_) );
    
    //
    // PROVIDED INTERFACES
    //

    // PathPlanner2d
    subStatus().ok("Creating PathPlanner2d Interface" );
    pathPlannerI_ = new PathPlanner2dI( pathPlannerTaskBuffer_, context_ );
    Ice::ObjectPtr pathPlannerObj = pathPlannerI_;
    
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, pathPlannerObj, "PathPlanner2d" );
}

void
MainThread::initDriver()
{
    subStatus().ok("Initialising Driver" );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroPathPlanner2dAcfr.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::PathPlanner2dFactory> driverFactory;

    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::PathPlanner2dFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" ) );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        // unrecoverable error
        context_.shutdown(); 
        throw;
    }

#if QT4_FOUND
    //
    // Create the graphics publisher
    //
    graphicsPublisher_.reset( new QGraphicsPublisher( context_, "graphics" ) );
    hydroDriverConfig_->graphicsPublisher = graphicsPublisher_.get();
#endif

    //
    // create the hydro driver
    //
    try {
        context_.tracer().info( "Creating hydro driver..." );
        assert( hydroDriverConfig_.get() );
        hydroDriver_.reset(0);
        hydroDriver_.reset( driverFactory->createDriver( *hydroDriverConfig_, context_.toHydroContext() ) );
    }
    catch ( ... ) {
        orcaice::catchExceptionsWithStatusAndSleep( "initialising algorithm driver", subStatus() );

        // this is a fatal error!
        context_.shutdown();
    }    

    //
    // Create our local driver: a wrapper for the hydro driver
    //
    double intermediateWaypointDistTolerance = 
            orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"IntermediateWaypointDistanceTolerance", 1.5 );
    driver_.reset( new Driver( *hydroDriver_,
                               intermediateWaypointDistTolerance,
                               context_ ) );

//     //
//     // Initialise the path-planning algorithm
//     //
//     try {
//         pathPlanner_.reset( pathPlannerFactory.getPathPlanner( ogMap_,
//                                                                traversabilityThreshhold,
//                                                                robotDiameterMetres,
//                                                                context_.toHydroContext( context_.tag()+".Config." ) ) );
//     }
//     catch ( ... ) {
//         orcaice::catchExceptionsWithStatusAndSleep( "initialising algorithm driver", subStatus() );

//         // this is a fatal error!
//         context_.shutdown();
//     }

//     //
//     // And the Driver to manage it
//     //
//     double intermediateDistTolerance = 
//             orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"IntermediateWaypointDistanceTolerance", 1.5 );
//     driver_.reset( new Driver( *pathPlanner_,
//                                ogMap_,
//                                traversabilityThreshhold,
//                                robotDiameterMetres,
//                                intermediateDistTolerance,
//                                context_ ) );

    context_.tracer().debug("driver instantiated",5);
}

}
