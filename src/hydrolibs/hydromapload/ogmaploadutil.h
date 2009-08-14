/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef HYDRO_OGMAP_LOAD_UTIL_H
#define HYDRO_OGMAP_LOAD_UTIL_H

#include <vector>
#include <string>
#include <hydroogmap/hydroogmap.h>

class QImage;

namespace hydromapload {

    //
    // @author Alex Brooks
    // @brief Loads an occupancy grid from an image file.
    //
    // Allowed formats: anything qt can load
    //
    // Throws std::strings if there are problems.
    //
    void loadMap( const std::string          &filename,
                  bool                        negate,
                  int                        &numCellsX,
                  int                        &numCellsY,
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
                  hydroogmap::OgMap &ogMap,
                  bool               negate,
                  float              offsetX,
                  float              offsetY,
                  float              offsetTheta,
                  float              metresPerCell );

    //
    // @author Alex Brooks
    // @brief Saves an occupancy grid to an image file.
    //
    void writeToQImage( const hydroogmap::OgMap &ogMap,
                        QImage &qImage,
                        bool negate = false );

    //
    // @author Alex Brooks
    // @brief Saves an occupancy grid to an image file.
    //
    void saveMap( const std::string       &filename,
                  const hydroogmap::OgMap &ogMap,
                  bool negate = false );
}

#endif
