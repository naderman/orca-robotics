/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_OGMAP_LOAD_UTIL_H
#define ORCA2_OGMAP_LOAD_UTIL_H

#include <vector>
#include <string>
#include <hydroogmap/hydroogmap.h>

#include <orca/pixmap.h>

namespace orcaogmapload {

    //
    // @author Alex Brooks
    // @brief Loads an occupancy grid from an image file.
    //
    // Allowed formats: anything qt can load
    //
    // Throws std::strings if there are problems.
    //
    void loadMap( const std::string &filename,
                  bool negate,
                  int &numCellsX,
                  int &numCellsY,
                  std::vector<unsigned char> &cells );

    
    //
    // @author Alex Brooks
    // @brief Loads an occupancy grid from an image file, into hydroogmap::OgMap format.
    //
    // Allowed formats: anything qt can load
    //
    // Throws std::strings if there are problems.
    //
    void loadMap( const std::string &filename,
                  hydroogmap::OgMap  &ogMap,
                  bool               negate,
                  float              offsetX,
                  float              offsetY,
                  float              offsetTheta,
                  float              metresPerCellX,
                  float              metresPerCellY  );
}

#endif
