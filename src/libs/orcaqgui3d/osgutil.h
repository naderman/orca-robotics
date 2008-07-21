#ifndef ORCAQGUI3D_OSGUTIL_H
#define ORCAQGUI3D_OSGUTIL_H

#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <orcaqgui3d/coordinateframe.h>
#include <QColor>

namespace orcaqgui3d {

osg::ref_ptr<osg::PositionAttitudeTransform>
getPositionAttitudeTransform( double x,
                              double y,
                              double z,
                              double roll,
                              double pitch,
                              double yaw );

// Force the Node and its children to be wireframe, overrides parents state
void
setWireFrameMode( osg::Node *srcNode );

void
setAntiAliasing( osg::Node *node );

//! draws an axis-aligned ellipse at height z
osg::ref_ptr<osg::Geometry>
drawEllipse( float radiusX,
             float radiusY,
             const osg::Vec4 &color,
             float z=0,
             int numPts=25 );

//! draws a cylinder oriented in the +z direction
osg::ref_ptr<osg::Geode>
drawCylinder( float height,
              float radius,
              const osg::Vec4 &color );

inline osg::Vec4 toVec4( const QColor &color )
{ return osg::Vec4( color.red()/255.0,
                    color.green()/255.0,
                    color.blue()/255.0,
                    color.alpha()/255.0 ); }

}

#endif
