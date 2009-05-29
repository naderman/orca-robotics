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
#include <orcaice/orcaice.h>
#include <gbxutilacfr/mathdefs.h>
#include <hydroqgui/exceptions.h>
#include "localise2dpainter.h"

using namespace std;
using namespace orcaqgui2d;

Localise2dPainter::Localise2dPainter( bool beginDisplayHistory )
    : length_(1e-3),
      width_(1e-3),
      radius_(1e-3),
      platformType_(PlatformTypeCubic),
      isDataAvailable_(false),
      basicColor_(Qt::blue),
      isDisplayHistory_(beginDisplayHistory),
      isDisplayMultiHypothesis_(true),
      isInFocus_(false)
{
}

void
Localise2dPainter::clear()
{
    isDataAvailable_ = false;
}

void 
Localise2dPainter::setData( const orca::Localise2dData& data )
{    
    data_ = data;
    isDataAvailable_ = true;

//    cout<<"TRACE(localise2dpainter.cpp): got data: " << orcaobj::toString(data) << endl;

    // should we keep history even if not displaying?
    if ( isDisplayHistory_ ) {
        if ( data.hypotheses.size() > 0 )
        {
            const orca::Pose2dHypothesis &h = orcaobj::mlHypothesis( data );
            history_.addPoint( h.mean.p.x, h.mean.p.y );
        }
        else
            throw hydroqgui::Exception( ERROR_INFO, "Localise2dPainter::setData(): Localise2dData had zero hypotheses" );
    }

}

void 
Localise2dPainter::paintHypothesis( QPainter* p, const orca::Pose2dHypothesis &hypothesis )
{
    float weight = hypothesis.weight;

    // Don't bother painting _really_ unlikely hypotheses
    // if ( weight < 0.02 ) continue;

    const orca::Frame2d      &mean = hypothesis.mean;
    const orca::Covariance2d &cov  = hypothesis.cov;

    // Translate to where the hypothesis is at
    {
        hydroqguielementutil::ScopedSaver translateSaver(p);
        
        // translate to mean+origin
        p->translate( mean.p.x+originX_, mean.p.y+originY_ );

        QColor color;
        if (useTransparency_) {
            color = hydroqguielementutil::getTransparentVersion( currentColor_, weight );
        } else {
            color = currentColor_;
        }

        double pixelsPerMetre = hydroqguielementutil::worldMatrixScale( p );
        const float minLengthInPixels = 15.0;
        const float minLengthInMetres = minLengthInPixels/pixelsPerMetre;
        const float lineThicknessInPixels = 2.0;
        const float lineThicknessInMetres = lineThicknessInPixels/pixelsPerMetre;
        assert( minLengthInMetres > 0 && !isinf(minLengthInMetres) );
        assert( lineThicknessInMetres > 0 && !isinf(lineThicknessInMetres) );
        
        // Rotate to draw the platform correctly
        {
            hydroqguielementutil::ScopedSaver rotateSaver(p);
            p->rotate( RAD2DEG( mean.o ) + originRot_ );

            p->setPen( QPen( Qt::black, lineThicknessInMetres) );
            p->setBrush( color );

            if (platformType_ == PlatformTypeCylindrical)
            {
                if ( isInFocus_ )
                {
                    p->save();
                    p->setPen( QPen( Qt::black, lineThicknessInMetres*1.2 ) );
                    p->setBrush( hydroqguielementutil::getTransparentVersion( Qt::white, weight ) );
                    hydroqguielementutil::paintCylindricalPlatformPose( p, radius_*1.2, weight, minLengthInMetres*1.2  );
                    p->restore();
                    p->setPen( QPen( Qt::black, lineThicknessInMetres*1.2) );
                }
                hydroqguielementutil::paintCylindricalPlatformPose( p, radius_, weight, minLengthInMetres  );
            }
            else
            {
                hydroqguielementutil::paintCubicPlatformPose( p, length_, width_, weight, minLengthInMetres );
            }

            p->setPen( color );
            hydroqguielementutil::paintUncertaintyWedge( p, cov.tt, minLengthInMetres*3.0 );
        }
        p->setPen( color );
        hydroqguielementutil::paintCovarianceEllipse( p, cov.xx, cov.xy, cov.yy );
    }
}

void 
Localise2dPainter::paint( QPainter *painter, int z )
{
    if ( !isDataAvailable_ ) return;
    
    if ( z == hydroqguielementutil::Z_POSE-2 && isDisplayHistory_ ) {
        history_.paint( painter, currentColor_ );
    }
    
    if ( z == hydroqguielementutil::Z_POSE ) 
    {
        if ( data_.hypotheses.size() != 0 )
        {
            if ( isDisplayMultiHypothesis_ )
            {
                for ( unsigned int i=0; i<data_.hypotheses.size(); ++i )
                {
                    paintHypothesis( painter, data_.hypotheses[i] );
                }
            }
            else
            {
                paintHypothesis( painter, orcaobj::mlHypothesis( data_ ) );
            }
        }
    }
}
    
void Localise2dPainter::setColor( QColor color )
{
    basicColor_ = color;
    currentColor_ = color; 
}
    
void Localise2dPainter::setFocus( bool inFocus )
{
    isInFocus_ = inFocus;

//     if (!inFocus) {
//         currentColor_=Qt::gray;
//     } else {
        currentColor_=basicColor_;
//     }
}

    
