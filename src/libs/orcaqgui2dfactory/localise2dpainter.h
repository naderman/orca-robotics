/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAGUI_LOCALISE2D_PAINTER_H
#define ORCAGUI_LOCALISE2D_PAINTER_H

#include <QColor>
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui2d/paintutils.h>

// forward declarations
class QPainter;

namespace orcaqgui2d
{
    
enum PlatformType
{
    PlatformTypeCubic,
    PlatformTypeCylindrical
};

class Localise2dPainter
{
  public:
    // The default color is the color when the robot is not selected.
    Localise2dPainter( bool beginDisplayHistory );

    void setData( const orca::Localise2dData& data );
    void setPlatformType( PlatformType &type );
    
    void setTypeAndGeometry( PlatformType type, double length, double width ) { platformType_ = type; length_ = length; width_ = width;};
    void setTypeAndGeometry( PlatformType type, double radius ) { platformType_ = type; radius_ = radius; };
    void setOrigin( double x, double y, double o ) { originX_ = x; originY_ = y; originRot_ = o; };

    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==Z_POSE || z==Z_POSE-2;}
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
      
    void paintHypothesis( QPainter* p, const orca::Pose2dHypothesis &hypothesis );

    orca::Localise2dData data_;
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
