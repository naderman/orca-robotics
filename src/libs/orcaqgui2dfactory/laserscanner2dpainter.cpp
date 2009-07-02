/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <assert.h>
#include <QPainter>
#include <QString>
#include <QPolygon>

#include <orcaice/orcaice.h>
#include <orcaobj/bros1.h>
#include <gbxutilacfr/mathdefs.h>
#include "laserscanner2dpainter.h"
#include <hydroqgui/exceptions.h>

using namespace std;
using namespace orcaqgui2d;

LaserScanner2dPainter::LaserScanner2dPainter( QColor outlineColor,
                            float  outlineThickness,
                            float  brightReturnWidth )
    : intensitiesValid_(false),
      laserMaxRange_(0),
      isDisplayScan_(true),
      isDisplayPoints_(false),
      isDisplayWalls_(true),
      isDisplayReflectors_(true),
      isFilledPolygon_(false),
      outlineColor_(outlineColor),
      outlineThickness_(outlineThickness),
      brightReturnWidth_(brightReturnWidth),
      offsetX_(0.0),
      offsetY_(0.0),
      offsetYaw_(0.0),
      offsetPitch_(0.0),
      isUpsideDown_(false)
{
    fillColor_ = QColor(204,204,255,127);
}

void
LaserScanner2dPainter::clear()
{
    qScan_.clear();
}

