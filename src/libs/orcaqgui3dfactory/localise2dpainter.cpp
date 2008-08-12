/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>

#include <QPainter>
#include <orcaobj/orcaobj.h>
#include <orcaice/orcaice.h>
#include <gbxutilacfr/mathdefs.h>
#include <hydroqgui/hydroqgui.h>
#include "localise2dpainter.h"
#include <orcaqgui3d/glutil.h>
#include <osg/ShapeDrawable>
#include <orcaqgui3d/osgutil.h>
#include <orcaifacestring/localise2d.h>

using namespace std;

namespace orcaqgui3d {

Localise2dPainter::Localise2dPainter( bool beginDisplayHistory )
    : basicColor_(Qt::blue),
      isDisplayMultiHypothesis_(true)
{
    root_ = new osg::Group();
}

void
Localise2dPainter::setCubicDescr( double length,
                                  double width,
                                  double height,
                                  const orca::Frame3d &platformToGeometryTransform )
{
    cout<<"TRACE(localise2dpainter.cpp): platformToGeometryTransform: " << ifacestring::toString(platformToGeometryTransform) << endl;

    assert( fabs(platformToGeometryTransform.o.r) < 1e-9 &&
            fabs(platformToGeometryTransform.o.p) < 1e-9 &&
            fabs(platformToGeometryTransform.o.y) < 1e-9 && "Not implemented!" );

    platformNode_ = new osg::PositionAttitudeTransform;
    platformNode_->setPosition( osg::Vec3( platformToGeometryTransform.p.x,
                                           platformToGeometryTransform.p.y,
                                           platformToGeometryTransform.p.z ) );

    osg::ref_ptr<osg::Box> box = new osg::Box( osg::Vec3( 0, 0, 0 ), length, width, height );
    osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(box.get());
    drawable->setColor( orcaqgui3d::toVec4( currentColor_ ) );

    osg::ref_ptr<osg::Geode> geode = new osg::Geode;
    geode->addDrawable( drawable.get() );

    platformNode_->addChild( geode.get() );
}

void
Localise2dPainter::setCylindricalDescr( double radius,
                                        double height,
                                        const orca::Frame3d &platformToGeometryTransform )
{
    cout<<"TRACE(localise2dpainter.cpp): platformToGeometryTransform: " << ifacestring::toString(platformToGeometryTransform) << endl;

    assert( fabs(platformToGeometryTransform.o.r) < 1e-9 &&
            fabs(platformToGeometryTransform.o.p) < 1e-9 &&
            fabs(platformToGeometryTransform.o.y) < 1e-9 && "Not implemented!" );

    platformNode_ = new osg::PositionAttitudeTransform;
    platformNode_->setPosition( osg::Vec3( platformToGeometryTransform.p.x,
                                           platformToGeometryTransform.p.y,
                                           platformToGeometryTransform.p.z ) );

    osg::Vec4 color = orcaqgui3d::toVec4( currentColor_ );
    platformNode_->addChild( orcaqgui3d::drawCylinder( height, radius, color ).get() );
}

void
Localise2dPainter::clear()
{
    cout<<"TRACE(localise2dpainter.cpp): TODO: "<<__func__ << endl;
}

void 
Localise2dPainter::setData( const orca::Localise2dData& data )
{    
    // Clear
    root_->removeChildren( 0, root_->getNumChildren() );

    // And re-set
    if ( data.hypotheses.size() != 0 )
    {
        if ( isDisplayMultiHypothesis_ )
        {
            for ( unsigned int i=0; i<data.hypotheses.size(); ++i )
            {
                paintHypothesis( data.hypotheses[i] );
            }
        }
        else
        {
            paintHypothesis( orcaobj::mlHypothesis( data ) );
        }
    }
}

void 
Localise2dPainter::paintHypothesis( const orca::Pose2dHypothesis &hypothesis )
{
    if ( !platformNode_ ) return;

    // Paint outselves a fixed height off the ground
    // (note: there's a similar hack in localise2delement.h)
    const double Z_OFFSET = 0.35;

//    const float weight = hypothesis.weight;
    const orca::Frame2d      &mean = hypothesis.mean;
//    const orca::Covariance2d &cov  = hypothesis.cov;

    osg::ref_ptr<osg::PositionAttitudeTransform> pos = new osg::PositionAttitudeTransform;
    pos->setPosition( osg::Vec3( mean.p.x, mean.p.y, Z_OFFSET ) );
    osg::Quat rotation; rotation.makeRotate( mean.o, osg::Vec3(0,0,1) );
    pos->setAttitude( rotation );

    pos->addChild( platformNode_.get() );
    root_->addChild( pos.get() );


    // TODO: draw uncertainty.
}

void Localise2dPainter::setColor( QColor color )
{
    basicColor_ = color;
    currentColor_ = color;

    // TODO: we should also modify the scenegraph here, or nothing will happen...
}
    
void Localise2dPainter::setFocus( bool inFocus )
{
    //cout << "TRACE(localise2dpainter.pp): inFocus is " << inFocus << endl;
    if (!inFocus) {
        currentColor_=Qt::gray;
    } else {
        currentColor_=basicColor_;
    }
}

    
}
