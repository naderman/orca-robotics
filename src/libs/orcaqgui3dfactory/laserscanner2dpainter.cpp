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
#include <osg/Point>
#include <orcaifacestring/bros1.h>

#include "laserscanner2dpainter.h"

using namespace std;
using namespace orca;
using namespace orcaqgui3d;

LaserScanner2dPainter::LaserScanner2dPainter( QColor outlineColor )
    : isDisplayScan_(true),
      isDisplayPoints_(true),
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
    if ( root_->containsNode( xformNode_.get() ) )
        root_->removeChild( xformNode_.get() );

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
LaserScanner2dPainter::setData( const orca::RangeScanner2dDataPtr &data )
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

    // Line primitive
    osg::ref_ptr<osg::DrawElementsUInt> linePrim =
        new osg::DrawElementsUInt(osg::PrimitiveSet::LINE_LOOP);
    geometry->addPrimitiveSet(linePrim.get());

    // Points primitive
    osg::ref_ptr<osg::DrawElementsUInt> pointsPrim;
    if ( isDisplayPoints_ )
    {
        pointsPrim = new osg::DrawElementsUInt(osg::PrimitiveSet::POINTS);
        geometry->addPrimitiveSet(pointsPrim.get());        
    }

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
    {
        linePrim->push_back(i);
        if ( isDisplayPoints_ )
            pointsPrim->push_back(i);
    }

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back( orcaqgui3d::toVec4( outlineColor_ ) );
    if ( isDisplayPoints_ )
        colors->push_back( osg::Vec4( 0, 0, 0, 1 ) );
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);    

    scanNode_ = new osg::Geode;
    scanNode_->addDrawable( geometry.get() );

    // Set the point size
    if ( isDisplayPoints_ )
    {
        const float pointSize = 3.0;
        osg::ref_ptr<osg::Point> pointAttr = new osg::Point( );
        pointAttr->setSize(pointSize);
        pointAttr->setSize(pointSize);
        osg::StateSet *stateSet = scanNode_->getOrCreateStateSet();        
        stateSet->setAttribute( pointAttr.get() );
    }

    xformNode_->addChild( scanNode_.get() );
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
