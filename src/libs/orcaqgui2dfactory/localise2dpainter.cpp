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
#include <hydroutil/mathdefs.h>
#include <hydroqgui/hydroqgui.h>
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
      isDisplayMultiHypothesis_(true)
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

//    cout<<"TRACE(localise2dpainter.cpp): got data: " << orcaice::toString(data) << endl;

    // should we keep history even if not displaying?
    if ( isDisplayHistory_ ) {
        if ( data.hypotheses.size() > 0 )
        {
            const orca::Pose2dHypothesis &h = orcaice::mlHypothesis( data );
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
        ScopedSaver translateSaver(p);
        
        // translate to mean+origin
        p->translate( mean.p.x+originX_, mean.p.y+originY_ );

        QColor color;
        if (useTransparency_) {
            color = getTransparentVersion( currentColor_, weight );
        } else {
            color = currentColor_;
        }

        // Need to get the world matrix before we rotate
        QMatrix m2win = p->worldMatrix();
        const float minLength = 15.0/m2win.m11();
        const float lineThickness = 2.0/m2win.m11();
        
        // Rotate to draw the platform correctly
        {
            ScopedSaver rotateSaver(p);
            p->rotate( RAD2DEG( mean.o ) + originRot_ );
            if (platformType_ == PlatformTypeCylindrical)
                paintCylindricalPlatformPose( p, color, radius_, weight, minLength, lineThickness  );
            else
                paintCubicPlatformPose( p, color, length_, width_, weight, minLength, lineThickness );
            paintUncertaintyWedge( p, color, cov.tt, minLength*3.0, lineThickness );
        }
        paintCovarianceEllipse( p, color, cov.xx, cov.xy, cov.yy, lineThickness );
    }
}

void 
Localise2dPainter::paint( QPainter *painter, int z )
{
    if ( !isDataAvailable_ ) return;
    
    if ( z == Z_POSE-2 && isDisplayHistory_ ) {
        history_.paint( painter, currentColor_ );
    }
    
    if ( z == Z_POSE ) 
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
                paintHypothesis( painter, orcaice::mlHypothesis( data_ ) );
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
    //cout << "TRACE(localise2dpainter.pp): inFocus is " << inFocus << endl;
    if (!inFocus) {
        currentColor_=Qt::gray;
    } else {
        currentColor_=basicColor_;
    }
}

    
