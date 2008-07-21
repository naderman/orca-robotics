/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <assert.h>
#include <QString>
#include <orcaqgui3d/glutil.h>
#include <orcaqgui3d/osgutil.h>
#include <orcaice/orcaice.h>
#include <osg/ShapeDrawable>
#include <osg/Geometry>

#include "laserscanner2dpainter.h"

using namespace std;
using namespace orca;
using namespace orcaqgui3d;

LaserScanner2dPainter::LaserScanner2dPainter( QColor outlineColor )
    : isDisplayScan_(true),
      isDisplayReflectors_(true),
      outlineColor_(outlineColor)
{
    root_ = new osg::Group;
}

void
LaserScanner2dPainter::clear()
{
//    ranges_.clear();
    cout<<"TRACE(laserscanner2dpainter.cpp): TODO: " << __func__ << endl;    
}

void
LaserScanner2dPainter::setDescr(  const orca::Frame3d &offset,
                                  const orca::Size3d  &size )
{
    cout<<"TRACE(laserscanner2dpainter.cpp): offset: " << offset.p.x << ", " << offset.p.y << ", " << offset.p.z << endl;

    xformNode_ = orcaqgui3d::getPositionAttitudeTransform( offset.p.x,
                                                           offset.p.y,
                                                           offset.p.z,
                                                           offset.o.r,
                                                           offset.o.p,
                                                           offset.o.y );
    
    osg::ref_ptr<osg::Box> box = new osg::Box( osg::Vec3( 0, 0, 0 ),
                                               size.l, size.w, size.h );

    osg::ref_ptr<osg::ShapeDrawable> filledDrawable = new osg::ShapeDrawable(box.get());
    filledDrawable->setColor( osg::Vec4(0,0,1,1) );
    osg::ref_ptr<osg::Geode> filledGeode = new osg::Geode;
    filledGeode->addDrawable( filledDrawable.get() );

    osg::ref_ptr<osg::ShapeDrawable> wfDrawable = new osg::ShapeDrawable(box.get());
    wfDrawable->setColor( osg::Vec4(0,0,0,1) );
    osg::ref_ptr<osg::Geode> wireFrameGeode = new osg::Geode;
    wireFrameGeode->addDrawable( wfDrawable.get() );
    orcaqgui3d::setWireFrameMode( wireFrameGeode.get() );

    xformNode_->addChild( filledGeode.get() );
    xformNode_->addChild( wireFrameGeode.get() );

    root_->addChild( xformNode_.get() );
}

void
LaserScanner2dPainter::setData( const orca::RangeScanner2dDataPtr & data )
{
    if ( data==0 ) return;

    // Clear out the old one
    if ( scanNode_.get() )
        xformNode_->removeChild( scanNode_.get() );

    // Check if this thing is a laser scan.
    orca::LaserScanner2dDataPtr laserScan = orca::LaserScanner2dDataPtr::dynamicCast( data );
    if ( laserScan )
    {
        intensitiesValid_ = true;
        intensities_      = laserScan->intensities;        
    }

    if ( !isDisplayScan_ ) return;
    if ( data->ranges.size() == 0 ) return;
    
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();    
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    geometry->setVertexArray( vertices.get() );

    osg::ref_ptr<osg::DrawElementsUInt> prim =
        new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
    geometry->addPrimitiveSet(prim.get());

    vertices->push_back( osg::Vec3(0,0,0) );
    double angleIncrement = data->fieldOfView / double(data->ranges.size()+1);
    for ( unsigned int i=0; i < data->ranges.size(); i++ )
    {
        float bearing = data->startAngle + i * angleIncrement;
        vertices->push_back( osg::Vec3( data->ranges[i]*cos(bearing),
                                        data->ranges[i]*sin(bearing),
                                        0 ) );
    }
    for ( unsigned int i=0; i < vertices->size(); i++ )
        prim->push_back(i);

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( osg::Vec4( outlineColor_.red(),
                                  outlineColor_.green(),
                                  outlineColor_.blue(),
                                  1.0 ) );
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);    

    scanNode_ = new osg::Geode;
    scanNode_->addDrawable( geometry.get() );

    xformNode_->addChild( scanNode_.get() );
}

void
LaserScanner2dPainter::paint( const orcaqgui3d::View &view, QGLWidget &p )
{
//     // Apply the vehicle-to-sensor transformation
//     orcaqgui3d::glutil::transform( offsetX_, offsetY_, offsetZ_,
//                                    offsetRoll_, offsetPitch_, offsetYaw_ );

//     const bool drawSurfaces  = true;
//     const bool drawWireFrame = true;
//     glColor3f( 0, 0, 1 );
//     orcaqgui3d::glutil::drawBox( size_.l,
//                                  size_.w,
//                                  size_.h,
//                                  drawSurfaces, 
//                                  drawWireFrame );
    
//     if ( !isDisplayScan_ ) return;
//     if ( ranges_.size() == 0 ) return;

// //    Polygon doesn't work so good...  slows things _right_ down.
// //    glBegin( GL_POLYGON );  
//     glBegin( GL_LINE_LOOP );  
//     glColor3f( outlineColor_.red(), outlineColor_.green(), outlineColor_.blue() );
//     glVertex3f( 0, 0, 0 );
//     double angleIncrement = fieldOfView_ / double(ranges_.size()+1);
//     for ( unsigned int i=0; i < ranges_.size(); ++i )
//     {
//         float bearing = startAngle_ + i * angleIncrement;
//         glVertex3f( ranges_[i] * cos(bearing),
//                     ranges_[i] * sin(bearing),
//                     0 );
//     }
//     glEnd();  
}

void LaserScanner2dPainter::setColor( QColor color )
{ 
    basisColor_ = color; 
    outlineColor_ = color;
}

void LaserScanner2dPainter::setFocus( bool inFocus )
{
    if (!inFocus) {
        outlineColor_=Qt::gray;
    } else {
        outlineColor_=basisColor_;
    }    
}
