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

#include "ogmappainter.h"
#include <orcaqgui2d/paintutils.h>

using namespace orca;
using namespace orcaice;
using namespace orcaqgui;
using namespace std;


OgMapPainter::OgMapPainter( QObject* parent, int winMaxWidth, int winMaxHeight )
    : isDisplayMap_(true),
      parent_(parent),
      haveMap_(false)
{
    //cout << "TRACE(ogmappainter.cpp): init " << endl;

    winMaxSize_.setWidth( winMaxWidth );
    winMaxSize_.setHeight( winMaxHeight );
    // ???
    winSize_.setWidth( 0 );
    winSize_.setHeight( 0 );

    // init member variables
    cellSize_.setHeight(0.0);
    cellSize_.setWidth(0.0);

    mapSizePix_.setHeight(0);
    mapSizePix_.setWidth(0);

    // connect the scaling/sizing signal coming from the component container
//     QObject::connect( parent_, SIGNAL(worldMatrixChanged(const QMatrix*)), this,SLOT(updateWorldMatrix(const QMatrix*)) );
//     QObject::connect( parent_, SIGNAL(windowSizeChanged(const QSize &)), this,SLOT(updateWindowSize(const QSize &)) );
}


OgMapPainter::~OgMapPainter()
{
}

void
OgMapPainter::clear()
{
    reset();
}

void
OgMapPainter::setData( const OgMapDataPtr & data )
{
    cout<<"TRACE(ogmappainter.cpp): setData(): " << data << endl;
//        orcaice::displayAsText(data);
    data_ = data;

    if ( data->origin.o != 0.0 ) {
        cout << "ERROR(ogmappainter.cpp): Don't know how to display a non-axis-aligned map." << endl;
        return;
    }

    // [m]
    cellSize_.setWidth( data->metresPerCellX );
    cellSize_.setHeight( data->metresPerCellY );
    // [cells]
    mapSizePix_.setWidth( data->numCellsX );
    mapSizePix_.setHeight( data->numCellsY );
    // [m]
    mapSizeM_.setWidth( data->numCellsX * cellSize_.width() );
    mapSizeM_.setHeight( data->numCellsY * cellSize_.height() );

    // map origin [cells]
    originX_ = (int)floor(data->origin.p.x/data->metresPerCellX);
    originY_ = (int)floor(data->origin.p.y/data->metresPerCellY);
    // map origin [m]
    origin_.setX( data->origin.p.x );
    origin_.setY( data->origin.p.y );
    
    cout<<"TRACE(ogmappainter.cpp): Painting full-size pixmap" << endl;
    assert( (int)data->data.size() == data->numCellsX*data->numCellsY );
    
    QPainter p;
    unsigned char occ;
    int c;
    
    qMap_ = QPixmap( data->numCellsX, data->numCellsY );
    p.begin( &qMap_ );
    
    for( int x=0; x<data->numCellsX; ++x )
    {
        for ( int y=0; y<data->numCellsY; ++y )
        {
            occ = orcaice::gridCell( data, x, y );
            c = 255 - (int)occ;
            p.setPen( QColor( c,c,c ) );
            p.drawPoint( x, y );
        }
    }
    p.end();

    haveMap_ = true;
    rescale();
}

// Reallocates pixmaps for scaled and unscaled map storage.
void
OgMapPainter::reset()
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
OgMapPainter::paint( QPainter *painter, int z )
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
    
//     cout<<"TRACE(ogmappainter.cpp): painting map "<<mapWin_.width()<<"x"<<mapWin_.height()<<" pix"<< endl;

    painter->save();
    painter->setMatrix( QMatrix() );
    painter->drawPixmap( QPoint(), mapWin_ );
    painter->restore();
    
//     painter->save();
//     painter->setMatrix( QMatrix() );
//     QPixmap test( 600,500 );
//     //QPixmap test( 60,50 );
//     test.fill( Qt::green );
//     painter->drawPixmap( QPoint(), test );
//     painter->restore();
    
//     orcaqgui::paintOrigin( p, Qt::red );

//     painter->drawPixmap( QRectF(QPointF(),QSizeF(50.0,20.0)), qMap_, QRectF(QPointF(),QSizeF(500.0,200.0)) );

    // TODO: Shouldn't re-paint this every time.
