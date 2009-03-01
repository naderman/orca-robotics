/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_POLARFEATURE2D_PAINTER_H
#define ORCAGUI_POLARFEATURE2D_PAINTER_H

#include <orca/polarfeature2d.h>
#include <hydroqguielementutil/definitions2d.h>
#include <hydroqgui/hydroqgui.h>

class QPainter;


namespace orcaqgui2d {

/*!
  \brief PolarFeature2dPainter paints polar feature
  \author Tobias Kaupp, t.kaupp@cas.edu.au, Alex Brooks
*/
class PolarFeature2dPainter
{
  
  public:
    PolarFeature2dPainter();

    void setOffset( orca::Frame3d &offset );
    void setData( const orca::PolarFeature2dData &featureData );
    void paint( QPainter *p, int z );
    bool paintThisLayer( int z ) const { return z==hydroqguielementutil::Z_LASER_FEATURES; }

    void clear();
    
    QStringList contextMenu();
    void execute( int action );

  private:

    double offsetX_;
    double offsetY_;
    double offsetYaw_;
	double offsetPitch_;
    bool   isOffsetSet_;

    bool isUpsideDown_;
    bool isNotHorizontal_;

    bool isDisplay_;

    orca::PolarFeature2dData featureData_;
};

}

#endif
