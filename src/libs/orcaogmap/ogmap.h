/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, George Mathews
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAOGMAP_OGMAP_H
#define ORCAOGMAP_OGMAP_H

#include <orcaogmap/genericmap.h>

namespace orcaogmap {

    //!
    //! A map of occupancy values, stored as 8-bit numbers in the range 0-254
    //!
    typedef GenericMap<unsigned char> OgMap;

    //! Signifies absolute certainty that the cell is empty
    //! @relatesalso OgMap
    const int CELL_EMPTY = 0;

    //! Signifies no knowledge of cell occupancy
    //! @relatesalso OgMap
    const int CELL_UNKNOWN = 127;

    //! Signifies absolute certainty that the cell is occupied
    //! Avoid 255 so that there's a definite midpoint.
    //! @relatesalso OgMap
    const int CELL_OCCUPIED = 254;

    //! Fuses the map tile 'update' into the master map, by replacing the values in
    //! 'master' with the values in 'update'.
    //! The master will be resized if necessary.  Any new cells whose
    //! occupancy cannot be determined by the update are set to 'newCells'.
    //! 
    //! Assumes: 
    //! - the resolutions match
    //! - the maps have zero orientation.
    void fuse( OgMap &master, const OgMap &update, unsigned char newCells=127 ); 
    
    //! Overlays two OgMaps, i.e. their occupancy values are bitwise or'd. 
    //! Resulting map is stored in 'master'.
    //! Assumes that all map parameters (offset, resolution, number of cells) are the same.
    void overlay( OgMap &master, const OgMap &slave ); 

    //! Convert an occupancy grid map to text
    std::string toText( const OgMap &map );

    //! Returns a character corresponding to occupancy (empty:' '; occupied:'#'; in-between:'-') 
    char displayOgmapCell(unsigned char cell);

}    

#endif
