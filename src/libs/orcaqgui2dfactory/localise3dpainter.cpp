/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
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
#include "localise3dpainter.h"

using namespace std;
using namespace orcaqgui2d;

Localise3dPainter::Localise3dPainter( bool beginDisplayHistory )
    : length_(0.5), 
      width_(0.3), 
      radius_(0.25),
      platformType_(PlatformTypeCubic),
      isDataAvailable_(false),
      basicColor_(Qt::blue),
      isDisplayHistory_(beginDisplayHistory),
      isDisplayMultiHypothesis_(true)
{
}


void
Localise3dPainter::clear()
{
    isDataAvailable_ = false;
}

void 
Localise3dPainter::setData( const orca::Localise3dData& data )
{    
    data_ = data;
    isDataAvailable_ = true;

//    cout<<"TRACE(localise2dpainter.cpp): got data: " << orcaice::toString(data) << endl;

    // should we keep history even if not displaying?
    if ( isDisplayHistory_ ) {
        if ( data.hypotheses.size() > 0 )
        {
            const orca::Pose3dHypothesis &h = orcaice::mlHypothesis( data );
            history_.addPoint( h.mean.p.x, h.mean.p.y );
        }
        else
            throw hydroqgui::Exception( ERROR_INFO, "Localise3dPainter::setData(): Localise3dData had zero hypotheses" );
    }

}

void 
Localise3dPainter::paintHypothesis( QPainter* p, const orca::Pose3dHypothesis &hypothesis )
{
    float weight = hypothesis.weight;

    // Don't bother painting _really_ unlikely hypotheses
    // if ( weight < 0.02 ) continue;

    const orca::Frame3d      &mean = hypothesis.mean;
    const orca::Covariance3d &cov  = hypothesis.cov;

    // Translate to where the hypothesis is at
    {
        ScopedSaver translateSaver(p);
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
            p->rotate( RAD2DEG( mean.o.y ) + originRot_ );
            if (platformType_ == PlatformTypeCylindrical)
                paintCylindricalPlatformPose( p, color, radius_, weight, minLength, lineThickness  );
            else
                paintCubicPlatformPose( p, color, length_, width_, weight, minLength, lineThickness );
            paintUncertaintyWedge( p, color, cov.aa, minLength*3.0, lineThickness );
        }    
        paintCovarianceEllipse( p, color, cov.xx, cov.xy, cov.yy, lineThickness );
    }
}

void 
Localise3dPainter::paint( QPainter *painter, int z )
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
    
void Localise3dPainter::setColor( QColor color )
{
    basicColor_ = color;
    currentColor_ = color; 
}
    
void Localise3dPainter::setFocus( bool inFocus )
{
    //cout << "TRACE(localise2dpainter.pp): inFocus is " << inFocus << endl;
    if (!inFocus) {
        currentColor_=Qt::gray;
    } else {
        currentColor_=basicColor_;
    }
}

    
