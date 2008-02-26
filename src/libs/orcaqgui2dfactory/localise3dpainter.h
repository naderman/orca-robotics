/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAGUI_LOCALISE3D_PAINTER_H
#define ORCAGUI_LOCALISE3D_PAINTER_H

#include <QColor>
#include <hydroqgui/definitions2d.h>
#include <orca/localise3d.h>
#include <orcaqgui2d/paintutils.h>
#include <orcaqgui2dfactory/localise2dpainter.h> // for PlatformType

// forward declarations
class QPainter;

namespace orcaqgui2d
{

class Localise3dPainter
{
  public:
    // The default color is the color when the robot is not selected.
    Localise3dPainter( bool beginDisplayHistory );

    void setData( const orca::Localise3dData& data );
    void setPlatformType( PlatformType &type );
    
    void setTypeAndGeometry( PlatformType type, double length, double width ) { platformType_ = type; length_ = length; width_ = width;};
    void setTypeAndGeometry( PlatformType type, double radius ) { platformType_ = type; radius_ = radius; };
    void setOrigin( double x, double y, double o ) { originX_ = x; originY_ = y; originRot_ = o; };

    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==hydroqgui::Z_POSE || z==hydroqgui::Z_POSE-2;}
    void setUseTransparency( bool useTransparency ) { useTransparency_= useTransparency; };
    void clear();
    void setColor( QColor color );
    void setFocus( bool inFocus );

    void toggleDisplayHistory()  { isDisplayHistory_ = !isDisplayHistory_; }
    void toggleMultiHypothesis() { isDisplayMultiHypothesis_ = !isDisplayMultiHypothesis_; }

  private:
      
    double length_;
    double width_;
    double radius_;
    PlatformType platformType_;
    
    double originX_;
    double originY_;
    double originRot_;

    void paintHypothesis( QPainter* p, const orca::Pose3dHypothesis &hypothesis );

    orca::Localise3dData data_;
    bool isDataAvailable_;

    QColor basicColor_;
    QColor currentColor_;

    bool useTransparency_;
    bool isDisplayHistory_;
    PoseHistory history_;

    bool isDisplayMultiHypothesis_;
};

}

#endif
