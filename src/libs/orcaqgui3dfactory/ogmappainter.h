/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_OGMAP_PAINTER_H
#define ORCAGUI_OGMAP_PAINTER_H

#include <orca/ogmap.h>
#include <hydroogmap/hydroogmap.h>
#include <orcaice/context.h>
#include <hydroqguielementutil/definitions2d.h>
#include <orcaqgui2dfactory/pixmappainter.h>
#include <osg/Group>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>

namespace orcaqgui3d
{

class OgMapPainter
{
    
  public:
    OgMapPainter();

    void setData( const orca::OgMapData& data );
    
    void clear() {};
    void toggleDisplay() { isDisplay_=!isDisplay_; setDisplayElements(); }
    void toggleWalls() { isDisplayWalls_=!isDisplayWalls_; setDisplayElements(); }
    void toggleGroundPlane() { isDisplayGroundPlane_=!isDisplayGroundPlane_; setDisplayElements(); }
    
    osg::Node *osgNode() const { return root_.get(); }

  private:

    void setDisplayElements();

    osg::ref_ptr<osg::Geode> drawAsWalls( const hydroogmap::OgMap &ogMap );
    osg::ref_ptr<osg::Geode> drawAsGroundPlane( const hydroogmap::OgMap &ogMap );

    osg::ref_ptr<osg::Group>                     root_;
    osg::ref_ptr<osg::PositionAttitudeTransform> offsetNode_;
    osg::ref_ptr<osg::Geode>                     groundPlaneGeode_;
    osg::ref_ptr<osg::Geode>                     wallsGeode_;

    bool isDisplay_;
    bool isDisplayWalls_;
    bool isDisplayGroundPlane_;
};

} // namespace
  
#endif
