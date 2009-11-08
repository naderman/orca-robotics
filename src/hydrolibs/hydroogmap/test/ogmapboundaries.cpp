#include <iostream>
#include <hydroogmap/ogmap.h>

using namespace std;
using namespace hydroogmap;

int main()
{
    const int numCells = 100;
    const double metresPerCell = 0.1;
    OgMap ogMap( numCells,
                 numCells,
                 Frame2d(0,0,0),
                 metresPerCell,
                 0 );

    cout<<"TRACE(ogmapboundaries.cpp): min/max world X: " << ogMap.minWorldX() << "/" << ogMap.maxWorldX() << endl;

    WorldCoords bottomLeft( ogMap.minWorldX(), ogMap.minWorldY() );
    WorldCoords topRight( ogMap.maxWorldX(), ogMap.maxWorldY() );
    const bool bottomLeftWithinMap = ogMap.coordsWithinMap( bottomLeft );
    const bool topRightWithinMap = ogMap.coordsWithinMap( topRight );

    cout<<"TRACE(ogmapboundaries.cpp): bottomLeftWithinMap: " << bottomLeftWithinMap << endl;
    cout<<"TRACE(ogmapboundaries.cpp): topRightWithinMap: " << topRightWithinMap << endl;
    assert( bottomLeftWithinMap && !topRightWithinMap );

//     double x = 0;
//     for ( int i=0; i < numCells; i++ )
//         x += metresPerCell;
//     bool withinMap = ogMap.coordsWithinMap( x, 5 );
//     cout<<"TRACE(ogmapboundaries.cpp): withinMap: " << withinMap << endl;

    cout<<"TRACE(ogmapboundaries.cpp): test PASSED" << endl;
    return 0;
}
