/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QPainter>
#include <orcaice/orcaice.h>

#include "ogmapscombinedpainter.h"

using namespace orcaqgui2d;
using namespace std;

OgMapsCombinedPainter::OgMapsCombinedPainter( const orcaice::Context &context )
{       
    QString lookupTableFile = QString(CMAKE_INSTALL_PREFIX) + "/share/orcaqgui2dfactory/ogHazardLookupTable.png";

    lookupTable_ = new QImage( lookupTableFile );

    if ( lookupTable_->isNull() )
    {
        stringstream ss;
        ss << "OgMapsCombinedPainter: Lookup table file not found.  Tried to find: " << lookupTableFile.toStdString();
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
    pixmapPainter_.reset( new PixmapPainter() );
}


OgMapsCombinedPainter::~OgMapsCombinedPainter()
{
}

void 
OgMapsCombinedPainter::paint( QPainter *p, int z )
{
    if (z!=Z_OG_MAP) return;
    pixmapPainter_->paint(p); 
}

void
OgMapsCombinedPainter::setData( const orca::OgMapData& data0, const orca::OgMapData& data1 )
{
    // both maps currently need to have the same dimensions
    assert( data0.offset == data1.offset );
    assert( data0.numCellsX == data1.numCellsX );
    assert( data0.numCellsY == data1.numCellsY );
    assert( data0.metresPerCellX == data1.metresPerCellX );
    assert( data0.metresPerCellY == data1.metresPerCellY );
    assert( (data0.mapType==orca::OgMapOccupancy || data0.mapType==orca::OgMapHazard)
             && (data1.mapType==orca::OgMapOccupancy  || data1.mapType==orca::OgMapHazard) );
    
    if ( data0.offset.o != 0.0 ) {
        cout << "ERROR(ogmapscombinedpainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
        return;
    }
    
    const orca::OgMapData *occMap;
    const orca::OgMapData *hazMap;
    if (data0.mapType==orca::OgMapOccupancy)
    {
        occMap = &data0;
        hazMap = &data1;
    }
    else
    {
        occMap = &data1;
        hazMap = &data0;
    }
    // if both maps are of type occMap then it doesn't matter which one is occMap/hazMap    
    
    PixmapData pixmapData;
    pixmapData.cellSize = QSizeF(occMap->metresPerCellX,occMap->metresPerCellY);
    pixmapData.mapSizePix = QSize(occMap->numCellsX,occMap->numCellsY);
    pixmapData.offset = QPointF(occMap->offset.p.x,occMap->offset.p.y);
    
    QRgb rgb;
    
    for (int i=0; i<occMap->numCellsX*occMap->numCellsY; i++)
    {
        rgb = lookupTable_->pixel(QPoint((int)hazMap->data[i],(int)occMap->data[i]));
        pixmapData.rgbR.push_back( qRed(rgb) );
        pixmapData.rgbG.push_back( qGreen(rgb) );
        pixmapData.rgbB.push_back( qBlue(rgb) );
    }
    
    pixmapPainter_->setData( pixmapData );
}



