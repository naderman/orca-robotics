#include <iostream>
#include <cstdlib>
using namespace std;
#include <hydromapload/ogmaploadutil.h>
#include <hydroogmap/hydroogmap.h>
#include <hydroogmap/oglostracer.h>
#include <hydropathplan/hydropathplan.h>

void testPathPlan( const hydropathplan::IPathPlanner2d &planner,
                   const hydroogmap::OgMap             &ogMap,
                   const hydropathplan::Cell2D         &start,
                   const hydropathplan::Cell2D         &goal )
{
    // Plan the path

    hydropathplan::Cell2DVector path;
    planner.computePath( start.x(), start.y(), goal.x(), goal.y(), path );

    // Check the path
    hydroogmap::OgLosTracer rayTracer(ogMap);
    if ( path.size() < 2 )
    {
        cout << "ERROR(testpathplan.cpp): Path too short: just one element." << endl;
        exit(1);
    }
    for ( unsigned int i=1; i < path.size(); i++ )
    {
        hydropathplan::Cell2D wp1 = path[i-1];
        hydropathplan::Cell2D wp2 = path[i];
        if ( !rayTracer.isClearCells( wp1.x(), wp1.y(), wp2.x(), wp2.y() ) )
        {
            cout<<"TRACE(testpathplan.cpp): OptimisePath messed up:" << endl;
            cout<<"TRACE(testpathplan.cpp): Couldn't get from wp "<<i-1<<" to "<<i<< endl;
            cout<<"TRACE(testpathplan.cpp):   " << wp1 << " --> " << wp2 << endl;
            cout<<"TRACE(testpathplan.cpp): entire path: " << endl;
            for ( unsigned int j=0; j < path.size(); j++ )
            {
                cout << "  " << j << ": " << path[j] << endl;
            }
            exit(1);
        }
    }
}

int main( int argc, char **argv )
{
    if ( argc < 2 )
    {
        cout << "USAGE: " << argv[0] << " <map_filename>" << endl;
        exit(1);
    }

    hydroogmap::OgMap ogMap;
    double offsetX=0, offsetY=0, offsetTheta=0, metresPerCell=0.1;
    hydromapload::loadMap( argv[1],
                           ogMap,
                           false,
                           offsetX,
                           offsetY,
                           offsetTheta,
                           metresPerCell );

    hydropathplan::Cell2D start( 50, 70 );
    hydropathplan::Cell2D goal( 550, 210 );

    const double robotDiameterMetres  = 0.6;
    const double traversabilityThresh = 0.3;
    const double extraNodeResolution  = 4.0;
    const bool   addExtraNodes = true;
    const bool   jiggleBadWaypoints = true;

    bool doPathOptimization;
    for ( int i=0; i < 1; i++ )
    {
        if ( i==0 ) 
            doPathOptimization = false;
        else
            doPathOptimization = true;

        // Skeleton
        {
            hydropathplan::SkeletonPathPlanner planner( ogMap,
                                                        robotDiameterMetres,
                                                        traversabilityThresh,
                                                        doPathOptimization );
            testPathPlan( planner, ogMap, start, goal );
        }
        // Sparse Skeleton
        {
            hydropathplan::SparseSkeletonPathPlanner planner( ogMap,
                                                              robotDiameterMetres,
                                                              traversabilityThresh,
                                                              doPathOptimization,
                                                              addExtraNodes,
                                                              extraNodeResolution,
                                                              jiggleBadWaypoints );
            testPathPlan( planner, ogMap, start, goal );
        }
        // SimpleNav
        {
            hydropathplan::SimpleNavPathPlanner planner( ogMap,
                                                         robotDiameterMetres,
                                                         traversabilityThresh,
                                                         doPathOptimization );
            testPathPlan( planner, ogMap, start, goal );    
        }
        // AStarNav
        {
            hydropathplan::AStarPathPlanner planner( ogMap,
                                                     robotDiameterMetres,
                                                     traversabilityThresh,
                                                     doPathOptimization );
            testPathPlan( planner, ogMap, start, goal );    
        }
    }

    cout<<"TRACE(testpathplan.cpp): test PASSED" << endl;
    return 0;
}
