/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <QPainter>

#include "pixmappainter.h"

using namespace std;

namespace orcaqgui2d {

PixmapPainter::PixmapPainter()
    : isDisplayMap_(true),
      haveMap_(false)
{
}


PixmapPainter::PixmapPainter( PixmapData &pixmapData )
    : data_(pixmapData),
      isDisplayMap_(true),
      haveMap_(false),
      previousOffset_(0.0,0.0)
{
}        


PixmapPainter::~PixmapPainter()
{
}

void
PixmapPainter::setData( PixmapData &pixmapData )
{
    assert( (int)pixmapData.rgbR.size() == pixmapData.mapSizePix.width()*pixmapData.mapSizePix.height() );
    assert( (int)pixmapData.rgbG.size() == pixmapData.mapSizePix.width()*pixmapData.mapSizePix.height() );
    assert( (int)pixmapData.rgbB.size() == pixmapData.mapSizePix.width()*pixmapData.mapSizePix.height() );
    
    data_ = pixmapData;
    
    QPainter p;
    int cR, cB, cG;
    
    qMap_ = QPixmap( data_.mapSizePix.width(), data_.mapSizePix.height() );
    p.begin( &qMap_ );
    
    for( int x=0; x<data_.mapSizePix.width(); ++x )
    {
        for ( int y=0; y<data_.mapSizePix.height(); ++y )
        {
            cR = data_.rgbR[y*data_.mapSizePix.width() + x];
            cG = data_.rgbG[y*data_.mapSizePix.width() + x];
            cB = data_.rgbB[y*data_.mapSizePix.width() + x];
            p.setPen( QColor( cR, cG, cB ) );
            p.drawPoint( x, y );
        }
    }
    p.end();

    haveMap_ = true;
    rescale();
}

void
PixmapPainter::paint( QPainter *painter )
{
    if ( !haveMap_ || !isDisplayMap_ ) return;

    // check that the window size has not changed
    bool dirty = false;
    dirty = dirty || updateWindowSize( QSize(painter->device()->width(), painter->device()->height()) );
    dirty = dirty || updateWorldMatrix( painter->worldMatrix() );
    
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
    // don't waste time resizing to the same scale and if the offset 
    // of the pixmap hasn't changed
    if ( m2win_ == m && previousOffset_==data_.offset ) {
        return false;
    }
    // cout << "TRACE(pixmappainter.cpp): updateWorldMatrix " << endl;

    m2win_ = m;
    
    // record the offset to check next time if it has changed
    previousOffset_ = data_.offset;

    // copy "m2win" matrix ...
    map2win_ = m2win_;
    QMatrix M = map2win_;
    //cout<<"m2win_: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    // and translate to the offset of the map [m]
    // remove scale and translation
    double s = sqrt( map2win_.m11()*map2win_.m11() + map2win_.m12()*map2win_.m12() );
    QMatrix rot( map2win_.m11()/s, map2win_.m12()/s, -map2win_.m21()/s, -map2win_.m22()/s, 0.0, 0.0);
    M = rot;
    //cout<<"rotation only: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;
    
    QPointF offset = rot.map( data_.offset );
    //cout<<"offset in meters: "<<data_.offset.x()<<","<<data_.offset.y()<<" rotated :"<<offset.x()<<","<<offset.y()<<endl;
    
    map2win_.translate( offset.x(), offset.y() );
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
//     cout << "TRACE(pixmappainter.cpp): updateWindowSize " << endl;

    winSize_ = s;
    
    mapWin_ = QPixmap( winSize_ );
    //cout<<"TRACE(pixmappainter.cpp):updateWindowSize: new size ["<<winSize_.width()<<","<<winSize_.height()<<"], so rescaled map buffer to ["<<mapWin_.width()<<"x"<<mapWin_.height()<<"]"<<endl;

    return true;
}

/*
    (Slooowly) transforms the unscaled map to current window view. Assumes that the window
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
PixmapPainter::saveMap( const QString &fileName,
                        const QString &fileExtension,
                        hydroqguielementutil::IHumanManager *humanManager )
{   
    // We have to mirror the map first
    QMatrix matrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);

    int ret = qMap_.transformed( matrix ).save( fileName, fileExtension.toLatin1() );
    if (!ret)
    {
        humanManager->showDialogError( "Problems saving pixmap to file " + fileName );
        return -1;
    }
    humanManager->showStatusMsg(hydroqguielementutil::IHumanManager::Information, "Successfully saved pixmap to file: " + fileName);
    
    return 0;
}

}
