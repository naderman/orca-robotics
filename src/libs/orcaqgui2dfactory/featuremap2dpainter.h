/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef QORC_FEATUREMAP2D_PAINTER_H
#define QORC_FEATUREMAP2D_PAINTER_H


#include <qcolor.h>
#include <orca/featuremap2d.h>
#include <hydroqguielementutil/definitions2d.h>
#include <hydroqgui/hydroqgui.h>

// forward declarations
class QPainter;

namespace orcaqgui2d {
    
    
class FeatureMap2dPainter
{
  public:
    FeatureMap2dPainter();
    ~FeatureMap2dPainter();

    void setData( const orca::FeatureMap2dData &featureData );
    void paint( QPainter *p, int z );
    bool paintThisLayer(int z) const {return z==hydroqguielementutil::Z_SLAM_MAP;}
    void setUseTransparency( bool useTransparency ) { useTransparency_= useTransparency; };
    
    void clear();
    
    void toggleFeatureNumbers() { displayFeatureNumbers_ = !displayFeatureNumbers_; }
    void toggleUncertainty() { displayUncertainty_ = !displayUncertainty_; }
    
    int saveMap( const QString fileName, hydroqguielementutil::IHumanManager *humanManager ) const;

  private:

    void 
    paintPointFeature( QPainter *painter, 
                       int featureType,
                       int featureNum,
                       double probExists,
                       double centreX,
                       double centreY,
                       double covXX,
                       double covXY,
                       double covYY );

    void paintPointFeature( QPainter *painter,
                            const orca::CartesianPointFeature2d &f,
                            int featureNum );
    void  paintPoseFeature( QPainter *painter, 
                            const orca::CartesianPoseFeature2d &f,
                            int featureNum );
    void paintLineFeature(  QPainter *painter,
                            const orca::CartesianLineFeature2d  &f,
                            int featureNum );

    orca::FeatureMap2dData data_;
    bool useTransparency_;
    bool displayFeatureNumbers_;
    bool displayUncertainty_;
};

}

#endif
