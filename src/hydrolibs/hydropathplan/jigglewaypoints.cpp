#include "jigglewaypoints.h"
#include <iostream>
#include <hydropathplan/hydropathplan.h>
#include <hydropathplan/util.h>
#include <hydroutil/rand.h>

using namespace std;

namespace hydropathplan {

void jiggleOntoClearCell( int &cellX, int &cellY,
                          const hydroogmap::OgMap &ogMap,
                          double traversabilityThreshhold )
{
    if ( hydropathplan::util::isTraversable( ogMap, cellX, cellY, traversabilityThreshhold ) &&
         hydropathplan::util::isTraversableAll8Neighbors( ogMap, cellX, cellY, traversabilityThreshhold ) )
        return;

    for ( int numSteps=2; numSteps < 10000; numSteps = (int)(numSteps*1.5) )
    {
        hydropathplan::Cell2D cell(cellX,cellY);

        // perform a random walk of numSteps steps.
        for ( int i=0; i < numSteps; i++ )
        {
            cell = hydropathplan::surroundCell( cell, (int)(hydroutil::randNum(0,8)) );

            if (  hydropathplan::util::isTraversable( ogMap, cell.x(), cell.y(), traversabilityThreshhold ) &&
                  hydropathplan::util::isTraversableAll8Neighbors( ogMap, cell.x(), cell.y(), traversabilityThreshhold ) )
            {
                cout<<"TRACE(jigglewaypoints.cpp): jiggled from " << cellX << ", " << cellY << " to " << cell << endl;

                cellX = cell.x();
                cellY = cell.y();
                return;
            }
        }
    }
    throw( hydropathplan::Exception( ERROR_INFO, "Couldn't jiggle onto clear cell." ) );
}

}

