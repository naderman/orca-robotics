/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <errno.h>
#include <QImage>
#include <QColor>
#include <iostream>
#include <sstream>
#include <gbxutilacfr/exceptions.h>
#include "pixmaploadutil.h"
#include <assert.h>

using namespace std;

namespace hydromapload {
    
    void loadMap( const std::string &filename,
                  int               &numCellsX,
                  int               &numCellsY,
                  std::vector<char> &pixels )
    {
        QImage qImage( QString(filename.c_str()) );
        if ( qImage.isNull() )
        {
            stringstream ss;
            ss << "failed to open " << filename;
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        numCellsX = qImage.width();
        numCellsY = qImage.height();
        pixels.resize( numCellsX * numCellsY * 3 );
    
        // Read in the image
        for ( int j = 0; j < qImage.height(); j++ )
        {
            for ( int i = 0; i < qImage.width(); i++ )
            { 
                QColor pixColor = qImage.pixel( i, j );
                
//                 pix.r = (unsigned char)pixColor.red();
//                 pix.g = (unsigned char)pixColor.green();
//                 pix.b = (unsigned char)pixColor.blue();
                       
                const int xi = i;
                const int yi = numCellsY-1-j;
                const int pixelI = numCellsX*(yi) + xi;

                pixels[3*pixelI+0]=pixColor.red();
                pixels[3*pixelI+1]=pixColor.green();
                pixels[3*pixelI+2]=pixColor.blue();
            }
        }        
    }
    
    void saveMap( const std::string       &filename,
                  int                      numCellsX,
                  int                      numCellsY,
                  const std::vector<char> &pixels )
    {
        QImage qImage( numCellsX, numCellsY, QImage::Format_RGB32 );
        assert( numCellsX*numCellsY == (int)pixels.size()*3 );

        // Write out the image
        for ( int j = 0; j < qImage.height(); j++ )
        {
            for ( int i = 0; i < qImage.width(); i++ )
            { 
                const int xi = i;
                const int yi = numCellsY-1-j;
                const int pixelI = numCellsX*(yi) + xi;

                QColor pixColor( pixels[3*pixelI+0],
                                 pixels[3*pixelI+1],
                                 pixels[3*pixelI+2] );

                qImage.setPixel( i, j, pixColor.rgb() );
            }
        }
        qImage.save( QString(filename.c_str()) );
    }
}
