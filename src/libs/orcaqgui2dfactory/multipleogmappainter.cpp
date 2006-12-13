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
    : isDisplayMap_(true),
      haveMap_(false)
{
    //cout << "TRACE(ogmappainter.cpp): init " << endl;

    winMaxSize_.setWidth( winMaxWidth );
    winMaxSize_.setHeight( winMaxHeight );

    winSize_.setWidth( 0 );
    winSize_.setHeight( 0 );

    // init member variables
    cellSize_.setHeight(0.0);
    cellSize_.setWidth(0.0);

    mapSizePix_.setHeight(0);
    mapSizePix_.setWidth(0);
}


MultipleOgMapPainter::~MultipleOgMapPainter()
{
}

void
MultipleOgMapPainter::clear()
{
    reset();
}

void
MultipleOgMapPainter::setData( const OgMapDataPtr & data0, const OgMapDataPtr & data1 )
{
//     cout<<"TRACE(ogmappainter.cpp): setData(): " << endl;
//     cout << orcaice::toVerboseString(data);
//     data_ = data;

    if ( data0->origin.o != 0.0 ) {
        cout << "ERROR(ogmappainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
        return;
    }

    // [m]
    cellSize_.setWidth( data0->metresPerCellX );
    cellSize_.setHeight( data0->metresPerCellY );
    // [cells]
    mapSizePix_.setWidth( data0->numCellsX );
    mapSizePix_.setHeight( data0->numCellsY );
    // [m]
    mapSizeM_.setWidth( data0->numCellsX * cellSize_.width() );
    mapSizeM_.setHeight( data0->numCellsY * cellSize_.height() );

    // map origin [cells]
    originX_ = (int)floor(data0->origin.p.x/data0->metresPerCellX);
    originY_ = (int)floor(data0->origin.p.y/data0->metresPerCellY);
    // map origin [m]
    origin_.setX( data0->origin.p.x );
    origin_.setY( data0->origin.p.y );
    
    cout<<"TRACE(ogmappainter.cpp): Painting full-size pixmap" << endl;
    assert( (int)data0->data.size() == data0->numCellsX*data0->numCellsY );
    
    QPainter p;
    unsigned char occ0;
    unsigned char occ1;
    int c0;
    int c1;
    
    qMap_ = QPixmap( data0->numCellsX, data0->numCellsY );
    p.begin( &qMap_ );
    
    for( int x=0; x<data0->numCellsX; ++x )
    {
        for ( int y=0; y<data0->numCellsY; ++y )
        {
            occ0 = orcaice::gridCell( data0, x, y );
            occ1 = orcaice::gridCell( data1, x, y );
            c0 = 255 - (int)occ0;
            c1 = 255 - (int)occ1;
            p.setPen( QColor( c0,c1,1 ) );
            p.drawPoint( x, y );
        }
    }
    p.end();

    haveMap_ = true;
    rescale();
}

// Reallocates pixmaps for scaled and unscaled map storage.
void
MultipleOgMapPainter::reset()
{
    return;
    
    cout<<"TRACE(ogmappainter.cpp): reset()" << endl;

    map2win_.reset();

    // set size of the OG original
    qMap_ = QPixmap( mapSizePix_ );
    qMap_.fill( Qt::white );
    cout<<"QredOgMap::reset: allocated unscaled map"<<mapSizePix_.width()<<"x"<<mapSizePix_.height()<<endl;

    // set the size of the window buffer to the maximum allowed size of the window
    // to minimize resizing. (Is this the best approach?)
    mapWin_ = QPixmap( winMaxSize_ );
    mapWin_.fill( Qt::white );
    cout<<"QredOgMap::reset: allocated scaled (win size) map "<<winMaxSize_.width()<<"x"<<winMaxSize_.height()<<endl;
}

void
MultipleOgMapPainter::paint( QPainter *painter, int z )
{
    if ( !haveMap_ ) return;
    
    if ( z!=Z_OG_MAP || !isDisplayMap_ ) return;

    // check that the window size has not changed
    bool dirty = false;
    dirty = dirty || updateWindowSize( QSize(painter->device()->width(), painter->device()->height()) );
    dirty = dirty || updateWorldMatrix( painter->matrix() );
    
    if ( dirty ) {
        rescale();
    }
     
    painter->save();
    painter->setMatrix( QMatrix() );
    painter->drawPixmap( QPoint(), mapWin_ );
    painter->restore();
    
//     QPixmap test(500,500);
//     test.fill(QColor(255,0,0,127));
//     painter->drawPixmap( QPoint(), test );
}

