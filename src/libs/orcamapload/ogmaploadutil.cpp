/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <errno.h>
#include <zlib.h>
#include <QImage>
#include <QColor>
#include <iostream>
#include <sstream>

#include "ogmaploadutil.h"
#include <orcaogmap/orcaogmap.h>

using namespace std;

namespace orcaogmapload {

void
loadMap( const std::string &filename,
         bool negate,
         int &numCellsX,
         int &numCellsY,
         std::vector<unsigned char> &cells )
{
    QImage qImage( QString(filename.c_str()) );
    if ( qImage.isNull() )
    {
        stringstream ss;
        ss << "ERROR(ogmaploader.cpp): failed to open " << filename;
        throw( ss.str() );
    }
    numCellsX = qImage.width();
    numCellsY = qImage.height();
    cells.resize( numCellsX * numCellsY );

    // Read in the image
    for ( int j = 0; j < qImage.height(); j++ )
    {
        for ( int i = 0; i < qImage.width(); i++ )
        { 
            QColor pixColor = qImage.pixel( i, j );
            int gray = (pixColor.red()+pixColor.green()+pixColor.blue())/3;

            // Remember: don't use 255, so we have a perfect 0.5 at 254/2.
            if ( gray > 254 ) gray = 254;

            if ( !negate )
                gray = 254 - gray;

            unsigned char cellVal = (unsigned char) gray;
            int xi = i;
            int yi = numCellsY-1-j;
            int index = numCellsX*(yi) + xi;
            cells[index] = cellVal;
        }
    }
}

void loadMap( const std::string &filename,
              hydroogmap::OgMap  &ogMap,
              bool               negate,
              float              offsetX,
              float              offsetY,
              float              offsetTheta,
              float              metresPerCellX,
              float              metresPerCellY )
{
    int numCellsX=0, numCellsY=0;
    std::vector<unsigned char> cells;
    
    orcaogmapload::loadMap( filename,
                            negate,
                            numCellsX,
                            numCellsY,
                            cells );
    
    ogMap.reallocate( numCellsX, numCellsY );
    memcpy( ogMap.data(), &(cells[0]), numCellsX*numCellsY*sizeof(unsigned char) );
    
    ogMap.offset().p.x = offsetX;
    ogMap.offset().p.y = offsetY;
    ogMap.offset().o   = offsetTheta;

    ogMap.setMetresPerCellX( metresPerCellX );
    ogMap.setMetresPerCellY( metresPerCellY );
}

}

