/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <Ice/Ice.h> 

#include <iostream>

#include <orcaice/orcaice.h>

#include <orcapathplan/orcapathplan.h>
#include <orcamisc/orcamisc.h>

#include "gridpotentialdriver.h"

using namespace std;
using namespace orcaogmap;
using namespace orcapathplan;

namespace pathplanner {

bool GridPotentialDriver::areAllWaypointsInMap(const orca::PathPlanner2dDataPtr  & pathDataPtr)
{
    // Check whether startCell is within map
    if ( !ogMap_.coordsWithinMap( startWp_.target.p.x, startWp_.target.p.y) )
    {
        pathDataPtr->result = orca::PathStartNotValid;
        cout << "ERROR(gridpotentialdriver.cpp): Start waypoint outside map. Returning..." << endl;
        return false;
    }
    cout << "INFO(gridpotentialdriver.cpp): Start waypoint is within map" << endl;

    // Check whether coarse path is within map
    for (unsigned int i=0; i<coarsePath_.size(); i++)
    {
        if ( !ogMap_.coordsWithinMap( coarsePath_[i].target.p.x, coarsePath_[i].target.p.y) )
        {
            pathDataPtr->result = orca::PathDestinationNotValid;
            cout << "ERROR(gridpotentialdriver.cpp): Goal waypoint " << i << " is outside map. Returning..." << endl;
            return false;
        }
    }
    cout << "INFO(gridpotentialdriver.cpp): Goals path is within map" << endl;

    return true;
}

Cell2D 
GridPotentialDriver::getStartCell()
{
    int cellX, cellY;
    ogMap_.getCellIndices( startWp_.target.p.x, startWp_.target.p.y, cellX, cellY ); 
    return Cell2D( cellX, cellY ); 
}

Cell2D 
GridPotentialDriver::getGoalCell( unsigned int i)
{
    assert( coarsePath_.size() > i );
    int cellX, cellY;
    ogMap_.getCellIndices( coarsePath_[i].target.p.x, coarsePath_[i].target.p.y, cellX, cellY ); 
    return Cell2D( cellX, cellY ); 
}

void GridPotentialDriver::computePath( const orca::OgMapDataPtr          & ogMapDataPtr,
                                       const orca::PathPlanner2dTaskPtr  & taskPtr,
                                       const orca::PathPlanner2dDataPtr  & pathDataPtr )
{
    // Measuring performance
    orcamisc::CpuStopwatch watch;

    // Convert to local storage
    orcaogmap::convert( ogMapDataPtr, ogMap_ );
    startWp_ = taskPtr->start;
    coarsePath_ = taskPtr->coarsePath;

    // check start and goal waypoints
    if ( !areAllWaypointsInMap(pathDataPtr) ) return;

    // initialize variables
    FloatMap navMap;
    bool success;
    FloatMap distGrid;
    Cell2D startCell = getStartCell(); 

    // for each waypoint in the coarse path we need to compute the navigation fct and the path
    for (unsigned int i=0; i<coarsePath_.size(); i++)
    {
        Cell2D goalCell = getGoalCell( i );
        
        // ============ Compute navigation function ========= 
        cout << "INFO(gridpotentialdriver.cpp): Starting to calculate navigation function" << endl;

        watch.start();
        success = calcSimpleNavigation( ogMap_, navMap, startCell, config_ );
        watch.stop();
        cout << "calculating navigation fct took (" << i << ") took: " << watch.elapsedSeconds() << " s" << endl << endl;

        if ( !success )
        {
            cout << "ERROR(gridpotentialdriver.cpp): Navigation function could not be computed" << endl;
            pathDataPtr->result = orca::OtherError;
            return;
        }
        // ===================================================

        // =========== Compute path ========================
        Cell2DVector path;
        cout << "INFO(gridpotentialdriver.cpp): Calculating path now" << endl;
        watch.start();
        Result result = orcapathplan::calcPath( ogMap_, navMap, goalCell, path, config_.robotDiameterMetres );
        watch.stop();
        cout << "calcPath took (" << i << ") took: " << watch.elapsedSeconds() << " s" << endl << endl;

        if ( result!=PathOk )
        {
            cout << "ERROR(gridpotentialdriver.cpp): Path could not be computed" << endl;
            // this will set the result flag accordingly but won't touch the path
            orcapathplan::convert( result, pathDataPtr );
            return;
        }
        // =================================================
    
        // ============= Optional path optimization ================
        if ( config_.doPathOptimization )
        {
            // separate full path into a optimized short path
            cout << "INFO(gridpotentialdriver.cpp): Optimizing path now" << endl;
            Cell2DVector waycells;    
            watch.start();        
            optimizePath( ogMap_, path, waycells, config_.robotDiameterMetres );
            watch.stop();
            cout << "optimizePath took (" << i << ") took: " << watch.elapsedSeconds() << " s" << endl << endl;
            path = waycells;
        }
        // =========================================================
        
        // ====== Convert to an Orca object in global coordinate system. =====
        // ====== Will append latest path to the total pathDataPtr. ==========
        // ====== Not all data fields are filled in (e.g.tolerances) =========
        if (i==0)
        {
            // the first time we'll have to insert the start cell
            path.insert(path.begin(),startCell);
        }
        orcapathplan::convert( ogMap_, path, result, pathDataPtr );
        // ==================================================================
        
        // set last goal cell as new start cell
        startCell.set( goalCell.x(), goalCell.y() );
    }
    
}

}


