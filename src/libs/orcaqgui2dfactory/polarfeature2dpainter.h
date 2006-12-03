/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_POLARFEATURE2D_PAINTER_H
#define ORCAGUI_POLARFEATURE2D_PAINTER_H

#include <orca/polarfeature2d.h>
#include <orcaqgui2d/definitions2d.h>

class QPainter;


namespace orcaqgui {

/*!
  \brief PolarFeature2dPainter paints polar feature
  \author Tobias Kaupp, t.kaupp@cas.edu.au, Alex Brooks
*/
class PolarFeature2dPainter
{
  
  public:
    PolarFeature2dPainter();
    ~PolarFeature2dPainter();

    void setData( const orca::PolarFeature2dDataPtr & featureDataPtr );
    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const { return z==Z_LASER_FEATURES; }

    void clear();
    
  private:

    orca::PolarFeature2dDataPtr featureDataPtr_;
};

}

#endif
