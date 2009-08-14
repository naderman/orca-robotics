#include "obstacleutil.h"
#include <iostream>

using namespace std;

namespace hydrosim2d {

void
placeSquareObstacle( hydroogmap::OgMap &ogMap,
                     double x,
                     double y,
                     double width )
{
    hydroogmap::GridIndices gridIndices = ogMap.gridIndices( x, y );

    int radiusInCells = (int)((width/2.0) / ogMap.metresPerCell());
    for ( int i=-radiusInCells; i < radiusInCells; i++ )
    {
        for ( int j=-radiusInCells; j < radiusInCells; j++ )
        {
            if ( ogMap.cellWithinMap(gridIndices.x+i,gridIndices.y+j) )
                ogMap.gridCell(gridIndices.x+i,gridIndices.y+j) = hydroogmap::CELL_OCCUPIED;
        }
    }    
}

}

