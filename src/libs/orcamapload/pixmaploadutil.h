/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_PIXMAP_LOAD_UTIL_H
#define ORCA2_PIXMAP_LOAD_UTIL_H

#include <vector>
#include <string>

#include <orca/pixmap.h>

//
// @author Tobias Kaupp 
//
namespace orcapixmapload
{    
    
    void loadMap( const std::string &filename,
                  int &numCellsX,
                  int &numCellsY,
                  orca::Pixels &pixels );
}

#endif
