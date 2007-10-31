/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAGUI_ODOMETRY2D_PAINTER_H
#define ORCAGUI_ODOMETRY2D_PAINTER_H

#include <orca/odometry2d.h>
#include <orcaqgui2d/definitions2d.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {

/*!
  \brief Odometry2DPainter is used for displaying the odometric velocities in the robotic platform frame.
  \author Tobias Kaupp
*/
class Odometry2dPainter
{
  
  public:
    Odometry2dPainter();
    ~Odometry2dPainter();

    void setData( const orca::Odometry2dData& data );
    void setColor( QColor color );
    void paint( QPainter *p, int z1 );
    bool paintThisLayer(int z) const { return z==Z_POSE; }
    void setTransparency( bool useTransparency ) { useTransparency_= useTransparency; };
    void clear() {};
    
  private:

    double speedX_;       // m/s
    double speedY_;       // m/s
    double turnrate_;     // rad/s
    
    bool useTransparency_;  // currently not used here
    QColor currentColour_;
};

}

#endif
