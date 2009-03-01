/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
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
#include <orcaifaceutil/bros1.h>

#include "pointcloudpainter.h"

using namespace std;
using namespace orca;
using namespace orcaqgui3d;

namespace{
void
height2Color(const float &height, float &r,float &g, float &b){
    float min=-.5, max=1.5, mid=.5;
    if(min>height){
        // below min all black
        r=0.0; g=0.0; b=0.0;
    }
    else if( (mid>height) && (min<height) ){
        // min to mid maps from blue to green
        r=0.0;
        g=(height-min)/(mid-min);
        b=1.0-(height-min)/(mid-min);
    }
    else if( (mid<height) && (max>height) ){
        // mid to max maps from green to red
        b=0.0;
        r=(height-mid)/(max-mid);
        g=1.0-(height-mid)/(max-mid);
    }
    else if(max<height){
        r=1.0; g=1.0; b=1.0; // are all white points visible??
    }
}

} //namespace

PointCloudPainter::PointCloudPainter( QColor outlineColor )
    : isDisplayScan_(true),
      isDisplayPoints_(true),
      outlineColor_(outlineColor)
{
    root_ = new osg::Group;
}

void
PointCloudPainter::clear()
{
//    ranges_.clear();
    cout<<"TRACE(pointcloudpainter.cpp): TODO: " << __func__ << endl;    
}

void
PointCloudPainter::setDescr(  const orca::Frame3d &offset,
                                  const orca::Size3d  &size )
{
    cout<<"TRACE(pointcloudpainter.cpp): start: " << __func__ << endl;    
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
    cout<<"TRACE(pointcloudpainter.cpp): end: " << __func__ << endl;    
}

void
PointCloudPainter::setData( const orca::PointCloudData &data )
{
    cout<<"TRACE(pointcloudpainter.cpp): start: " << __func__ << endl;    
    //if ( data==0 ) return;

    // Clear out the old one
    if ( scanNode_.get() ){
        //xformNode_->removeChild( scanNode_.get() );
        root_->removeChild( scanNode_.get() );
    }

    //if ( !isDisplayScan_ ) return;
    if ( data.points.size() == 0 ) return;
    
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();    
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    geometry->setVertexArray( vertices.get() );

    // Points primitive
    osg::ref_ptr<osg::DrawElementsUInt> pointsPrim;
    pointsPrim = new osg::DrawElementsUInt(osg::PrimitiveSet::POINTS);
    geometry->addPrimitiveSet(pointsPrim.get());        

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

    for ( unsigned int i=0; i < data.points.size(); i++ ) {
        vertices->push_back( osg::Vec3( data.points[i].x,
                                        data.points[i].y,
                                        data.points[i].z) );
        pointsPrim->push_back(i);
        float r=0.0,g=0.0,b=0.0;
        height2Color(data.points[i].z, r, g, b);
        colors->push_back( osg::Vec4( r, g, b, 1 ) );
    }

    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);    

    scanNode_ = new osg::Geode;
    scanNode_->addDrawable( geometry.get() );

    // Set the point size
    const float pointSize = 1.0;
    osg::ref_ptr<osg::Point> pointAttr = new osg::Point( );
    pointAttr->setSize(pointSize);
    osg::StateSet *stateSet = scanNode_->getOrCreateStateSet();        
    stateSet->setAttribute( pointAttr.get() );

    //xformNode_->addChild( scanNode_.get() );
    root_->addChild( scanNode_.get() );
    cout<<"TRACE(pointcloudpainter.cpp): end: " << __func__ << endl;    
}

void PointCloudPainter::setColor( QColor color )
{ 
    cout<<"TRACE(pointcloudpainter.cpp): start: " << __func__ << endl;    
    basisColor_ = color; 
    outlineColor_ = color;
    cout<<"TRACE(pointcloudpainter.cpp): end: " << __func__ << endl;    
}

void PointCloudPainter::setFocus( bool inFocus )
{
    cout<<"TRACE(pointcloudpainter.cpp): start: " << __func__ << endl;    
    if (!inFocus) {
        outlineColor_=Qt::gray;
    } else {
        outlineColor_=basisColor_;
    }    
    cout<<"TRACE(pointcloudpainter.cpp): end: " << __func__ << endl;    
}
