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
#include <QPainter>
#include <QString>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <hydroqguielementutil/paintutils.h>
#include "polarfeature2dpainter.h"
#include <orcaqguielementutil/featurecolours.h>

using namespace std;

namespace orcaqgui2d {

namespace {
    const double BOX_WIDTH = 0.750;
    const double LINE_WIDTH_MAX = BOX_WIDTH/2.0;
}

PolarFeature2dPainter::PolarFeature2dPainter()
    : isOffsetSet_(false)
{
}

void 
PolarFeature2dPainter::clear()
{
}

void 
PolarFeature2dPainter::setData( const orca::PolarFeature2dData &data )
{
    assert( isOffsetSet_ );

    featureData_ = data;

    //
    // Deal with isUpsideDown_ by modifying the featureData_
    //
    if ( isUpsideDown_ )
    {
        for ( unsigned int i=0; i < featureData_.features.size(); i++ )
        {
            orca::SinglePolarFeature2d &ftr = *(featureData_.features[i]);

            {
                orca::PointPolarFeature2d *f = dynamic_cast<orca::PointPolarFeature2d *>(&ftr);
                if ( f != NULL )
                {
                    f->p.o = -f->p.o;            
                    continue;
                }
            }
            {
                orca::PosePolarFeature2d *f = dynamic_cast<orca::PosePolarFeature2d *>(&ftr);
                if ( f != NULL )
                {
                    f->p.o = -f->p.o;
                    f->orientation = -f->orientation;
                    continue;
                }
            }
            {
                orca::LinePolarFeature2d *f = dynamic_cast<orca::LinePolarFeature2d *>(&ftr);
                if ( f != NULL )
                {
                    f->start.o = -f->start.o;
                    f->end.o   = -f->end.o;
                    continue;
                }
            }
        }
    }
}

void
PolarFeature2dPainter::setOffset( orca::Frame3d &offset )
{
    offsetX_   = offset.p.x;
    offsetY_   = offset.p.y;
    offsetYaw_ = offset.o.y;
    offsetPitch_ = offset.o.p;
    isUpsideDown_ = false;

    // for 2D display, the only thing we know how to paint
    // is a sensor mounted horizontally, either right-way-up or upside-down
    if ( !( offset.o.r == 0.0 || NEAR( offset.o.r,M_PI,0.01 ) ) )
    {
        stringstream ss;
        ss << "PolarFeature2dPainter::setOffset(): Can only properly deal with (possibly-flipped) horizontal lasers.  Offset: " << orcaobj::toString(offset);
        throw gbxsickacfr::gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    if ( NEAR( offset.o.r,M_PI,0.01 ) ) {
        isUpsideDown_ = true;
    }
    
    // Don't really know how to deal with non-zero values here...
    if ( offset.p.z != 0.0 )
    {
        stringstream ss;
        ss << "PolarFeature2dPainter::setOffset(): Cannot properly deal with non-zero z.  Offset: " << orcaobj::toString(offset);
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );
    }

    // Can't deal with non-zero pitch
    if ( offsetPitch_ != 0.0 )
    {
        stringstream ss;
        ss << "PolarFeature2dPainter::setOffset(): Cannot properly deal with non-zero pitch.  Offset: " << orcaobj::toString(offset);
        throw hydroqgui::Exception( ERROR_INFO, ss.str() );        
    }

    isOffsetSet_ = true;
}

void
drawPointFeature( QPainter *painter, const orca::PointPolarFeature2d &f )
{
    painter->save();
    {
        painter->rotate( (int)floor(RAD2DEG(f.p.o)) );
        painter->translate( f.p.r, 0.0 );
        
        // Not sure of a better way to represent both pFalsePositive and pTruePositive
        // with just one number...
        double lineWidth = LINE_WIDTH_MAX*( f.pTruePositive-f.pFalsePositive );
        painter->setPen( QPen( orcaqguielementutil::featureColour(f.type),lineWidth) );
        painter->drawRect( QRectF( -BOX_WIDTH/2.0, -BOX_WIDTH/2.0, BOX_WIDTH, BOX_WIDTH) );
    }
    painter->restore();
}

void
drawPoseFeature( QPainter *painter, const orca::PosePolarFeature2d &f )
{
    painter->save();
    {
        // painter->rotate( (int)floor(RAD2DEG(f.p.o)) );
        painter->rotate( RAD2DEG(f.p.o) );
        painter->translate( f.p.r, 0.0 );
        
        // Not sure of a better way to represent both pFalsePositive and pTruePositive
        // with just one number...
        double lineWidth = LINE_WIDTH_MAX*( f.pTruePositive-f.pFalsePositive );
        painter->setPen( QPen( orcaqguielementutil::featureColour(f.type),lineWidth) );
        painter->drawRect( QRectF( -BOX_WIDTH/2.0, -BOX_WIDTH/2.0, BOX_WIDTH, BOX_WIDTH) );

        painter->rotate( RAD2DEG( -f.p.o + f.orientation ) );
        painter->translate( BOX_WIDTH, 0.0 );
        hydroqguielementutil::paintArrow( painter );
    }
    painter->restore();
}

void
drawLineFeature( QPainter *painter, const orca::LinePolarFeature2d &f )
{
    painter->save();
    {
        double lineWidth = LINE_WIDTH_MAX*( f.pTruePositive-f.pFalsePositive );
        painter->setPen( QPen( orcaqguielementutil::featureColour(f.type), lineWidth ) );

        double x1 = f.start.r*cos(f.start.o);
        double y1 = f.start.r*sin(f.start.o);
        double x2 = f.end.r*cos(f.end.o);
        double y2 = f.end.r*sin(f.end.o);

        // Move to the first point, and rotate the x-axis parallel to the line
        painter->translate( x1, y1 );
        painter->rotate( atan2(y2-y1,x2-x1)*180.0/M_PI );

        double len = hypot(y2-y1,x2-x1);

        painter->drawLine( QLineF( 0,  BOX_WIDTH/2, len,  BOX_WIDTH/2 ) );
        painter->drawLine( QLineF( 0, -BOX_WIDTH/2, len, -BOX_WIDTH/2 ) );

        if ( f.startSighted )
            painter->drawLine( QLineF( 0, -BOX_WIDTH/2, 0, BOX_WIDTH/2 ) );
        if ( f.endSighted )
            painter->drawLine( QLineF( len, -BOX_WIDTH/2, len, BOX_WIDTH/2 ) );
    }
    painter->restore();
}

void
PolarFeature2dPainter::paint( QPainter *painter, int z )
{
    if ( z != hydroqguielementutil::Z_LASER_FEATURES ) return;

    // handle sensor offset
    painter->translate(offsetX_,offsetY_);
    painter->rotate( offsetYaw_*180.0/M_PI );    

    painter->setBrush(QBrush());

    for ( unsigned int i=0; i<featureData_.features.size(); ++i )
    {
        const orca::SinglePolarFeature2d &ftr = *(featureData_.features[i]);

        {
            const orca::PointPolarFeature2d *f = dynamic_cast<const orca::PointPolarFeature2d *>(&ftr);
            if ( f != NULL )
            {
                drawPointFeature( painter, *f );
                continue;
            }
        }
        {
            const orca::PosePolarFeature2d *f = dynamic_cast<const orca::PosePolarFeature2d *>(&ftr);
            if ( f != NULL )
            {
                drawPoseFeature( painter, *f );
                continue;
            }
        }
        {
            const orca::LinePolarFeature2d *f = dynamic_cast<const orca::LinePolarFeature2d *>(&ftr);
            if ( f != NULL )
            {
                drawLineFeature( painter, *f );
                continue;
            }
        }
    }
}

}
