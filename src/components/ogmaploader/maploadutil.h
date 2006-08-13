/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_OGMAP_LOAD_UTIL_H
#define ORCA2_OGMAP_LOAD_UTIL_H

#include <vector>

namespace maploadutil {

//! Loads an occupancy grid map from a bitmap file. Supported file types is anything gdk can load.
//! Parameters: negate (1 means white is occupied), cells (0: unoccupied, 254: occupied, 127: unknown)
int loadBitmap( const char *filename,
                    bool negate,
                    int &numCellsX,
                    int &numCellsY,
                    std::vector<unsigned char> &cells );
                    
//! As above but for gzipped pnm's
int loadPnmGz( const char *filename, 
                bool negate,
                int &numCellsX,
                int &numCellsY,
                std::vector<unsigned char> &cells );
}

#endif
