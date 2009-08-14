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
#include "ogmaploadutil.h"
#include <hydroogmap/hydroogmap.h>

using namespace std;

namespace hydromapload {

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
        ss << "failed to open " << filename;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
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
            const int xi = i;
            const int yi = numCellsY-1-j;
            const int index = numCellsX*(yi) + xi;
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
              float              metresPerCell )
{
    int numCellsX=0, numCellsY=0;
    std::vector<unsigned char> cells;
    
    hydromapload::loadMap( filename,
                           negate,
                           numCellsX,
                           numCellsY,
                           cells );
    
    ogMap.reallocate( numCellsX, numCellsY );
    memcpy( ogMap.data(), &(cells[0]), numCellsX*numCellsY*sizeof(unsigned char) );
    
    ogMap.offset().p.x = offsetX;
    ogMap.offset().p.y = offsetY;
    ogMap.offset().o   = offsetTheta;

    ogMap.setMetresPerCell( metresPerCell );
}

void writeToQImage( const hydroogmap::OgMap &ogMap,
                    QImage &qImage,
                    bool  negate )
{
    qImage = QImage( ogMap.numCellsX(), ogMap.numCellsY(), QImage::Format_RGB32 );

//    QImage qImage( ogMap.numCellsX(), ogMap.numCellsY(), QImage::Format_RGB32 );

    // Read in the image
    for ( int j = 0; j < qImage.height(); j++ )
    {
        for ( int i = 0; i < qImage.width(); i++ )
        { 
            const int xi = i;
            const int yi = ogMap.numCellsY()-1-j;
            const int index = ogMap.numCellsX()*(yi) + xi;
            unsigned char val = ogMap.data()[index];
            if ( !negate )
                val = 254 - val;

            QColor pixColor( val, val, val );
            qImage.setPixel( i, j, pixColor.rgb() );
        }
    }
}


void saveMap( const std::string       &filename,
              const hydroogmap::OgMap &ogMap,
              bool  negate )
{
    QImage qImage;
    writeToQImage( ogMap, qImage, negate );
    qImage.save( QString(filename.c_str()) );    
}

}

