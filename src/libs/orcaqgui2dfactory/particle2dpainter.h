/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAGUI_PARTICLE2D_PAINTER_H
#define ORCAGUI_PARTICLE2D_PAINTER_H

#include <QVector>
#include <QLine>
#include <orca/particle2d.h>
#include <orcaqgui2d/definitions2d.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {

/*!
  \brief Particle2DPainter is used for displaying the particles from a Monte Carlo Localization component.
  \author Alex Brooks
*/
class Particle2dPainter
{
  
  public:
    Particle2dPainter( int particlePenThickness=30 );
    ~Particle2dPainter();

    void setData( const orca::Particle2dData& particleData );
    void setColor( QColor color );
    void paint( QPainter *p, int z1 );
    bool paintThisLayer(int z) const { return z==Z_POSE-2; }
    void setTransparency( bool useTransparency ) { useTransparency_= useTransparency; };
    void clear();
    
  private:

    QVector<QLineF> qParticles_;   // in mm
    std::vector<float> weights_;
    float              maxWeight_;
    int particlePenThickness_;
    
    bool useTransparency_;
    QColor activeColour_;
    QColor kaputColour_;
    QColor currentColour_;
};

}

#endif