//     QPen pen;
//     pen.setWidth( (int)(data_->metresPerCellX * 1000) );
//     for( int x=0; x<data_->numCellsX; x++)
//     {
//         for (int y=0; y<data_->numCellsY; y++)
//         {
//             unsigned char occ = orcaice::gridCell( data_, x, y );
//             int c = 255 - (int)occ;
//             pen.setColor( qRgb(c,c,c) );
//             painter->setPen( pen );
//             painter->drawPoint( x, y );
//         }
//     }
// 
// 
//     paintOrigin( p, Qt::red );
}

bool
OgMapPainter::updateWorldMatrix( const QMatrix & m )
{
    // don't waste time resizing to the same scale
    if ( m2win_ == m ) {
        return false;
    }
    cout << "TRACE(ogmappainter.cpp): updateWorldMatrix " << endl;

    m2win_ = m;

    // copy "m2win" matrix ...
    map2win_ = m2win_;
    QMatrix M = map2win_;
    cout<<"m2win_: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    // and translate to the origin of the map [m]
    // remove scale and translation
    double s = sqrt( map2win_.m11()*map2win_.m11() + map2win_.m12()*map2win_.m12() );
    QMatrix rot( map2win_.m11()/s, map2win_.m12()/s, -map2win_.m21()/s, -map2win_.m22()/s, 0.0, 0.0);
    M = rot;
    cout<<"rotation only: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;
    
    QPointF origin = rot.map( origin_ );
    cout<<"origin in meters: "<<origin_.x()<<","<<origin_.y()<<" rotated :"<<origin.x()<<","<<origin.y()<<endl;
    
    map2win_.translate( origin.x(), origin.y() );
//     map2win_.translate( origin_.x(), origin_.y() );
    M = map2win_;
    cout<<"translated: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    // and scale it by cell size in [m]
    // this matrix is [pix-2-pix]
    map2win_.scale( cellSize_.width(), cellSize_.height() );
    M = map2win_;
    cout<<"scaled: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    // and translate to the origin of the map
//     map2win_.setMatrix( map2win_.m11(),
//                         map2win_.m12(),
//                         map2win_.m21(),
//                         map2win_.m22(),
//                         map2win_.dx()+originX_*map2win_.m11() ,
//                         map2win_.dy()+originY_*map2win_.m22() );
//     M = map2win_;
//     cout<<"translated: " << M.m11()<<","<<M.m12()<<","<<M.m21()<<","<<M.m22()<<", "<<M.dx()<<","<<M.dy()<<endl;

    return true;
}

bool
OgMapPainter::updateWindowSize( const QSize & s )
{
    // don't waste time resizing to the same size
    if ( winSize_ == s ) {
        return false;
    }
    cout << "TRACE(ogmappainter.cpp): updateWindowSize " << endl;

    winSize_ = s;

//     if ( winSize_.width()>winMaxSize_.width() || winSize_.height()>winMaxSize_.height() )
//     {
//         // ???
//         winMaxSize_ = winSize_;
//         cout<<"QredOgMap::updateWindowSize: current window size is larger than expected max size. Resizing map buffer..."<<endl;
//     }
    
    mapWin_ = QPixmap( winSize_ );
    cout<<"QredOgMap::updateWindowSize: new size ["<<winSize_.width()<<","<<winSize_.height()<<"], so rescaled map buffer to ["<<mapWin_.width()<<"x"<<mapWin_.height()<<"]"<<endl;

    return true;
}

