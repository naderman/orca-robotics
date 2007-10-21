/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
#include "fakedriver.h"
#include "skeletondriver.h"

#include <orcaogmap/orcaogmap.h>

#ifdef QT4_FOUND
    #include "skeletongraphicsI.h"
#endif


using namespace std;
using namespace orca;
using namespace pathplanner;

namespace {

    class DistBasedCostEvaluator : public orcapathplan::CostEvaluator {
    public:
        DistBasedCostEvaluator( double distanceThreshold,
                                double costMultiplier )
            : distanceThreshold_(distanceThreshold),
              costMultiplier_(costMultiplier)
            {}

        double costAtDistFromObstacle( double distInMetres ) const
            {
                if ( distInMetres < 0 )
                    return NAN;
                else if ( distInMetres < distanceThreshold_ ) 
                    return costMultiplier_; 
                else 
                    return 1;
            }
        
    private:
        double distanceThreshold_;
        double costMultiplier_;
    };

}

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
    if ( costEvaluator_ ) delete costEvaluator_;
}

void
AlgoHandler::initNetwork()
{
    //
    // REQUIRED INTERFACE: OgMap
    //

    while( !isStopping() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::OgMapPrx>( context_, ogMapPrx_, "OgMap" );
            break;
        }
        catch ( const std::exception & e )
        {
            stringstream ss;
            ss << "AlgoHandler: failed to connect to ogMap interface: " << e.what();
            context_.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }
    
    // Optional hazard map
    std::string prefix = context_.tag() + ".Config.";
    useHazardMap_ = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"UseHazardMap", 0 );
    
    if (useHazardMap_)
    {
        while( !isStopping() )
        {
            try
            {
                orcaice::connectToInterfaceWithTag<orca::OgMapPrx>( context_, hazardMapPrx_, "HazardMap" );
                break;
            }
            catch ( const std::exception & e )
            {
                stringstream ss;
                ss << "AlgoHandler: failed to connect to hazardMap interface: " << e.what();
                context_.tracer()->error( ss.str() );
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
            }
            // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
            //       but if this happens it's ok if we just quit.
        }
    }

    //
    // PROVIDED INTERFACES
    //
    

    // PathPlanner2d
    // create the proxy/buffer for tasks
    pathPlannerTaskProxy_ = new hydroutil::Proxy<PathPlanner2dTask>; 
    pathPlannerDataProxy_ = new hydroutil::Proxy<PathPlanner2dData>;

    pathPlannerI_ = new PathPlanner2dI( *pathPlannerTaskProxy_, *pathPlannerDataProxy_, context_ );
    Ice::ObjectPtr pathPlannerObj = pathPlannerI_;
    
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, pathPlannerObj, "PathPlanner2d" );
}