void
LaserScanner2dPainter::setOffset( orca::Frame3d &offset )
{
    offsetX_   = offset.p.x;
    offsetY_   = offset.p.y;
    offsetYaw_ = offset.o.y;
    offsetPitch_ = offset.o.p;

    // for 2D display, the only thing we know how to paint
    // is a laser mounted horizontally, either right-way-up or upside-down
    if ( !( offset.o.r == 0.0 || NEAR( offset.o.r,M_PI,0.01 ) ) )
    {
        stringstream ss;
        ss << "LaserScanner2dPainter::setOffset(): Can only properly deal with (possibly-flipped) horizontal lasers.  Offset: " << orcaobj::toString(offset);
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }

    if ( NEAR( offset.o.r,M_PI,0.01 ) ) {
        isUpsideDown_ = true;
    }
    
    // Don't really know how to deal with non-zero values here...
    if ( offset.p.z != 0.0 )
    {
        stringstream ss;
        ss << "LaserScanner2dPainter::setOffset(): Cannot properly deal with non-zero z, ignoring.  Offset: " << orcaobj::toString(offset);
        cout << ss.str();
        // throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }
}

void
LaserScanner2dPainter::setData( const orca::RangeScanner2dDataPtr & data )
{
    if ( data==0 ) return;

    // Check if this thing is a laser scan.
    orca::LaserScanner2dDataPtr laserScan = orca::LaserScanner2dDataPtr::dynamicCast( data );
    if ( laserScan )
    {
        intensitiesValid_ = true;
        intensities_      = laserScan->intensities;        
    }

    ranges_        = data->ranges;
    laserMaxRange_ = data->maxRange;

    QPointF point;
    qScan_.clear();
    qReturns_.clear();
    qScan_.push_back( QPointF(0.0,0.0) );    // first point in array is origin

    // convert from polar to cartesian coord. sys.
    double bearing;
    double angleIncrement = data->fieldOfView / double(ranges_.size()-1);
    for ( unsigned int i=0; i<ranges_.size(); ++i ) {
        bearing = data->startAngle + i * angleIncrement;
        if ( isUpsideDown_ ) {
            bearing = -bearing;
        }
        
        // account for sensor pitch: only the x-coordinate becomes "compressed", y is not affected
        point.setX( ranges_[i] * cos(bearing) * cos(offsetPitch_) );
        point.setY( ranges_[i] * sin(bearing) );

        qReturns_.push_back( point );
        qScan_.push_back( point );
    }

}

void
LaserScanner2dPainter::paint( QPainter *painter, int z )
{
    if ( z != hydroqguielementutil::Z_LASER || !isDisplayScan_ ) return;
    if ( qScan_.isEmpty() ) return;
    
    // scaling
    qreal onePix = painter->worldMatrix().inverted().mapRect(QRectF(0, 0, 1, 1)).width();

    painter->save();
    
    // handle laser offset
    painter->translate(offsetX_,offsetY_);
    painter->rotate( offsetYaw_*180.0/M_PI );

    if ( outlineThickness_ == -1 ) {
        painter->setPen( QPen( outlineColor_ ) );
    }
    else {
        painter->setPen( QPen( outlineColor_, outlineThickness_ ) );
    }
    if ( isFilledPolygon_ ) {
        painter->setBrush( fillColor_ );
    }

    //
    // draw the scan itself
    //
    painter->drawConvexPolygon( qScan_ );

    // draw the walls
    if ( isDisplayWalls_ ) {

        bool isCurrWall = (ranges_[1] < laserMaxRange_ );
        bool isPrevWall = isCurrWall;
        bool isWallSegment = isCurrWall;
        int start = 1;
        int finish = 1;
        
        for ( int i=2; i<qScan_.size(); ++i ) {
            isCurrWall = ( ranges_[i-1] < laserMaxRange_
                           && fabs(ranges_[i-1]-ranges_[i-2])<0.5 );

            // look for state change or end of data
            if ( isCurrWall != isPrevWall || i==qScan_.size()-1 ) {
                if ( isWallSegment ) {
                    finish = i-1;
                    painter->setPen( QPen( outlineColor_, 3.0*onePix ) );
                    painter->drawPolyline( &qScan_[start], finish-start+1 );
                    // start of next segment
                    start = i-1;
                }
                else {
                    finish = i;
                    // start of next segment
                    start = i;
                }
                
                isWallSegment = isCurrWall;
                isPrevWall = isCurrWall;
            }
            
        }
    }

    if ( isDisplayPoints_ )
    {
        painter->setPen( QPen( Qt::black, 3.0*onePix ) );
//         painter->setPen( QPen( Qt::black ) );
        painter->drawPoints( qReturns_ );
    }

    // draw bright rectangles for high intensity returns
    if ( intensitiesValid_ && isDisplayReflectors_ ) 
    {
        QColor intensityColor;
    
        // bright returns
        for ( int i=0; i<qScan_.size()-1; ++i )
        {
            if ( intensities_[i] > 0 )
            {
                int greenness = intensities_[i] * 36;    // splits up the green colorspace into 8 equal spaces
                if ( intensities_[i] > 7 )
                {
                    cout << "ERROR(laserscanner2dpainter.cpp): invalid intensity: " << (int)(intensities_[i]) << endl;
                    continue;
                }
                intensityColor.setRgb( 0, greenness, 255 );
                painter->setPen( QPen(intensityColor, 1.0*onePix) );
                painter->setBrush( intensityColor );

                painter->save();
                painter->translate( qScan_[i+1].x(), qScan_[i+1].y() );

                painter->drawEllipse( QRectF(-brightReturnWidth_/2.0,
                                             -brightReturnWidth_/2.0,
                                             brightReturnWidth_,
                                             brightReturnWidth_) );

                painter->restore();
            }
        }
    }
    
    painter->restore();
}

void
LaserScanner2dPainter::execute( int action )
{
    // debug
    cout<<"LaserScanner2dPainter::execute: action #"<<action<<endl;

    switch ( action )
    {
    case 0 :
        // toggle scan
        isDisplayScan_ = !isDisplayScan_;
        break;
    case 1 :
        // toggle points
        isDisplayPoints_ = !isDisplayPoints_;
//         cout<<"TRACE(laserscanner2dpainter.cpp): isDisplayPoints_ set to " << isDisplayPoints_ << endl;
        break;
    case 2 :
        // toggle walls
        isDisplayWalls_ = !isDisplayWalls_;
        break;
    case 3 :
        // toggle reflectors
        isDisplayReflectors_ = !isDisplayReflectors_;
        break;
    case 4 :
        // toggle filling
        isFilledPolygon_ = !isFilledPolygon_;
        break;
    case 5 :
        // toggle transparency
        if (fillColor_.alpha()==127) {
            fillColor_.setAlpha(255);
        } else {
            fillColor_.setAlpha(127);
        }
        break;
    }
}

void LaserScanner2dPainter::setColor( QColor color )
{ 
    basisColor_ = color; 
    outlineColor_ = color;
}

void LaserScanner2dPainter::setFocus( bool inFocus )
{
    if (!inFocus) {
        outlineColor_=Qt::gray;
    } else {
        outlineColor_=basisColor_;
    }    
}
