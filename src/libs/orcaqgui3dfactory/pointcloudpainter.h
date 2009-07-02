/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_POINT_CLOUD_PAINTER_H
#define ORCAGUI3D_POINT_CLOUD_PAINTER_H

#include <QColor>
#include <orca/pointcloud.h>
#include <osg/Group>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

namespace orcaqgui3d
{

/*!
  @brief A 3D painter
*/
class PointCloudPainter
{
  
  public:
    PointCloudPainter();

    void setData( const orca::PointCloudData &pointCloud );

    void clear();
    
    osg::Node *osgNode() const { return root_.get(); }

  private:

    osg::ref_ptr<osg::Geode> pointCloudNode_;
    osg::ref_ptr<osg::Group> root_;
};

} // namespace

#endif
