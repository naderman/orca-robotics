/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
// Ensure that assertions are compiled in
#undef NDEBUG

#include <iostream>
#include <orcamapload/ogmaploadutil.h>
#include <orcaogmap/orcaogmap.h>
#include <orca/ogmap.h>

using namespace std;
using namespace orcaogmap;

int main( int argc, char **argv )
{

    if ( argc < 2 )
    {
        cout << "USAGE: " << argv[0] << " <map_filename>" << endl;
        exit(1);
    }
    
    // load map from file into internal representation
    orcaogmap::OgMap internalMap;
    orcaogmapload::loadMap( argv[1],
                            internalMap,
                            false,
                            0.0,
                            0.0,
                            0.0,
                            0.1,
                            0.1 );
    cout << "Loaded map with number of cells (" << internalMap.numCellsX() << "," << internalMap.numCellsY() << ")" << endl;
    
    // convert to a Slice-defined ogmap
    orca::OgMapData sliceMap;
    orcaogmap::convert( internalMap, sliceMap );
    
    // convert back
    orcaogmap::OgMap internalMap2;
    orcaogmap::convert( sliceMap, internalMap2 );
    
    // alter map for testing
//     internalMap2.gridCell( 10, 10 ) = 133;
    
    // compare
    unsigned char cell1=0;
    unsigned char cell2=0;
    bool ret1;
    bool ret2;
    
    for (int x=0; x<internalMap.numCellsX(); x++)
    {
        for (int y=0; y<internalMap.numCellsY(); y++)
        {
            ret1 = internalMap.tryGridCell(x,y,cell1);
            ret2 = internalMap2.tryGridCell(x,y,cell2);
            if ( !ret1 || !ret2 ) {
                cout << "Error accessing cells in map" << endl;
                cout << "Test failed" << endl;
                return -1;
            }
            if (cell1!=cell2){
                cout << "Cells at (" << x << "," << y << ") are not the same" << endl;
                cout << "Test failed" << endl;
                return -1;
            }
        }
    }
    
    cout << "Test passed" << endl;
    return 0;
}

