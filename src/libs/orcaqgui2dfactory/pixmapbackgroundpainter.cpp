/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <QPainter>
#include <orcaice/orcaice.h>
#include <orcaqgui/exceptions.h>
#include <orcaqgui/ihumanmanager.h>

#include "pixmapbackgroundpainter.h"

using namespace orcaqgui;
using namespace orcaqgui2d;
using namespace std;


PixMapBackgroundPainter::PixMapBackgroundPainter()
{    
    pixmapPainter_ = new PixmapPainter();
}


PixMapBackgroundPainter::~PixMapBackgroundPainter()
{
    delete pixmapPainter_;
}

void 
PixMapBackgroundPainter::paint( QPainter *p, int z )
{
    if (z!=Z_BACKGROUND) return;
    pixmapPainter_->paint(p); 
}

void
PixMapBackgroundPainter::setData( const orca::PixMapData& data )
{
    if ( data.offset.o != 0.0 ) 
    {
        stringstream ss;
        ss << "PixMapBackgroundPainter: Don't know how to display non-axis-aligned map: "
           << orcaice::toString( data );
        throw Exception( ss.str() );
    }
    
    // assemble information to give to pixmapPainter
    PixmapData pixmapData;
    pixmapData.cellSize = QSizeF(data.metresPerCellX,data.metresPerCellY);
    pixmapData.mapSizePix = QSize(data.numCellsX,data.numCellsY);
    pixmapData.offset = QPointF(data.offset.p.x,data.offset.p.y);
    
    for (int i=0; i<(data.numCellsX*data.numCellsY); i++)
    {
        const orca::Pixel &p = data.rgbPixels[i];
        pixmapData.rgbR.push_back(p.r);
        pixmapData.rgbG.push_back(p.g);
        pixmapData.rgbB.push_back(p.b);
    }
    
    pixmapPainter_->setData( pixmapData );
}

bool
PixMapBackgroundPainter::checkFileExtension( QString &fe, orcaqgui::IHumanManager *humanManager )
{
    if ( fe.isEmpty() ) 
    {
        //default
        fe="png";
        return 0;
    }
    else if (fe=="png" || fe=="bmp" || fe=="jpg" || fe=="jpeg" || fe=="ppm" || fe=="xbm" || fe=="xpm")
    {
        return 0;    
    }
    else
    {
        cout << "ERROR(pixmapbackgroundpainter.cpp): File extension not supported" << endl;
        humanManager->showBoxMsg(Error, "File extension not supported" );
        return -1;
    }
}

int 
PixMapBackgroundPainter::saveMap( const orcaice::Context & context, const QString fileName, orcaqgui::IHumanManager *humanManager )
{
    QString fileExtension = fileName.section('.',-1,-1);
    bool isOk = checkFileExtension( fileExtension, humanManager );
    
    if ( isOk )
    {
        pixmapPainter_->saveMap( fileName, fileExtension, humanManager );
        return 0;
    }    
    return -1;
}
