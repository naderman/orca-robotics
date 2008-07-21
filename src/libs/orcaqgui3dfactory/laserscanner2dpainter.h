/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_LASERSCANNER2D_PAINTER_H
#define ORCAGUI3D_LASERSCANNER2D_PAINTER_H

#include <QColor>
#include <QGLWidget>
#include <orcaqgui3d/view.h>
#include <orca/laserscanner2d.h>
#include <osg/Group>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

namespace orcaqgui3d
{

/*!
  @brief LaserPainter paints laser scans
*/
class LaserScanner2dPainter
{
  
  public:
    // -1 means 'default'
    LaserScanner2dPainter( QColor outlineColor=QColor( 102,102,153, 255 ) );

    void setDescr( const orca::Frame3d &offset,
                   const orca::Size3d  &size );

    void setData( const orca::RangeScanner2dDataPtr &scan );

    void clear();
    
    void setColor( QColor color );
    void setFocus( bool inFocus );

    osg::Node *osgNode() const { return root_.get(); }    

  private:

    std::vector<float>         ranges_;
    std::vector<unsigned char> intensities_;
    bool                       intensitiesValid_;

    bool isDisplayScan_;
    bool isDisplayReflectors_;
    QColor outlineColor_;
    QColor basisColor_;

    osg::ref_ptr<osg::PositionAttitudeTransform> xformNode_;
    osg::ref_ptr<osg::Geode>                     scanNode_;

    osg::ref_ptr<osg::Group> root_;
};

} // namespace

#endif
