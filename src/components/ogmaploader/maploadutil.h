/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
