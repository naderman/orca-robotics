/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_OG_MAP_H
#define ORCAOBJ_OG_MAP_H

#include <orca/ogmap.h>

namespace orcaobj
{

//! Get a @b reference to a grid cell (no bounds checking is performed)
inline unsigned char& gridCell( orca::OgMapData& obj, int indX, int indY )
{ 
    return obj.data[ indY*obj.numCellsX + indX ]; 
}

//! Calculate index of a cell in a grid map based on its row and column.
inline int gridCellIndex( const orca::OgMapData& obj, int indX, int indY )
{ 
    return indY*obj.numCellsX + indX; 
}

//!
//! Display OG map cell
//!
char displayOgmapCell(unsigned char cell);

//! Converts to string
std::string toString( const orca::OgMapData& );

//! Displays the contents of the map on to the console
//! (large maps can be displyed with a small font size)
std::string toVerboseString( const orca::OgMapData& );

} // namespace

#endif
