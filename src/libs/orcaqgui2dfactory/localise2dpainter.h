/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAGUI_LOCALISE2D_PAINTER_H
#define ORCAGUI_LOCALISE2D_PAINTER_H

#include <QColor>
#include <hydroqguielementutil/definitions2d.h>
#include <hydroqguielementutil/paintutils.h>
#include <orca/localise2d.h>

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
    Localise2dPainter( bool beginDisplayHistory );

    void setData( const orca::Localise2dData& data );
    void setPlatformType( PlatformType &type );
    
    void setTypeAndGeometry( PlatformType type, double length, double width ) { platformType_ = type; length_ = length; width_ = width;};
    void setTypeAndGeometry( PlatformType type, double radius ) { platformType_ = type; radius_ = radius; };
    void setOrigin( double x, double y, double o ) { originX_ = x; originY_ = y; originRot_ = o; };

    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==hydroqguielementutil::Z_POSE || z==hydroqguielementutil::Z_POSE-2;}
    void setUseTransparency( bool useTransparency ) { useTransparency_= useTransparency; };
    void clear();
    void setColor( QColor color );
    void setFocus( bool inFocus );

    void toggleDisplayHistory()  { isDisplayHistory_ = !isDisplayHistory_; }
    void toggleMultiHypothesis() { isDisplayMultiHypothesis_ = !isDisplayMultiHypothesis_; }
    void toggleUncertainty() { isDisplayUncertainty_ = !isDisplayUncertainty_; }
    void setMaxNumHistoryPoints( int maxNumPoints ) { history_.setMaxNumPoints( maxNumPoints ); }
    int currentMaxNumHistoryPoints() const { return history_.currentMaxNumPoints(); }

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
    hydroqguielementutil::PoseHistory history_;

    bool isDisplayMultiHypothesis_;
    bool isDisplayUncertainty_;

    bool isInFocus_;
};

}

#endif
