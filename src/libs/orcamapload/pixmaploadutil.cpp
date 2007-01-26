/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <errno.h>
#include <zlib.h>
#include <QImage>
#include <QColor>
#include <iostream>
#include <sstream>

#include "pixmaploadutil.h"

using namespace std;

namespace orcapixmapload {
    
    void loadMap( const std::string &filename,
                  int &numCellsX,
                  int &numCellsY,
                  orca::Pixels &pixels )
    {
        QImage qImage( QString(filename.c_str()) );
        if ( qImage.isNull() )
        {
            stringstream ss;
            ss << "ERROR(orcapixmapload.cpp): failed to open " << filename;
            throw( ss.str() );
        }
        numCellsX = qImage.width();
        numCellsY = qImage.height();
        pixels.resize( numCellsX * numCellsY );
    
        // Read in the image
        for ( int j = 0; j < qImage.height(); j++ )
        {
            for ( int i = 0; i < qImage.width(); i++ )
            { 
                orca::Pixel pix;
                QColor pixColor = qImage.pixel( i, j );
                
                pix.r = pixColor.red();
                pix.g = pixColor.green();
                pix.b = pixColor.blue();
    
                int xi = i;
                int yi = numCellsY-1-j;
                int index = numCellsX*(yi) + xi;
                pixels[index] = pix;
            }
        }
        
    }
    
}
