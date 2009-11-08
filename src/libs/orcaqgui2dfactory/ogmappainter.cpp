/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <fstream>
#include <QPainter>

#include <Ice/Ice.h>
#include <orcaice/orcaice.h>
#include <orcaobj/ogmap.h>
#include "ogmappainter.h"
#include <hydroqgui/exceptions.h>
#include <hydroqguielementutil/ihumanmanager.h>
using namespace std;

namespace orcaqgui2d {

void
OgMapPainter::paint( QPainter *p, int z )
{
    if (z!=hydroqguielementutil::Z_OG_MAP) return;
    pixmapPainter_.paint(p);
}

void
OgMapPainter::setData( const orca::OgMapData& data )
{
//     cout << orcaobj::toVerboseString(data);

    if ( data.offset.o != 0.0 )
    {
        stringstream ss;
        ss << "OgMapPainter: Don't know how to display non-axis-aligned map: " << orcaobj::toString( data );
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }

    // remember our map type
    mapType_ = data.mapType;

    // assemble information to give to pixmapPainter
    pixmapData_.cellSize = data.metresPerCell;
    pixmapData_.mapSizePix = QSize(data.numCellsX,data.numCellsY);
    pixmapData_.offset = QPointF(data.offset.p.x,data.offset.p.y);
    const int numCells = data.numCellsX*data.numCellsY;
    pixmapData_.rgbR.resize( numCells );
    pixmapData_.rgbG.resize( numCells );
    pixmapData_.rgbB.resize( numCells );

    switch ( mapType_ )
    {
        case orca::OgMapHazard : {
            for (int i=0; i<numCells; i++) {
                // unoccupied: yellow, occupied: red
                pixmapData_.rgbR[i] = 255;
                pixmapData_.rgbG[i] = 255-data.data[i];
                pixmapData_.rgbB[i] = 0;
            }
            break;
        }
        case orca::OgMapOccupancy : {
            for (int i=0; i<numCells; i++) {
                // unoccupied: white, occupied: black
                pixmapData_.rgbR[i] = 255-data.data[i];
                pixmapData_.rgbG[i] = 255-data.data[i];
                pixmapData_.rgbB[i] = 255-data.data[i];
            }
            break;
        }
        default :
            assert( !"unhandled OG map type" );
    }

    pixmapPainter_.setData( pixmapData_ );
}

ImageFileType
OgMapPainter::checkFileExtension( QString &fe, hydroqguielementutil::IHumanManager *humanManager )
{
    if ( fe.isEmpty() )
    {
        fe="png";
        return BITMAP;
    }
    else if (fe=="png" || fe=="bmp" || fe=="jpg" || fe=="jpeg" || fe=="ppm" || fe=="xbm" || fe=="xpm")
    {
        return BITMAP;
    }
    else
    {
        cout << "ERROR(ogmappainter.cpp): File extension not supported" << endl;
        humanManager->showDialogError( "File extension not supported" );
        return NOT_SUPPORTED;
    }
}

int
OgMapPainter::saveMap( const orcaice::Context   &context,
                       const QString            &fileName,
                       hydroqguielementutil::IHumanManager *humanManager )
{
    QString fileExtension = fileName.section('.',-1,-1);
    ImageFileType type = checkFileExtension( fileExtension, humanManager );

    if ( type == BITMAP )
    {
        pixmapPainter_.saveMap( fileName, fileExtension, humanManager );
    }
    else
    {
        return -1;
    }

    return 0;
}

void
OgMapPainter::drawCircle ( const QPoint& center, qreal diameter )
{
    QColor color;
    switch ( mapType_ )
    {
        case orca::OgMapHazard :
            color = Qt::red;
            break;
        case orca::OgMapOccupancy :
            color = Qt::black;
            break;
        default :
            assert( !"unhandled OG map type" );
    }
    pixmapPainter_.drawCircle( color, center, diameter);
}

}
