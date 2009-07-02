/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

#include "rangerarraypainter.h"

using namespace std;
using namespace orcaqgui2d;

RangerArrayPainter::RangerArrayPainter( QColor outlineColor,
                            float  outlineThickness,
                            float  brightReturnWidth )
    : isDisplayScan_(true),
      isDisplayPoints_(false),
      isFilledPolygon_(false),
      outlineColor_(outlineColor),
      basisColor_(outlineColor),
      outlineThickness_(outlineThickness)
{
    fillColor_ = QColor(204,204,255,127);
}

void
RangerArrayPainter::clear()
{
    for ( unsigned int i=0; i<rangers_.size(); ++i )
    {
        rangers_[i].qScan.clear();
        rangers_[i].qReturn.clear();
        rangers_[i].qReturnArc.clear();
    }
}

void
RangerArrayPainter::setDescription( orca::RangerArrayDescription &description )
{
    description_ = description;
}

void
RangerArrayPainter::setData( const orca::RangerArrayDataPtr & data )
{
    if ( data==0 ) return;

    ranges_        = data->ranges;

    if ( rangers_.size() != ranges_.size() )
    {
        rangers_.resize( ranges_.size() );
    }
    clear();

    QPointF point;

    for ( unsigned int i=0; i<rangers_.size(); ++i )
    {
	rangers_[i].qScan.push_back( QPointF(0.0,0.0) );    // first point in array is origin

        // convert from polar to cartesian coord. sys.
        const int DRAWN_POINT_COUNT = 5;
        for ( int j = -DRAWN_POINT_COUNT / 2; j<=DRAWN_POINT_COUNT/2; ++j ) {

            // account for sensor heading:
	    // - only the x-coordinate becomes "compressed" by pitch
	    // - only the y-coordinate becomes "compressed" by roll
            double bearing = 0.5 * description_.detectionConeAngle * (j / (DRAWN_POINT_COUNT / 2));
	    point.setX( ranges_[i] * cos( bearing ) * cos( description_.poses[i].o.p ) );
    	    point.setY( ranges_[i] * sin( bearing ) * cos( description_.poses[i].o.r ) );

	    rangers_[i].qScan.push_back( point );
    	    rangers_[i].qReturnArc.push_back( point );
	    if(j == 0)
	    {
        	rangers_[i].qReturn.push_back( point );
    	    }
	}
    }
}

void
RangerArrayPainter::paint( QPainter *painter, int z )
{
    if ( z != hydroqguielementutil::Z_RANGERARRAY || !isDisplayScan_ ) return;
    if ( rangers_.empty() ) return;
    
    // scaling
    qreal onePix = painter->worldMatrix().inverted().mapRect(QRectF(0, 0, 1, 1)).width();

    for ( std::size_t rayNum=0; rayNum<rangers_.size(); ++rayNum )
    {
	painter->save();
    
	// handle offset of this ray
	painter->translate( description_.poses[rayNum].p.x, description_.poses[rayNum].p.y );
	painter->rotate( description_.poses[rayNum].o.y*180.0/M_PI );

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
	painter->drawConvexPolygon( rangers_[rayNum].qScan );

	//
	// draw the arc
	//
    	painter->setPen( QPen( outlineColor_, 3.0*onePix ) );
    	painter->drawPolyline( rangers_[rayNum].qReturnArc );

	if ( isDisplayPoints_ )
	{
    	    painter->setPen( QPen( Qt::black, 3.0*onePix ) );
//         	painter->setPen( QPen( Qt::black ) );
    	    painter->drawPoints( rangers_[rayNum].qReturn );
	}

	painter->restore();
    }
}

void
RangerArrayPainter::execute( int action )
{
    // debug
    cout<<"RangerArrayPainter::execute: action #"<<action<<endl;

    switch ( action )
    {
    case 0 :
        // toggle scan
        isDisplayScan_ = !isDisplayScan_;
        break;
    case 1 :
        // toggle points
        isDisplayPoints_ = !isDisplayPoints_;
//         cout<<"TRACE(rangerarraypainter.cpp): isDisplayPoints_ set to " << isDisplayPoints_ << endl;
        break;
    case 2 :
        // toggle filling
        isFilledPolygon_ = !isFilledPolygon_;
        break;
    case 3 :
        // toggle transparency
        if (fillColor_.alpha()==127) {
            fillColor_.setAlpha(255);
        } else {
            fillColor_.setAlpha(127);
        }
        break;
    }
}

void RangerArrayPainter::setFocus( bool inFocus )
{
    if (!inFocus) {
        outlineColor_=Qt::gray;
    } else {
        outlineColor_=basisColor_;
    }
}
