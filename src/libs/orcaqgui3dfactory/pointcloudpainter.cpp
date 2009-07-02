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
#include <gbxutilacfr/mathdefs.h>
#include "pointcloudpainter.h"

using namespace std;
using namespace orcaqgui3d;

namespace{

void
height2Color(const float &height, float &r,float &g, float &b)
{
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

PointCloudPainter::PointCloudPainter()
{
    root_ = new osg::Group;
}

void
PointCloudPainter::clear()
{
    cout<<"TRACE(pointcloudpainter.cpp): TODO: " << __func__ << endl;    
}

void
PointCloudPainter::setData( const orca::PointCloudData &data )
{
    // Clear out the old one
    if ( pointCloudNode_.get() ){
        root_->removeChild( pointCloudNode_.get() );
    }
    pointCloudNode_ = new osg::Geode;

    if ( data.points.size() == 0 ) return;
    
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();    
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
    osg::ref_ptr<osg::DrawElementsUInt> pointsPrim = 
        new osg::DrawElementsUInt(osg::PrimitiveSet::POINTS);
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;

    for ( unsigned int i=0; i < data.points.size(); i++ )
    {
        vertices->push_back( osg::Vec3( data.points[i].x,
                                        data.points[i].y,
                                        data.points[i].z ) );
        pointsPrim->push_back(i);
        float r=0.0,g=0.0,b=0.0;
        height2Color(data.points[i].z, r, g, b);
        colors->push_back( osg::Vec4( r, g, b, 1 ) );
    }

    // assemble the osg represenation
    geometry->setVertexArray( vertices.get() );
    geometry->addPrimitiveSet(pointsPrim.get());
    geometry->setColorArray(colors.get());
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    pointCloudNode_->addDrawable( geometry.get() );

    // Set the point size
    const float pointSize = 3.0;
    osg::ref_ptr<osg::Point> pointAttr = new osg::Point();
    pointAttr->setSize(pointSize);
    osg::StateSet *stateSet = pointCloudNode_->getOrCreateStateSet();        
    stateSet->setAttribute( pointAttr.get() );

    root_->addChild( pointCloudNode_.get() );
}
