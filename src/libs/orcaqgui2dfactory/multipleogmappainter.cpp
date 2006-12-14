/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <QPainter>

#include "multipleogmappainter.h"

using namespace orcaqgui;
using namespace std;


MultipleOgMapPainter::MultipleOgMapPainter()
{
    pixmapPainter_ = new PixmapPainter();
}


MultipleOgMapPainter::~MultipleOgMapPainter()
{
}

void 
MultipleOgMapPainter::paint( QPainter *p, int z )
{
    if (z!=Z_OG_MAP) return;
    pixmapPainter_->paint(p); 
}

void
MultipleOgMapPainter::setData( const orca::OgMapDataPtr & data0, const orca::OgMapDataPtr & data1 )
{

//     cout << orcaice::toVerboseString(data);

    if ( data0->origin.o != 0.0 ) {
        cout << "ERROR(ogmappainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
        return;
    }
    
    // assemble information to give to pixmapPainter
    PixmapData pixmapData;
    pixmapData.cellSize = QSizeF(data0->metresPerCellX,data0->metresPerCellY);
    pixmapData.mapSizePix = QSize(data0->numCellsX,data0->numCellsY);
    pixmapData.origin = QPointF(data0->origin.p.x,data0->origin.p.y);
    pixmapData.rgbR = data0->data;
    pixmapData.rgbG = data1->data;
    pixmapData.rgbB = data1->data;
    
    pixmapPainter_->setData( pixmapData );
}

