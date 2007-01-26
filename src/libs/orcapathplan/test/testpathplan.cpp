#include <iostream>
using namespace std;
#include <orcamapload/ogmaploadutil.h>
#include <orcaogmap/orcaogmap.h>
#include <orcapathplan/orcapathplan.h>
using namespace orcapathplan;

void testPathPlan( const IPathPlanner2d   &planner,
                   const orcaogmap::OgMap &ogMap,
                   const Cell2D           &start,
                   const Cell2D           &goal )
{
    // Plan the path

    Cell2DVector path;
    planner.computePath( start.x(), start.y(), goal.x(), goal.y(), path );

    // Check the path
    orcaogmap::OgLosTracer rayTracer(ogMap);
    if ( path.size() < 2 )
    {
        cout << "ERROR(testpathplan.cpp): Path too short: just one element." << endl;
        exit(1);
    }
    for ( unsigned int i=1; i < path.size(); i++ )
    {
        Cell2D wp1 = path[i-1];
        Cell2D wp2 = path[i];
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

    orcaogmap::OgMap ogMap;
    orcaogmapload::loadMap( argv[1],
                            ogMap,
                            false,
                            0.0, 0.0, 0.0, 0.1, 0.1 );
    Cell2D start( 50, 70 );
    Cell2D goal( 550, 210 );

    const double robotDiameterMetres  = 0.6;
    const double traversabilityThresh = 0.3;
    
    bool doPathOptimization;
    for ( int i=0; i < 1; i++ )
    {
        if ( i==0 ) 
            doPathOptimization = false;
        else
            doPathOptimization = true;

        // Skeleton
        {
            SkeletonPathPlanner planner( ogMap,
                                         robotDiameterMetres,
                                         traversabilityThresh,
                                         doPathOptimization );
            testPathPlan( planner, ogMap, start, goal );
        }
        // Sparse Skeleton
        {
            SparseSkeletonPathPlanner planner( ogMap,
                                               robotDiameterMetres,
                                               traversabilityThresh,
                                               doPathOptimization );
            testPathPlan( planner, ogMap, start, goal );
        }
        // SimpleNav
        {
            SimpleNavPathPlanner planner( ogMap,
                                          robotDiameterMetres,
                                          traversabilityThresh,
                                          doPathOptimization );
            testPathPlan( planner, ogMap, start, goal );    
        }
        // AStarNav
        {
            AStarPathPlanner planner( ogMap,
                                      robotDiameterMetres,
                                      traversabilityThresh,
                                      doPathOptimization );
            testPathPlan( planner, ogMap, start, goal );    
        }
    }

    cout<<"TRACE(testpathplan.cpp): test PASSED" << endl;
    return 0;
}
