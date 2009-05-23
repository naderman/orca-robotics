/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_DRIVEBICYCLE_PAINTER_H
#define ORCAGUI_DRIVEBICYCLE_PAINTER_H

#include <QColor>
#include <QPolygonF>
#include <orca/drivebicycle.h>
#include <hydroqguielementutil/definitions2d.h>

class QPainter;

namespace orcaqgui2d
{

/*!
  @brief Paints ranger returns.
*/
class DriveBicyclePainter
{
  
  public:
    // -1 means 'default'
    DriveBicyclePainter();

    void setDescription( orca::VehicleControlVelocityBicycleDescriptionPtr
			 &description );
    void setData( const orca::DriveBicycleData &data );
    
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==hydroqguielementutil::Z_POSE;}

    void clear();

    void execute( int action );
    
    void setFocus( bool inFocus );

  private:

    orca::DriveBicycleData data_;

    QPolygonF qArrow_;

    bool isDisplaySteerLimits_;
    bool isDisplayCurrentSteerAngle_;
    bool isDisplayRefSteerAngle_;
    QColor outlineColor_;
    QColor basisColor_;

    orca::VehicleControlVelocityBicycleDescriptionPtr description_;

};

} // namespace

#endif
