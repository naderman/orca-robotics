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

#include <orcaqgui3d/view.h>
#include <QGLWidget>
#include <qcolor.h>
#include <orca/featuremap2d.h>
#include <hydroqguielementutil/definitions2d.h>
#include <hydroqgui/hydroqgui.h>
#include <osg/Group>
#include <osg/Geode>

// forward declarations
class QPainter;

namespace orcaqgui3d {
    
    
class FeatureMap2dPainter
{
  public:
    FeatureMap2dPainter();

    void setData( const orca::FeatureMap2dData &featureData );
    void clear() {}
    void paint( const orcaqgui3d::View &view, QGLWidget &p ) {};
    
    int saveMap( const QString fileName, hydroqguielementutil::IHumanManager *humanManager ) const;

    osg::Node *osgNode() const { return root_.get(); }

  private:

    void 
    paintPointFeature( int featureType,
                       int featureNum,
                       double probExists,
                       double centreX,
                       double centreY,
                       double covXX,
                       double covXY,
                       double covYY );

    void paintPointFeature( const orca::CartesianPointFeature2d &f,
                            int featureNum );
    void  paintPoseFeature( const orca::CartesianPoseFeature2d &f,
                            int featureNum );
    void paintLineFeature(  const orca::CartesianLineFeature2d  &f,
                            int featureNum );

//     bool useTransparency_;
//     bool displayFeatureNumbers_;
//     bool displayUncertainty_;

    // Keep it so we can save it
    orca::FeatureMap2dData data_;

    osg::ref_ptr<osg::Group> root_;
};

}

#endif
