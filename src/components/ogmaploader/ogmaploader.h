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

//!
//! @author Tim Arney (t.arney at cas.edu.au), Tobias Kaupp (t.kaupp at cas.edu.au), Alex Brooks
//! @brief Loads an occupancy grid from an image file and sticks it into an Orca OgMap object
//!

#ifndef OGMAP_LOADER
#define OGMAP_LOADER

#include <orca/ogmap.h>

class OgMapLoader
{
    
public:
         
    OgMapLoader( std::string filename,
                 float metresPerCellX,
                 float metresPerCellY,
                 float xOrigin,
                 float yOrigin,
                 float originTheta,
                 bool  negate );
    ~OgMapLoader();
        
    //! Access function for OgMap
    void getMap(orca::OgMapDataPtr &toMap);
    bool hasMap() const { return hasMap_; }

private:

    int loadMap( std::string filename,
                 bool  negate );
        
    // Loads a map from a gzipped file
    int loadPnmGz(const char *filename, bool negate);
        
    // Loads a map from various bitmap files
    int loadBitmap(const char *filename, bool negate);
        
    // Data
    orca::OgMapDataPtr ogMap_;

    bool hasMap_;

    ///////////////////////////////////////////////////////////////////////

    static const float DEFAULT_SCALE    = 1.0;
    static const float DEFAULT_X_ORIGIN = 0.0;
    static const float DEFAULT_Y_ORIGIN = 0.0;
    static const bool  DEFAULT_NEGATE   = false;
};

#endif
