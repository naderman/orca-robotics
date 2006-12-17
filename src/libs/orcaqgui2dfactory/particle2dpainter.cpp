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
#include <cmath> 
#include <qpainter.h>

#include "particle2dpainter.h"
#include <orcaqgui2d/paintutils.h>
#include <orcaobj/mathdefs.h>

using namespace orca; 
using namespace std;

namespace orcaqgui
{

Particle2dPainter::Particle2dPainter( int particlePenThickness )
    : particlePenThickness_(particlePenThickness),
      activeColour_(Qt::darkCyan),
      kaputColour_(Qt::gray),
      currentColour_(kaputColour_)
{
}

Particle2dPainter::~Particle2dPainter()
{
}

void 
Particle2dPainter::setColor( QColor color )
{
    activeColour_ = color;
}

void
Particle2dPainter::clear()
{
    currentColour_ = kaputColour_;
}

void Particle2dPainter::setData( Particle2dDataPtr particleData )
{
    // cout<<"TRACE(particle2dpainter.cpp): setData()" << endl;

    // how long to draw the particles?
    const double pLength = 0.1;

    Particle2dSequence &particles = particleData->particles;

//        cout<<"TRACE(particle2dpainter.cpp): numParticles: " << particles.size() << endl;

    // set local storage
    qParticles_.resize( particles.size() );
    weights_.resize( particles.size() );

    if ( qParticles_.size() > 0 )
    {
        maxWeight_ = -1.0;
        for ( unsigned int i=0; i<particles.size(); ++i )
        {
            double x       = particles[i].pose.p.x;
            double y       = particles[i].pose.p.y;
            double heading = particles[i].pose.o;

            qParticles_[i] = QLineF( x, y, x+cos(heading)*pLength, y+sin(heading)*pLength );
            weights_[i] = particles[i].weight;
            assert( weights_[i] >= 0.0 );
            if ( weights_[i] > maxWeight_ )
                maxWeight_ = weights_[i];

//            cout << "  ["<<i<<"]: ("<<x<<","<<y<<","<<heading*180.0/M_PI<<"deg]"<<endl;
        }
        assert( maxWeight_ >= 0.0 );
    }
    currentColour_ = activeColour_;
}

void Particle2dPainter::paint( QPainter *painter, int z )
{
    if ( z != orcaqgui::Z_POSE-2 ) return;

    painter->setPen( QPen( currentColour_ ) );

    for ( int i=0; i < qParticles_.size(); i++ )
    {
        if (useTransparency_) {
            painter->setPen( QPen( getTransparentVersion( currentColour_, MAX( 0.25, weights_[i]/maxWeight_)  ) ) );
        } else {
            painter->setPen( QPen( currentColour_ ) );
        }
        painter->drawLine( qParticles_[i] );
    }
}

} // namespace