void
AlgoHandler::initDriver()
{
    // get the og map once    
    orca::OgMapData ogMapSlice;
    try
    {
        ogMapSlice = ogMapPrx_->getData();
        stringstream ss;
        ss << "AlgoHandler::initDriver(): got ogMap: " << orcaice::toString( ogMapSlice );
        context_.tracer()->info( ss.str() );
    }
    catch ( const orca::DataNotExistException & e )
    {
        std::stringstream ss;
        ss << "algohandler::initDriver: DataNotExistException: "<<e.what;
        context_.tracer()->warning( ss.str() );
    }
    // convert into internal representation
    orcaogmap::convert(ogMapSlice,ogMap_);
    
    // hazard map is optional
    if (useHazardMap_)
    {
        orca::OgMapData hazardMapSlice;
        orcaogmap::OgMap hazardMap;
        try
        {
            hazardMapSlice = hazardMapPrx_->getData();
        }
        catch ( const orca::DataNotExistException & e )
        {
            std::stringstream ss;
            ss << "algohandler::initDriver: DataNotExistException: "<<e.what;
            context_.tracer()->warning( ss.str() );
        }
        // convert into internal representation
        orcaogmap::convert(hazardMapSlice,hazardMap);
        // overlay the two maps, result is stored in ogMap
        orcaogmap::overlay(ogMap_,hazardMap);
    }


    //
    // Read settings
    //
    std::string prefix = context_.tag() + ".Config.";

    double traversabilityThreshhold = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"TraversabilityThreshhold", 0.3 );
    double robotDiameterMetres = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"RobotDiameterMetres", 0.8 );
    int doPathOptimization = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"DoPathOptimization", 0 );
    bool jiggleWaypointsOntoClearCells = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"JiggleWaypointsOntoClearCells", true );

    // based on the config parameter, create the right driver
    string driverName = orcaice::getPropertyWithDefault( context_.properties(), prefix+"Driver", "simplenav" );
    context_.tracer()->debug( std::string("loading ")+driverName+" driver",3);
    
    if ( driverName == "simplenav" || driverName == "astar")
    {
        orcapathplan::IPathPlanner2d *pathPlanner=NULL;
        
        if (driverName == "simplenav") {
            pathPlanner = new orcapathplan::SimpleNavPathPlanner( ogMap_,
                                                           robotDiameterMetres,
                                                           traversabilityThreshhold,
                                                           doPathOptimization );
        } else if (driverName == "astar") {
            pathPlanner = new orcapathplan::AStarPathPlanner( ogMap_,
                                                           robotDiameterMetres,
                                                           traversabilityThreshhold,
                                                           doPathOptimization );
        }
        driver_ = new GenericDriver( pathPlanner,
                                     ogMap_,
                                     robotDiameterMetres,
                                     traversabilityThreshhold,
                                     doPathOptimization,
                                     jiggleWaypointsOntoClearCells,
                                     context_ );
    }    
    else if ( driverName == "skeletonnav" || driverName == "sparseskeletonnav" )
    {
        bool useSparseSkeleton = (driverName == "sparseskeletonnav");

        double distanceThreshold = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Skeleton.Cost.DistanceThreshold", 0.3 );
        double costMultiplier = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Skeleton.Cost.CostMultiplier", 10 );

        costEvaluator_ = new DistBasedCostEvaluator( distanceThreshold, costMultiplier );

        try {
            driver_ = new SkeletonDriver( ogMap_,
                                          robotDiameterMetres,
                                          traversabilityThreshhold,
                                          doPathOptimization,
                                          jiggleWaypointsOntoClearCells,
                                          useSparseSkeleton,
                                          *costEvaluator_,
                                          context_ );
        }
        catch ( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Trouble constructing a skeletondriver" << endl << "Problem was: " << e.what();
            context_.tracer()->error( ss.str() );
            throw orcapathplan::Exception( ss.str() );  // this will exit
        }
        
    }
    else if ( driverName == "fake" )
    {
        driver_ = new FakeDriver();
    }
    else {
        string errorStr = "Unknown driver type.";
        context_.tracer()->error( errorStr);
        context_.tracer()->info( "Valid driver values are {'simplenav', 'skeletonnav', 'sparseskeletonnav', 'astar', 'fake'}" );
        throw hydroutil::Exception( ERROR_INFO, errorStr );
    }

    context_.tracer()->debug("driver instantiated",5);
}

void 
AlgoHandler::run()
{
    assert( driver_ );
    assert( pathPlannerTaskProxy_ );

    // we are in a different thread now, catch all stray exceptions

    PathPlanner2dTask task; 
    PathPlanner2dData pathData;   

    while ( !isStopping() )
    {
        try
        {
            
            //
            //  ======== waiting for a task (blocking) =======
            //
            context_.tracer()->info("waiting for a new task");
            bool haveTask=false;
            
            while ( !isStopping() )
            {
                int ret = pathPlannerTaskProxy_->getNext( task, 1000 );
                if ( ret==0 ) {
                    haveTask = true;
                    context_.tracer()->info("task arrived");  
                    break;
                }
            }
            
            // the only way of getting out of the above loop without a task
            // is if the user pressed Ctrl+C, ie we have to quit
            if (!haveTask) break;
            
            //
            // ===== tell driver to compute the path ========
            //
            
            // input: ogmap, task; output: path
            try 
            {
                context_.tracer()->info("telling driver to compute the path now");
                driver_->computePath( task, pathData );
                pathData.result = orca::PathOk;
                pathData.resultDescription = "All good";
            }
            catch ( orcapathplan::Exception &e )
            {
                std::stringstream ss;
                ss << "Couldn't compute path: " << orcaice::toVerboseString(task) << endl << "Problem was: " << e.what();
                context_.tracer()->error( ss.str() );
                pathData.resultDescription = ss.str();
                
                switch( e.type() )
                {
                    case orcapathplan::PathStartNotValid:             
                        pathData.result = orca::PathStartNotValid;
                        break;
                    case orcapathplan::PathDestinationNotValid:       
                        pathData.result = orca::PathDestinationNotValid;
                        break;
                    case orcapathplan::PathDestinationUnreachable:    
                        pathData.result = orca::PathDestinationUnreachable;
                        break;
                    case orcapathplan::OtherError:                    
                        pathData.result = orca::PathOtherError; 
                        break;
                    default : {}
                        // nothing
                }
            }
    
            //
            // ======= send result (including error code) ===============
            //
            context_.tracer()->info("sending off the resulting path");
            context_.tracer()->debug(orcaice::toVerboseString(pathData));
    
            // There are three methods to let other components know about the computed path:
            // 1. using the proxy
            if (task.prx!=0)
            {
                task.prx->setData( pathData );
            }
            // 2. and 3.: use getData or icestorm
            pathPlannerI_->localSetData( pathData );
    
            // resize the pathData: future tasks might not compute a path successfully and we would resend the old path
            pathData.path.resize( 0 );
    
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
        catch ( const hydroutil::Exception & e )
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
