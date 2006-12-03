/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
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

    const double boxWidth = 0.750;
    const double lineWidthMax = boxWidth/2.0;

    painter->setBrush(QBrush());

    for ( unsigned int i=0; i<featureDataPtr_->features.size(); ++i )
    {
        painter->save();
        {
            const orca::SinglePolarFeature2dPtr &f = featureDataPtr_->features[i];
            painter->rotate( (int)floor(RAD2DEG(f->p.o)) );
            painter->translate( f->p.r, 0.0 );

            // Not sure of a better way to represent both pFalsePositive and pTruePositive
            // with just one number...
            double lineWidth = lineWidthMax*( f->pTruePositive-f->pFalsePositive );
            painter->setPen( QPen( orcaqgui::featureColour(f->type),lineWidth) );
            painter->drawRect( QRectF( -boxWidth/2.0, -boxWidth/2.0, boxWidth, boxWidth) );
        }
        painter->restore();
    }

}
