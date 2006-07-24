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
#include <string>

namespace maploadutil {

    //
    // @author Tim Arney (t.arney at cas.edu.au), Tobias Kaupp (t.kaupp at cas.edu.au), Alex Brooks
    // @brief Loads an occupancy grid from an image file.
    //
    // Allowed formats: anything gdk can load, plus .pnm.gz
    //
    // Throws std::strings if there are problems.
    //
    void loadMap( const std::string &filename,
                  bool negate,
                  int &numCellsX,
                  int &numCellsY,
                  std::vector<unsigned char> &cells );
}

#endif
