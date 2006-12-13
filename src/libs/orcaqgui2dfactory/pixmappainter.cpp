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
#include <orcaqgui/ihumanmanager.h>

#include "pixmappainter.h"
#include <orcaqgui2d/paintutils.h>

using namespace orca;
using namespace orcaice;
using namespace orcaqgui;
using namespace std;


PixmapPainter::PixmapPainter( int winMaxWidth, int winMaxHeight )
    : isDisplayMap_(true),
      haveMap_(false)
{
}


PixmapPainter::~PixmapPainter()
{
}

void
PixmapPainter::setData( PixmapData &pixmapData )
{
    data_ = pixmapData;
    
    QPainter p;
    int cR, cB, cG;
    
    qMap_ = QPixmap( data_.mapSizePix.width(), data_.mapSizePix.height() );
    p.begin( &qMap_ );
    
    for( int x=0; x<data_.mapSizePix.width(); ++x )
    {
        for ( int y=0; y<data_.mapSizePix.height(); ++y )
        {
            cR = 255-(int)data_.rgbR[y*data_.mapSizePix.width() + x];
            cG = 255-(int)data_.rgbG[y*data_.mapSizePix.width() + x];
            cB = 255-(int)data_.rgbB[y*data_.mapSizePix.width() + x];
            p.setPen( QColor( cR, cG, cB ) );
            p.drawPoint( x, y );
        }
    }
    p.end();

    haveMap_ = true;
    rescale();
}

void
PixmapPainter::clear()
{
    reset();
}

// Reallocates pixmaps for scaled and unscaled map storage.
void
PixmapPainter::reset()
{
    return;
    
    cout<<"TRACE(pixmappainter.cpp): reset()" << endl;

    map2win_.reset();

    // set size of the OG original
    qMap_ = QPixmap( data_.mapSizePix );
    qMap_.fill( Qt::white );
    cout<<"QredOgMap::reset: allocated unscaled map"<<data_.mapSizePix.width()<<"x"<<data_.mapSizePix.height()<<endl;

    // set the size of the window buffer to the maximum allowed size of the window
    // to minimize resizing. (Is this the best approach?)
    mapWin_ = QPixmap( winMaxSize_ );
    mapWin_.fill( Qt::white );
    cout<<"QredOgMap::reset: allocated scaled (win size) map "<<winMaxSize_.width()<<"x"<<winMaxSize_.height()<<endl;
}

void
PixmapPainter::paint( QPainter *painter, int z )
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
}

bool
PixmapPainter::updateWorldMatrix( const QMatrix & m )
{
    // don't waste time resizing to the same scale
    if ( m2win_ == m ) {
        return false;
    }
//     cout << "TRACE(pixmappainter.cpp): updateWorldMatrix " << endl;

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
    
    QPointF origin = rot.map( data_.origin );
    //cout<<"origin in meters: "<<data_.origin.x()<<","<<data_.origin.y()<<" rotated :"<<origin.x()<<","<<origin.y()<<endl;
    
    map2win_.translate( origin.x(), origin.y() );
    M = map2win_;
    //cout<<"translated: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    // and scale it by cell size in [m]
    // this matrix is [pix-2-pix]
    map2win_.scale( data_.cellSize.width(), data_.cellSize.height() );
    M = map2win_;
    //cout<<"scaled: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;
    return true;
}

bool
PixmapPainter::updateWindowSize( const QSize & s )
{
    // don't waste time resizing to the same size
    if ( winSize_ == s ) {
        return false;
    }
    cout << "TRACE(pixmappainter.cpp): updateWindowSize " << endl;

    winSize_ = s;
    
    mapWin_ = QPixmap( winSize_ );
    //cout<<"QredOgMap::updateWindowSize: new size ["<<winSize_.width()<<","<<winSize_.height()<<"], so rescaled map buffer to ["<<mapWin_.width()<<"x"<<mapWin_.height()<<"]"<<endl;

    return true;
}

/*
    (Slooowly) transforms the unscaled OG map to current window view. Assumes that the window
    map buffer is sized correctly.
*/
void PixmapPainter::rescale()
{
    //cout<<"TRACE(pixmappainter.cpp): ----------- rescale() -----------" << endl;
    if ( !haveMap_ ) return;

    // peep hole into window display [pix] of the map [pix]
    QMatrix win2map = map2win_.inverted();
    // this is in [pix]
    QRect peepRectPix = win2map.mapRect( QRect( QPoint(), winSize_) );

    //cout<<"peep hole [pix] ["<<peepRectPix.width()<<","<<peepRectPix.height()<<"] at ("<<peepRectPix.left()<<","<<peepRectPix.top()<<")"<<endl;

    // make sure that the peep rectangle is not outside the map (insideRect is also in map CS)
    QRect insideRectPix = peepRectPix.intersect( QRect( QPoint(), data_.mapSizePix ) );
    //cout<<"inside hole ["<<insideRectPix.width()<<","<<insideRectPix.height()<<"] at ("<<insideRectPix.left()<<","<<insideRectPix.top()<<")"<<endl;

    // copy unscaled peep hole into a separate pixmap
    QPixmap unscaledInsideRect( insideRectPix.size() );
    QPainter toUnscaledInsideRect( &unscaledInsideRect );
    toUnscaledInsideRect.drawPixmap( QPoint(), qMap_, insideRectPix );

    // scale to the window buffer [pix] (no translation)
    QPixmap scaledInsideRect = unscaledInsideRect.transformed( map2win_ );
 
    // Find the win coords of the top-left point of insideRect
    QPoint topCorner = map2win_.map( insideRectPix.bottomLeft() );

//     cout<<"TRACE(pixmappainter.cpp): topCorner: " << topCorner.x() << "," << topCorner.y() << endl;

    mapWin_.fill( Qt::gray );

//     cout<<"TRACE(pixmappainter.cpp): copying from "<<scaledInsideRect.width()<<"x"<<scaledInsideRect.height()
//         <<" to "<<mapWin_.width()<<"x"<<mapWin_.height()<<endl;
        
    // copy into window buffer
    QPainter toMapWin( &mapWin_ );
    toMapWin.drawPixmap( topCorner, scaledInsideRect );
}

void
PixmapPainter::toggleDisplayMap()
{
    isDisplayMap_ = !isDisplayMap_;
}

int 
PixmapPainter::saveMap( const QString fileName, IHumanManager *humanManager )
{
    QString fileExtension = fileName.section('.',-1,-1);
    
    // We have to mirror the map first
    QMatrix matrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);

    int ret = qMap_.transformed( matrix ).save( fileName, fileExtension.toLatin1() );
    if (!ret)
    {
        cout << "ERROR(pixmappainter.cpp): Problems saving file " << fileName.toStdString() << endl; 
        humanManager->showBoxMsg(Error, "Problems saving file " + fileName);
        return -1;
    }
    cout << "TRACE(pixmappainter.cpp): Successfully saved qMap to file: " << fileName.toStdString() << endl;      
    humanManager->showStatusMsg(Information, "Successfully saved ogMap to file: " + fileName);
    
    return 0;
}
