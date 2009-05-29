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
#include <gbxutilacfr/mathdefs.h>

#include "drivebicyclepainter.h"

using namespace std;
using namespace orcaqgui2d;

DriveBicyclePainter::DriveBicyclePainter()
    : isDisplaySteerLimits_(true),
      isDisplayCurrentSteerAngle_(true),
      isDisplayRefSteerAngle_(true),
      outlineColor_(Qt::black),
      basisColor_(Qt::black)
{
}

void
DriveBicyclePainter::clear()
{
    qArrow_.clear();
}

void
DriveBicyclePainter::setDescription( orca::VehicleControlVelocityBicycleDescriptionPtr &description )
{
    description_ = description;
}

void
DriveBicyclePainter::setData( const orca::DriveBicycleData & data )
{
    data_        = data;
}

void
DriveBicyclePainter::paint( QPainter *painter, int z )
{
    const double ARROW_WIDTH = 0.1; // in m
    const double ARROW_LENGTH = 0.5; // in m

    if ( z != hydroqguielementutil::Z_POSE ||
	 !description_.get() )
    {
	return;
    }
    
    // scaling
    qreal onePix = painter->worldMatrix().inverted().mapRect(QRectF(0, 0, 1, 1)).width();

    clear();

    qArrow_.push_back( QPointF( 0, ARROW_WIDTH / 2 ));
    qArrow_.push_back( QPointF( ARROW_LENGTH, 0 ));
    qArrow_.push_back( QPointF( 0, -ARROW_WIDTH / 2 ));

    //
    // draw the reference line
    //
    if( isDisplaySteerLimits_ || isDisplayCurrentSteerAngle_ ||
	isDisplayRefSteerAngle_ )
    {
	painter->setPen( QPen( outlineColor_, onePix*1.5 ) );
	painter->drawLine( 0, 0, description_->wheelbase, 0 );
    }

    //
    // draw the steer limits
    //
    if( isDisplaySteerLimits_ )
    {
	painter->save();
	painter->translate( description_->wheelbase, 0 );

	int angleDeg =
	    (int) floor( RAD2DEG( description_->maxSteerAngle ) * 16 );
	painter->setPen( QPen( outlineColor_, onePix*1.5 ) );
	painter->drawPie( QRectF( -ARROW_LENGTH, -ARROW_LENGTH,
				  2.0*ARROW_LENGTH, 2.0*ARROW_LENGTH ),
			  angleDeg, -angleDeg * 2 );

	painter->restore();
    }

    //
    // draw the target steer angle
    //
    if( isDisplayRefSteerAngle_ )
    {
	painter->save();
	painter->translate( description_->wheelbase, 0 );
	painter->rotate( RAD2DEG( data_.referenceSteerAngle ) );

	painter->setPen( QPen( Qt::gray, onePix ) );
	painter->setBrush( Qt::gray );
	painter->drawConvexPolygon( qArrow_ );

	painter->restore();
    }

    //
    // draw the current steer angle
    //
    if( isDisplayCurrentSteerAngle_ )
    {
	painter->save();
	painter->translate( description_->wheelbase, 0 );
	painter->rotate( RAD2DEG( data_.currentSteerAngle ) );

	painter->setPen( QPen( outlineColor_, onePix ) );
	painter->setBrush( outlineColor_ );
	painter->drawConvexPolygon( qArrow_ );

	painter->restore();
    }
}

void
DriveBicyclePainter::execute( int action )
{
    // debug
    cout<<"DriveBicyclePainter::execute: action #"<<action<<endl;

    switch ( action )
    {
    case 0 :
        // toggle display steer limits
        isDisplaySteerLimits_ = !isDisplaySteerLimits_;
        break;
    case 1 :
	// toggle display current steer angle
	isDisplayCurrentSteerAngle_ = !isDisplayCurrentSteerAngle_;
	break;
    case 2 :
	// toggle display reference steer angle
	isDisplayRefSteerAngle_ = !isDisplayRefSteerAngle_;
	break;
    }
}

void DriveBicyclePainter::setFocus( bool inFocus )
{
    if (!inFocus) {
        outlineColor_=Qt::gray;
    } else {
        outlineColor_=basisColor_;
    }
}
