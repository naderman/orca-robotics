/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDROOGMAP_OGMAP_H
#define HYDROOGMAP_OGMAP_H

#include <hydroogmap/genericmap.h>
#include <hydroportability/sharedlib.h>

namespace hydroogmap {

    //!
    //! A map of occupancy values, stored as 8-bit numbers in the range 0-254
    //!
    typedef GenericMap<unsigned char> OgMap;

    //! Signifies absolute certainty that the cell is empty
    //! @relatesalso OgMap
    SOEXPORT const unsigned char CELL_EMPTY = 0;

    //! Signifies no knowledge of cell occupancy
    //! @relatesalso OgMap
    SOEXPORT const unsigned char CELL_UNKNOWN = 127;

    //! Signifies absolute certainty that the cell is occupied
    //! Avoid 255 so that there's a definite midpoint.
    //! @relatesalso OgMap
    SOEXPORT const unsigned char CELL_OCCUPIED = 254;

    inline SOEXPORT bool isTraversable( unsigned char cellVal,
                                        int occupiedLowerBound=CELL_UNKNOWN )
    { return (cellVal < occupiedLowerBound); }

    inline SOEXPORT bool isWorldCellTraversable( const OgMap &ogMap,
                                                 double worldX,
                                                 double worldY,
                                                 int occupiedLowerBound=CELL_UNKNOWN )
    {
        unsigned char cellVal;
        if ( !ogMap.tryWorldCell( worldX, worldY, cellVal ) ) return false;
        return isTraversable( cellVal, occupiedLowerBound );
    }

    //! Fuses the map tile 'update' into the master map, by replacing the values in
    //! 'master' with the values in 'update'.
    //! The master will be resized if necessary.  Any new cells whose
    //! occupancy cannot be determined by the update are set to 'newCells'.
    //! 
    //! Assumes: 
    //! - the resolutions match
    //! - the maps have zero orientation.
    SOEXPORT void fuse( OgMap &master, const OgMap &update, unsigned char newCells=127 ); 
    
    //! Overlays two OgMaps, i.e. their occupancy values are bitwise or'd. 
    //! Resulting map is stored in 'master'.
    //! Assumes that all map parameters (offset, resolution, number of cells) are the same.
    SOEXPORT void overlay( OgMap &master, const OgMap &slave ); 

    //! Convert an occupancy grid map to text
    SOEXPORT std::string toText( const OgMap &map );

    //! Returns a character corresponding to occupancy (empty:' '; occupied:'#'; in-between:'-') 
    SOEXPORT char displayOgmapCell(unsigned char cell);

}    

#endif
