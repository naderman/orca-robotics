#include "pathplannerfactory.h"
#include <iostream>
#include <orcaice/orcaice.h>
#ifdef QT4_FOUND
#  include "skeletongraphicsI.h"
#endif

using namespace std;

namespace pathplanner {

//////////////////////////////////////////////////////////////////////

namespace {

    //
    // Cost function for planner
    //
    class DistBasedCostEvaluator : public hydropathplan::CostEvaluator {
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

#ifdef QT4_FOUND
    void
    publishSkeletonGraphics( hydropathplan::IPathPlanner2d *pathPlanner,
                             bool useSparseSkeleton,
                             const hydroogmap::OgMap &ogMap,
                             double            robotDiameterMetres,
                             double            traversabilityThreshhold,
                             bool              doPathOptimization,
                             orcaice::Context &context )
    {                   
        // Instantiate QGraphics2d
        context.tracer().info( "Instantiating QGraphics2d Interface" );
        SkeletonGraphicsI *skelGraphicsI = new SkeletonGraphicsI( context, "SkeletonGraphics" );
        Ice::ObjectPtr graphicsObj = skelGraphicsI;
        orcaice::createInterfaceWithTag( context, graphicsObj, "SkeletonGraphics" ); 

        if ( !useSparseSkeleton )
        {
            hydropathplan::SkeletonPathPlanner *skelPathPlanner = 
                dynamic_cast<hydropathplan::SkeletonPathPlanner*>( pathPlanner );
            assert( skelPathPlanner != NULL );
            skelGraphicsI->localSetSkel( ogMap, &(skelPathPlanner->skeleton()) );
        }
        else
        {
            const bool displayDenseSkelFirst = false;
            if ( displayDenseSkelFirst )
            {
                hydropathplan::SkeletonPathPlanner *temp = 
                    new hydropathplan::SkeletonPathPlanner( ogMap,
                                                            robotDiameterMetres,
                                                            traversabilityThreshhold,
                                                            doPathOptimization );
                skelGraphicsI->localSetSkel( ogMap, &(temp->skeleton()) );
            }        
        
            hydropathplan::SparseSkeletonPathPlanner *sparseSkelPathPlanner = 
                dynamic_cast<hydropathplan::SparseSkeletonPathPlanner*>( pathPlanner );
            assert( sparseSkelPathPlanner != NULL );
            skelGraphicsI->localSetSkel( ogMap,
                                         &(sparseSkelPathPlanner->denseSkel()),
                                         &(sparseSkelPathPlanner->sparseSkel()) );
        }
    }
#endif

}

//////////////////////////////////////////////////////////////////////

hydropathplan::IPathPlanner2d*
PathPlannerFactory::getPathPlanner( hydroogmap::OgMap  &ogMap, 
                                    double              traversabilityThreshhold,
                                    double              robotDiameterMetres,
                                    hydroutil::Context  context )
{
    const hydroutil::Properties &prop = context.properties();

    //
    // Generic settings
    //
    bool   doPathOptimization = prop.getPropertyAsIntWithDefault( "DoPathOptimization", 0 );

    // based on the config parameter, create the right driver
    string algorithm = prop.getPropertyWithDefault( "Algorithm", "simplenav" );
    context.tracer().debug( std::string("loading ")+algorithm+" algorithm",3);
    
    if ( algorithm == "simplenav" || algorithm == "astar")
    {
        if ( algorithm == "simplenav" )
        {
            return new hydropathplan::SimpleNavPathPlanner( ogMap,
                                                            robotDiameterMetres,
                                                            traversabilityThreshhold,
                                                            doPathOptimization );
        } 
        else if (algorithm == "astar") 
        {
            return new hydropathplan::AStarPathPlanner( ogMap,
                                                        robotDiameterMetres,
                                                        traversabilityThreshhold,
                                                        doPathOptimization );
        }
    }
    else if ( algorithm == "skeletonnav" || algorithm == "sparseskeletonnav" )
    {
        double distanceThreshold = prop.getPropertyAsDoubleWithDefault( "Skeleton.Cost.DistanceThreshold", 0.3 );
        double costMultiplier = prop.getPropertyAsDoubleWithDefault( "Skeleton.Cost.CostMultiplier", 10 );

        DistBasedCostEvaluator costEvaluator( distanceThreshold, costMultiplier );

        bool useSparseSkeleton = (algorithm == "sparseskeletonnav");
        hydropathplan::IPathPlanner2d *pathPlanner;
        if ( useSparseSkeleton )
        {
            double addExtraSparseSkelNodes = prop.getPropertyAsDoubleWithDefault( "Skeleton.SparseSkelAddExtraNodes", 1 );
            double sparseSkelExtraNodeResolution = prop.getPropertyAsDoubleWithDefault( "Skeleton.SparseSkelExtraNodeResolution", 5 );
            pathPlanner = new hydropathplan::SparseSkeletonPathPlanner( ogMap,
                                                                        robotDiameterMetres,
                                                                        traversabilityThreshhold,
                                                                        doPathOptimization,
                                                                        addExtraSparseSkelNodes,
                                                                        sparseSkelExtraNodeResolution,
                                                                        costEvaluator );
        }
        else
        {
            pathPlanner = new hydropathplan::SkeletonPathPlanner( ogMap,
                                                                  robotDiameterMetres,
                                                                  traversabilityThreshhold,
                                                                  doPathOptimization,
                                                                  costEvaluator );
        }
#ifdef QT4_FOUND
        bool provideSkeletonGraphics = prop.getPropertyAsDoubleWithDefault( "Skeleton.ProvideGraphics", 1 );
        if ( provideSkeletonGraphics )
        {
            publishSkeletonGraphics( pathPlanner,
                                     useSparseSkeleton,
                                     ogMap,
                                     robotDiameterMetres,
                                     traversabilityThreshhold,
                                     doPathOptimization,
                                     orcaIceContext_ );
        }
#endif
        return pathPlanner;
    }

    // Only get to here if we didn't return a pathPlanner earlier.
    stringstream ssUnknown;
    ssUnknown << "Unknown algorithm: " << algorithm;
    throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ssUnknown.str() );
}

}

