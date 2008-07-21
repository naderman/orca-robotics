/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAGUI3D_LOCALISE2D_PAINTER_H
#define ORCAGUI3D_LOCALISE2D_PAINTER_H

#include <QColor>
#include <QGLWidget>
#include <orcaqgui3d/view.h>
#include <orca/localise2d.h>
#include <hydroqguielementutil/paintutils.h>
#include <osg/Group>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

// forward declarations
class QPainter;

namespace orcaqgui3d
{
    
class Localise2dPainter
{
  public:
    Localise2dPainter( bool beginDisplayHistory );

    void setData( const orca::Localise2dData& data );
    void setCubicDescr( double length,
                        double width,
                        double height,
                        const orca::Frame3d &vehicleToGeometryTransform );
    void setCylindricalDescr( double radius,
                              double height,
                              const orca::Frame3d &vehicleToGeometryTransform );

    void paint( const orcaqgui3d::View &view, QGLWidget &p ) {};
    void setUseTransparency( bool useTransparency ) { useTransparency_= useTransparency; };
    void clear();
    void setColor( QColor color );
    void setFocus( bool inFocus );

    void toggleDisplayHistory()  { isDisplayHistory_ = !isDisplayHistory_; }
    void toggleMultiHypothesis() { isDisplayMultiHypothesis_ = !isDisplayMultiHypothesis_; }

    osg::Node *osgNode() const { return root_.get(); }

  private:

    void drawCuboidPlatform();
    void drawCylindricalPlatform();

    void paintHypothesis( const orca::Pose2dHypothesis &hypothesis );

//    orca::Localise2dData data_;
//    bool isDataAvailable_;

    QColor basicColor_;
    QColor currentColor_;

    bool useTransparency_;
    bool isDisplayHistory_;
    hydroqguielementutil::PoseHistory history_;

    bool isDisplayMultiHypothesis_;

    osg::ref_ptr<osg::PositionAttitudeTransform> platformNode_;

    osg::ref_ptr<osg::Group> root_;
};

}

#endif
