/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <QPainter>
#include <QString>
#include <QPixmap>

#include <fstream>
#include <orcaobj/orcaobj.h>

#include "multipleogmappainter.h"
#include <orcaqgui2d/paintutils.h>

using namespace orca;
using namespace orcaice;
using namespace orcaqgui;
using namespace std;


MultipleOgMapPainter::MultipleOgMapPainter( int winMaxWidth, int winMaxHeight )
{
    pixmapPainter_ = new PixmapPainter( winMaxWidth, winMaxHeight );
}


MultipleOgMapPainter::~MultipleOgMapPainter()
{
}

void
MultipleOgMapPainter::setData( const OgMapDataPtr & data0, const OgMapDataPtr & data1 )
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
    
    cout<<"TRACE(ogmappainter.cpp): Painting full-size pixmap" << endl;
    assert( (int)data0->data.size() == data0->numCellsX*data0->numCellsY );
}

