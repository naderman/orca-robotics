/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include <orcamisc/orcamisc.h>
#include "algohandler.h"
#include "pathplanner2dI.h"
#include "skeletongraphicsI.h"
#include "fakedriver.h"
#include "simplenavdriver.h"
#include "astardriver.h"
#include "skeletondriver.h"

using namespace std;
using namespace orca;
using namespace pathplanner;

AlgoHandler::AlgoHandler( const orcaice::Context & context )
    : driver_(0),
      pathPlannerTaskProxy_(0),
      context_(context)
{
    initNetwork();
    initDriver();
}

AlgoHandler::~AlgoHandler()
{
    delete pathPlannerTaskProxy_;
}

void
AlgoHandler::initNetwork()
{
    //
    // REQUIRED INTERFACE: OgMap
    //

    while( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::OgMapPrx>( context_, ogMapPrx_, "OgMap" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            context_.tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }

    //
    // PROVIDED INTERFACES
    //
    

    // PathPlanner2d
    // create the proxy/buffer for tasks
    pathPlannerTaskProxy_ = new orcaice::PtrProxy<PathPlanner2dTaskPtr>; 
    pathPlannerDataProxy_ = new orcaice::PtrProxy<PathPlanner2dDataPtr>;

    pathPlannerI_ = new PathPlanner2dI( *pathPlannerTaskProxy_, *pathPlannerDataProxy_, context_ );
    Ice::ObjectPtr pathPlannerObj = pathPlannerI_;
    
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, pathPlannerObj, "PathPlanner2d" );
}

void
AlgoHandler::initDriver()
{
    // get the map once    
    orca::OgMapDataPtr ogMapDataPtr;
    try
    {
        ogMapDataPtr = ogMapPrx_->getData();
    }
    catch ( const orca::DataNotExistException & e )
    {
        std::stringstream ss;
        ss << "algohandler::initDriver: DataNotExistException: "<<e.what;
        context_.tracer()->warning( ss.str() );
    }


    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";

    double traversabilityThreshhold = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"TraversabilityThreshhold", 0.3 );
    double robotDiameterMetres = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"RobotDiameterMetres", 0.8 );
    int doPathOptimization = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"DoPathOptimization", 0 );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(), prefix+"Driver", "simplenav" );
    context_.tracer()->debug( std::string("loading ")+driverName+" driver",3);
    if ( driverName == "simplenav" )
    {
        driver_ = new SimpleNavDriver( ogMapDataPtr,
                                       robotDiameterMetres,
                                       traversabilityThreshhold,
                                       doPathOptimization );
    }
    else if ( driverName == "skeletonnav" || driverName == "sparseskeletonnav" )
    {
        // QGraphics2d
        SkeletonGraphicsI* graphicsI = new SkeletonGraphicsI( context_, "SkeletonGraphics" );
        Ice::ObjectPtr graphicsObj = graphicsI;
        orcaice::createInterfaceWithTag( context_, graphicsObj, "SkeletonGraphics" ); 

        bool useSparseSkeleton = (driverName == "sparseskeletonnav");
        
        try {
            driver_ = new SkeletonDriver( ogMapDataPtr,
                                      graphicsI,
                                      robotDiameterMetres,
                                      traversabilityThreshhold,
                                      doPathOptimization,
                                      useSparseSkeleton );
        }
        catch ( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Trouble constructing a skeletondriver" << endl << "Problem was: " << e.what();
            context_.tracer()->error( ss.str() );
            throw orcapathplan::Exception( ss.str() );  // this will exit
        }
    }
    else if ( driverName == "astar" )
    {
        driver_ = new AStarDriver( ogMapDataPtr,
                                   robotDiameterMetres,
                                   traversabilityThreshhold,
                                   doPathOptimization  );
    }
    else if ( driverName == "fake" )
    {
        driver_ = new FakeDriver();
    }
    else {
        string errorStr = "Unknown driver type.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'simplenav', 'skeletonnav', 'sparseskeletonnav', 'astar', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
}

void 
AlgoHandler::run()
{
    assert( driver_ );
    assert( pathPlannerTaskProxy_ );

    // we are in a different thread now, catch all stray exceptions

    PathPlanner2dTaskPtr taskPtr; 
    PathPlanner2dDataPtr pathDataPtr = new PathPlanner2dData;   

    while ( isActive() )
    {
        try
        {
            
            //
            //  ======== waiting for a task (blocking) =======
            //
            context_.tracer()->info("waiting for a new task");
            
            while ( isActive() )
            {
                int ret = pathPlannerTaskProxy_->getNext( taskPtr, 1000 );
                if ( ret!=0 ) {
                    // context_.tracer()->info("waiting for a new task");      
                } else {
                    context_.tracer()->info("task arrived");  
                    break;
                }
            }
            
            //
            // ===== tell driver to compute the path ========
            //
            
            // input: ogmap, task; output: path
            try 
            {
                context_.tracer()->info("telling driver to compute the path now");
                driver_->computePath( taskPtr, pathDataPtr );
            }
            catch ( orcapathplan::Exception &e )
            {
                std::stringstream ss;
                // TODO: display taskPtr.
                ss << "Couldn't compute path: " // << orcaice::toString(taskPtr)
                << endl << "Problem was: " << e.what();
                context_.tracer()->error( ss.str() );
                
                switch( e.type() )
                {
                    case PathStartNotValid:             pathDataPtr->result = PathStartNotValid;
                    case PathDestinationNotValid:       pathDataPtr->result = PathDestinationNotValid;
                    case PathDestinationUnreachable:    pathDataPtr->result = PathDestinationUnreachable;
                    case OtherError:                    pathDataPtr->result = OtherError; 
                    case PathOk:                        ; //compiler wants me to handle this case, otherwise produces warning
                }
            }
    
            //
            // ======= send result (including error code) ===============
            //
            context_.tracer()->info("sending off the resulting path");
    
            // There are three methods to let other components know about the computed path:
            // 1. using the proxy
            if (taskPtr->prx!=0)
            {
                taskPtr->prx->setData( pathDataPtr );
            }
            // 2. and 3.: use getData or icestorm
            pathPlannerI_->localSetData( pathDataPtr );
    
            // resize the pathDataPtr: future tasks might not compute a path successfully and we would resend the old path
            pathDataPtr->path.resize( 0 );
    
        //
        // unexpected exceptions
        //
        } // try
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
        }
        catch ( const orcaice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            // once caught this beast in here, don't know who threw it 'St9bad_alloc'
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "unexpected exception from somewhere.");
        }
    
    } // end of while
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}
