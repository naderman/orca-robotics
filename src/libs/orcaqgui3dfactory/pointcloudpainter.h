/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
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
  @brief LaserPainter paints laser scans
*/
class PointCloudPainter
{
  
  public:
    // -1 means 'default'
    PointCloudPainter( QColor outlineColor=QColor( 102,102,153, 255 ) );

    void setDescr( const orca::Frame3d &offset,
                   const orca::Size3d  &size );

    void setData( const orca::PointCloudData &pointCloud );

    void clear();
    
    void setColor( QColor color );
    void setFocus( bool inFocus );

    void toggleDisplayScan() { isDisplayScan_ = !isDisplayScan_; }
    void toggleDisplayPoints() { isDisplayPoints_ = !isDisplayPoints_; }

    osg::Node *osgNode() const { return root_.get(); }    

  private:

    std::vector<float>         ranges_;
    std::vector<unsigned char> intensities_;
    bool                       intensitiesValid_;

    bool isDisplayScan_;
    bool isDisplayPoints_;

    QColor outlineColor_;
    QColor basisColor_;

    osg::ref_ptr<osg::PositionAttitudeTransform> xformNode_;
    osg::ref_ptr<osg::Geode>                     scanNode_;

    osg::ref_ptr<osg::Group> root_;
};

} // namespace

#endif
