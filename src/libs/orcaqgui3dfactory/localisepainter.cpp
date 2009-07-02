/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>

#include <QPainter>
#include <orcaobj/localise2d.h>
#include <orcaobj/localise3d.h>
#include <orcaice/orcaice.h>
#include <gbxutilacfr/mathdefs.h>
#include "localisepainter.h"
#include <orcaqgui3d/glutil.h>
#include <osg/ShapeDrawable>
#include <orcaqgui3d/osgutil.h>
#include <orcaifaceutil/localise2d.h>
#include <orcaobj/bros1.h>

using namespace std;

namespace orcaqgui3d {

namespace {

    orca::Pose3dHypothesis projectTo3d( const orca::Pose2dHypothesis &h2d,
                                        double zVal )
    {
        orca::Pose3dHypothesis h3d;
        h3d.weight = h2d.weight;

        h3d.mean.p.x = h2d.mean.p.x;
        h3d.mean.p.y = h2d.mean.p.y;
        h3d.mean.p.z = zVal;
        h3d.mean.o.r = 0.0;
        h3d.mean.o.p = 0.0;
        h3d.mean.o.y = h2d.mean.o;

        h3d.cov.xx = h2d.cov.xx;
        h3d.cov.xy = h2d.cov.xy;
        h3d.cov.xz = 0.0;
        h3d.cov.xr = 0.0;
        h3d.cov.xp = 0.0;
        h3d.cov.xa = h2d.cov.xt;
        h3d.cov.yy = h2d.cov.yy;
        h3d.cov.yz = 0.0;
        h3d.cov.yr = 0.0;
        h3d.cov.yp = 0.0;
        h3d.cov.ya = h2d.cov.yt;
        h3d.cov.zz = 1e-2;
        h3d.cov.zr = 0.0;
        h3d.cov.zp = 0.0;
        h3d.cov.za = 0.0;
        h3d.cov.rr = 1e-2;
        h3d.cov.rp = 0.0;
        h3d.cov.ra = 0.0;
        h3d.cov.pp = 1e-2;
        h3d.cov.pa = 0.0;
        h3d.cov.aa = h2d.cov.tt;

        return h3d;
    }

}

LocalisePainter::LocalisePainter( bool beginDisplayHistory )
    : basicColor_(Qt::blue),
      isDisplayMultiHypothesis_(true)
{
    root_ = new osg::Group();
}

void
LocalisePainter::setCubicDescr( double length,
                                double width,
                                double height,
                                const orca::Frame3d &platformToGeometryTransform )
{
    cout<<"TRACE(localisepainter.cpp): platformToGeometryTransform: " << ifaceutil::toString(platformToGeometryTransform) << endl;

    cout<<"TRACE(localisepainter.cpp): " << __func__ << ": not implemented..." << endl;
    assert( false && "not implemented" );

//     assert( fabs(platformToGeometryTransform.o.r) < 1e-9 &&
//             fabs(platformToGeometryTransform.o.p) < 1e-9 &&
//             fabs(platformToGeometryTransform.o.y) < 1e-9 && "Not implemented!" );

//     platformNode_ = new osg::PositionAttitudeTransform;
//     platformNode_->setPosition( osg::Vec3( platformToGeometryTransform.p.x,
//                                            platformToGeometryTransform.p.y,
//                                            platformToGeometryTransform.p.z ) );

//     osg::ref_ptr<osg::Box> box = new osg::Box( osg::Vec3( 0, 0, 0 ), length, width, height );
//     osg::ref_ptr<osg::ShapeDrawable> drawable = new osg::ShapeDrawable(box.get());
//     drawable->setColor( orcaqgui3d::toVec4( currentColor_ ) );

//     osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//     geode->addDrawable( drawable.get() );

//     platformNode_->addChild( geode.get() );
}

void
LocalisePainter::setCylindricalDescr( double radius,
                                      double height,
                                      const orca::Frame3d &platformToGeometryTransform )
{
    cout<<"TRACE(localisepainter.cpp): platformToGeometryTransform: " << ifaceutil::toString(platformToGeometryTransform) << endl;

    assert( fabs(platformToGeometryTransform.o.r) < 1e-9 &&
            fabs(platformToGeometryTransform.o.p) < 1e-9 &&
            fabs(platformToGeometryTransform.o.y) < 1e-9 && "Not implemented!" );

    platformNode_ = new osg::Group;

    // Circle around the 'centre' of the platform
    osg::ref_ptr<osg::Geode> platformCentreGeode = new osg::Geode;
    platformCentreGeode->addDrawable( orcaqgui3d::drawEllipse( radius,
                                                               radius,
                                                               osg::Vec4(0,0,0,1) ).get() );
    platformNode_->addChild( platformCentreGeode.get() );

    platformGeometryNode_ = new osg::PositionAttitudeTransform;
    platformGeometryNode_->setPosition( osg::Vec3( platformToGeometryTransform.p.x,
                                                   platformToGeometryTransform.p.y,
                                                   platformToGeometryTransform.p.z ) );

    // Cylinder for the body
    osg::Vec4 color = orcaqgui3d::toVec4( currentColor_ );
    platformGeometryNode_->addChild( orcaqgui3d::drawCylinder( height, radius, color ).get() );

    // A 'vee' on top to mark orientation
    osg::ref_ptr<osg::Geode> veeGeode = new osg::Geode;
    veeGeode->addDrawable( orcaqgui3d::drawVee( radius, osg::Vec4(0,0,0,1), height/2.0 ).get() );
    platformGeometryNode_->addChild( veeGeode.get() );

    platformNode_->addChild( platformGeometryNode_.get() );
}

void
LocalisePainter::clear()
{
    cout<<"TRACE(localisepainter.cpp): TODO: "<<__func__ << endl;
}

void 
LocalisePainter::setData( const orca::Localise2dData& data )
{    
    // Clear
    root_->removeChildren( 0, root_->getNumChildren() );

    // Paint outselves a fixed height off the ground
    // (note: there's a similar hack in localise2delement.h)
    const double Z_OFFSET = 0.35;

    // And re-set
    if ( data.hypotheses.size() != 0 )
    {
        if ( isDisplayMultiHypothesis_ )
        {
            for ( unsigned int i=0; i<data.hypotheses.size(); ++i )
            {
                paintHypothesis( projectTo3d( data.hypotheses[i], Z_OFFSET ) );
            }
        }
        else
        {
            paintHypothesis( projectTo3d( orcaobj::mlHypothesis( data ), Z_OFFSET ) );
        }
    }
}

void 
LocalisePainter::setData( const orca::Localise3dData& data )
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
LocalisePainter::paintHypothesis( const orca::Pose3dHypothesis &hypothesis )
{
    if ( !platformNode_ ) return;

//    const float weight = hypothesis.weight;
    const orca::Frame3d      &mean = hypothesis.mean;
//    const orca::Covariance2d &cov  = hypothesis.cov;

    osg::ref_ptr<osg::PositionAttitudeTransform> pos = 
        getPositionAttitudeTransform( mean.p.x, mean.p.y, mean.p.z,
                                      mean.o.r, mean.o.p, mean.o.y );
    pos->addChild( platformNode_.get() );
    root_->addChild( pos.get() );

    // TODO: draw uncertainty.
}

void LocalisePainter::setColor( QColor color )
{
    basicColor_ = color;
    currentColor_ = color;

    cout<<"TRACE(localisepainter.cpp): TODO: also need to modify the scenegraph..." << endl;
}
    
void LocalisePainter::setFocus( bool inFocus )
{
    //cout << "TRACE(localisepainter.pp): inFocus is " << inFocus << endl;
    if (!inFocus) {
        currentColor_=Qt::gray;
    } else {
        currentColor_=basicColor_;
    }
}

    
}