/*
    (Slooowly) transforms the unscaled OG map to current window view. Assumes that the window
    map buffer is sized correctly.
*/
void OgMapPainter::rescale()
{
    cout<<"TRACE(ogmappainter.cpp): ----------- rescale() -----------" << endl;
    if ( !haveMap_ ) return;

    // peep hole into window display [pix] of the map [pix]
    QMatrix win2map = map2win_.inverted();
    // this is in [pix]
    QRect peepRectPix = win2map.mapRect( QRect( QPoint(), winSize_) );

    cout<<"peep hole [pix] ["<<peepRectPix.width()<<","<<peepRectPix.height()<<"] at ("<<peepRectPix.left()<<","<<peepRectPix.top()<<")"<<endl;

    // make sure that the peep rectangle is not outside the map (insideRect is also in map CS)
    QRect insideRectPix = peepRectPix.intersect( QRect( QPoint(), mapSizePix_ ) );
    cout<<"inside hole ["<<insideRectPix.width()<<","<<insideRectPix.height()<<"] at ("<<insideRectPix.left()<<","<<insideRectPix.top()<<")"<<endl;

    // copy unscaled peep hole into a separate pixmap
    QPixmap unscaledInsideRect( insideRectPix.size() );
    QPainter toUnscaledInsideRect( &unscaledInsideRect );
    toUnscaledInsideRect.drawPixmap( QPoint(), qMap_, insideRectPix );

    // scale to the window buffer [pix] (no translation)
//     QMatrix mapPix2winPix( map2win_.m11(),map2win_.m12(), map2win_.m21(),map2win_.m22(), 0.0,0.0 );
//     QPixmap scaledInsideRect = unscaledInsideRect.transformed( mapPix2winPix );
    QPixmap scaledInsideRect = unscaledInsideRect.transformed( map2win_ );

//     cout<<"TRACE(ogmappainter.cpp): m11,m22: " << map2win_.m11() << "," << map2win_.m22() << endl;
// 
    // Find the win coords of the top-left point of insideRect
    QPoint topCorner = map2win_.map( insideRectPix.bottomLeft() );

//     cout<<"TRACE(ogmappainter.cpp): topCorner: " << topCorner.x() << "," << topCorner.y() << endl;

    mapWin_.fill( Qt::gray );

    cout<<"TRACE(ogmappainter.cpp): copying from "<<scaledInsideRect.width()<<"x"<<scaledInsideRect.height()
        <<" to "<<mapWin_.width()<<"x"<<mapWin_.height()<<endl;
        
    // copy into window buffer
    QPainter toMapWin( &mapWin_ );
    toMapWin.drawPixmap( topCorner, scaledInsideRect );
}

void
OgMapPainter::toggleDisplayMap()
{
        isDisplayMap_ = !isDisplayMap_;
}


ImageFileType
OgMapPainter::checkFileExtension( QString &fe, IHumanManager *humanManager )
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
    else if (fe=="bin")
    {
        return ICE_STREAM;
    }
    else
    {
        cout << "ERROR(ogmappainter.cpp): File extension not supported" << endl;
        humanManager->showBoxMsg(Error, "File extension not supported" );
        return NOT_SUPPORTED;
    }
}

int 
OgMapPainter::saveMap( const orcaice::Context context, const QString fileName, IHumanManager *humanManager )
{
    QString fileExtension = fileName.section('.',-1,-1);
    ImageFileType type = checkFileExtension( fileExtension, humanManager );
    
    if ( type == NOT_SUPPORTED )
    {
        return -1;
    }
    else if ( type == BITMAP )
    {
        // We have to mirror the map first
        QMatrix matrix(1.0, 0.0, 0.0, -1.0, 0.0, 0.0);
    
        int ret = qMap_.transformed( matrix ).save( fileName, fileExtension.toLatin1() );
        if (!ret)
        {
            cout << "ERROR(ogmappainter.cpp): Problems saving file " << fileName.toStdString() << endl; 
            humanManager->showBoxMsg(Error, "Problems saving file " + fileName);
            return -1;
        }
        cout << "TRACE(ogmappainter.cpp): Successfully saved qMap to file: " << fileName.toStdString() << endl;      
        humanManager->showStatusMsg(Information, "Successfully saved ogMap to file: " + fileName);
    }
    else if ( type == ICE_STREAM )
    {
        // create data file
        std::ofstream *dataFile = new ofstream( fileName.toStdString().c_str(),ios::binary );
        if ( !dataFile->is_open() ) 
        {
            cout << "ERROR(ogmappainter.cpp): Could not create data file " << fileName.toStdString() << endl;
            humanManager->showBoxMsg(Error, "Could not create ICE_STREAM file " + fileName); 
            return -1;
        }
                
        // create stream
        vector<Ice::Byte> byteData;
        Ice::OutputStreamPtr outStreamPtr = Ice::createOutputStream( context.communicator() );
        ice_writeOgMapData(outStreamPtr, data_);
        outStreamPtr->writePendingObjects();
        outStreamPtr->finished(byteData);
                
        // write stream to binary file
        size_t length = byteData.size();
        dataFile->write( (char*)&length, sizeof(size_t) );
        dataFile->flush();
        dataFile->write( (char*)&byteData[0], length);
        dataFile->flush();
        dataFile->close();
        delete dataFile;
        cout << "INFO(ogmappainter.cpp): Successfully saved map to file " << fileName.toStdString() << endl;
        humanManager->showStatusMsg(Information, "Successfully saved ogMap to file: " + fileName);
    }
    
    return 0;
}
