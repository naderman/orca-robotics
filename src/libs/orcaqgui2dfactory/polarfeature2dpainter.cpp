/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <QPainter>
#include <QString>

#include <orcaice/orcaice.h>

#include "polarfeature2dpainter.h"
#include <orcaqgui/features.h>

using namespace std;
using namespace orca;
using namespace orcaqgui;
using namespace orcaqgui2d;

namespace {
    const double BOX_WIDTH = 0.750;
    const double LINE_WIDTH_MAX = BOX_WIDTH/2.0;
}

PolarFeature2dPainter::PolarFeature2dPainter()
{
}

PolarFeature2dPainter::~PolarFeature2dPainter()
{
}

void 
PolarFeature2dPainter::clear()
{
}

void 
PolarFeature2dPainter::setData( const orca::PolarFeature2dDataPtr & data )
{
    if ( data==0 ) return;
    
    featureDataPtr_ = data;

//    cout<<"TRACE(polarfeature2dpainter.cpp): setData: " << data << endl;
}

void
drawPointFeature( QPainter *painter, const orca::PointPolarFeature2d &f )
{
    painter->save();
    {
        painter->rotate( (int)floor(RAD2DEG(f.p.o)) );
        painter->translate( f.p.r, 0.0 );
        
        // Not sure of a better way to represent both pFalsePositive and pTruePositive
        // with just one number...
        double lineWidth = LINE_WIDTH_MAX*( f.pTruePositive-f.pFalsePositive );
        painter->setPen( QPen( featureColour(f.type),lineWidth) );
        painter->drawRect( QRectF( -BOX_WIDTH/2.0, -BOX_WIDTH/2.0, BOX_WIDTH, BOX_WIDTH) );
    }
    painter->restore();
}

void
drawLineFeature( QPainter *painter, const orca::LinePolarFeature2d &f )
{
    painter->save();
    {
        double lineWidth = LINE_WIDTH_MAX*( f.pTruePositive-f.pFalsePositive );
        painter->setPen( QPen( featureColour(f.type), lineWidth ) );

        double x1 = f.start.r*cos(f.start.o);
        double y1 = f.start.r*sin(f.start.o);
        double x2 = f.end.r*cos(f.end.o);
        double y2 = f.end.r*sin(f.end.o);

        // Move to the first point, and rotate the x-axis parallel to the line
        painter->translate( x1, y1 );
        painter->rotate( atan2(y2-y1,x2-x1)*180.0/M_PI );

        double len = hypotf(y2-y1,x2-x1);

        painter->drawLine( QLineF( 0,  BOX_WIDTH/2, len,  BOX_WIDTH/2 ) );
        painter->drawLine( QLineF( 0, -BOX_WIDTH/2, len, -BOX_WIDTH/2 ) );

        if ( f.startSighted )
            painter->drawLine( QLineF( 0, -BOX_WIDTH/2, 0, BOX_WIDTH/2 ) );
        if ( f.endSighted )
            painter->drawLine( QLineF( len, -BOX_WIDTH/2, len, BOX_WIDTH/2 ) );
    }
    painter->restore();
}

void
PolarFeature2dPainter::paint( QPainter *painter, int z )
{
    if ( z != Z_LASER_FEATURES ) return;
    if ( !featureDataPtr_ ) return;

    // debug
//     cout<<"["<<featureDataPtr_->features.size()<<"] ";
//     for ( unsigned int i=0; i<featureDataPtr_->features.size(); ++i ) {
//         cout<<" ("<<featureDataPtr_->features[i]->p.r<<","<<RAD2DEG(featureDataPtr_->features[i]->p.o)<<")";
//         //if ( i>10 ) break;
//     }
//     cout<<endl;

    painter->setBrush(QBrush());

    for ( unsigned int i=0; i<featureDataPtr_->features.size(); ++i )
    {
        const orca::SinglePolarFeature2d &ftr = *(featureDataPtr_->features[i]);
        switch ( ftr.type )
        {
        case orca::feature::LINE:
        {
            const orca::LinePolarFeature2d &lf = dynamic_cast<const LinePolarFeature2d&>(ftr);
            drawLineFeature( painter, lf );
            break;
        }
        default:
        {
            const orca::PointPolarFeature2d &pf = dynamic_cast<const PointPolarFeature2d&>(ftr);
            drawPointFeature( painter, pf );
            break;
        }
        }
    }
}


