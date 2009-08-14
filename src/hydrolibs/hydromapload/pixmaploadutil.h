/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_PIXMAP_LOAD_UTIL_H
#define HYDRO_PIXMAP_LOAD_UTIL_H

#include <vector>
#include <string>

//
// @author Tobias Kaupp 
//
namespace hydromapload
{    

    // Loads the file 'filename'.
    // The vector 'pixels' is resized appropriately, to 3*numPixels.
    // The pixel order is 'R,G,B'.
    void loadMap( const std::string &filename,
                  int               &numCellsX,
                  int               &numCellsY,
                  std::vector<char> &pixels );

    void saveMap( const std::string       &filename,
                  int                      numCellsX,
                  int                      numCellsY,
                  const std::vector<char> &pixels );
}

#endif