bool
MultipleOgMapPainter::updateWorldMatrix( const QMatrix & m )
{
    // don't waste time resizing to the same scale
    if ( m2win_ == m ) {
        return false;
    }
//     cout << "TRACE(ogmappainter.cpp): updateWorldMatrix " << endl;

    m2win_ = m;

    // copy "m2win" matrix ...
    map2win_ = m2win_;
    QMatrix M = map2win_;
    //cout<<"m2win_: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    // and translate to the origin of the map [m]
    // remove scale and translation
    double s = sqrt( map2win_.m11()*map2win_.m11() + map2win_.m12()*map2win_.m12() );
    QMatrix rot( map2win_.m11()/s, map2win_.m12()/s, -map2win_.m21()/s, -map2win_.m22()/s, 0.0, 0.0);
    M = rot;
    //cout<<"rotation only: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;
    
    QPointF origin = rot.map( origin_ );
    //cout<<"origin in meters: "<<origin_.x()<<","<<origin_.y()<<" rotated :"<<origin.x()<<","<<origin.y()<<endl;
    
    map2win_.translate( origin.x(), origin.y() );
    M = map2win_;
    //cout<<"translated: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    // and scale it by cell size in [m]
    // this matrix is [pix-2-pix]
    map2win_.scale( cellSize_.width(), cellSize_.height() );
    M = map2win_;
    //cout<<"scaled: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;
    return true;
}

bool
MultipleOgMapPainter::updateWindowSize( const QSize & s )
{
    // don't waste time resizing to the same size
    if ( winSize_ == s ) {
        return false;
    }
    cout << "TRACE(ogmappainter.cpp): updateWindowSize " << endl;

    winSize_ = s;
    
    mapWin_ = QPixmap( winSize_ );
    //cout<<"QredOgMap::updateWindowSize: new size ["<<winSize_.width()<<","<<winSize_.height()<<"], so rescaled map buffer to ["<<mapWin_.width()<<"x"<<mapWin_.height()<<"]"<<endl;

    return true;
}

/*
    (Slooowly) transforms the unscaled OG map to current window view. Assumes that the window
    map buffer is sized correctly.
*/
void MultipleOgMapPainter::rescale()
{
    //cout<<"TRACE(ogmappainter.cpp): ----------- rescale() -----------" << endl;
    if ( !haveMap_ ) return;

    // peep hole into window display [pix] of the map [pix]
    QMatrix win2map = map2win_.inverted();
    // this is in [pix]
    QRect peepRectPix = win2map.mapRect( QRect( QPoint(), winSize_) );

    //cout<<"peep hole [pix] ["<<peepRectPix.width()<<","<<peepRectPix.height()<<"] at ("<<peepRectPix.left()<<","<<peepRectPix.top()<<")"<<endl;

    // make sure that the peep rectangle is not outside the map (insideRect is also in map CS)
    QRect insideRectPix = peepRectPix.intersect( QRect( QPoint(), mapSizePix_ ) );
    //cout<<"inside hole ["<<insideRectPix.width()<<","<<insideRectPix.height()<<"] at ("<<insideRectPix.left()<<","<<insideRectPix.top()<<")"<<endl;

    // copy unscaled peep hole into a separate pixmap
    QPixmap unscaledInsideRect( insideRectPix.size() );
    QPainter toUnscaledInsideRect( &unscaledInsideRect );
    toUnscaledInsideRect.drawPixmap( QPoint(), qMap_, insideRectPix );

    // scale to the window buffer [pix] (no translation)
    QPixmap scaledInsideRect = unscaledInsideRect.transformed( map2win_ );
 
    // Find the win coords of the top-left point of insideRect
    QPoint topCorner = map2win_.map( insideRectPix.bottomLeft() );

//     cout<<"TRACE(ogmappainter.cpp): topCorner: " << topCorner.x() << "," << topCorner.y() << endl;

    mapWin_.fill( Qt::gray );

//     cout<<"TRACE(ogmappainter.cpp): copying from "<<scaledInsideRect.width()<<"x"<<scaledInsideRect.height()
//         <<" to "<<mapWin_.width()<<"x"<<mapWin_.height()<<endl;
        
    // copy into window buffer
    QPainter toMapWin( &mapWin_ );
    toMapWin.drawPixmap( topCorner, scaledInsideRect );
}

void
MultipleOgMapPainter::toggleDisplayMap()
{
    isDisplayMap_ = !isDisplayMap_;
}
