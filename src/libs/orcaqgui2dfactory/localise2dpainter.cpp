/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>

#include <QPainter>
#include <orcaobj/orcaobj.h>
#include <orcaqgui/exceptions.h>
#include "localise2dpainter.h"

using namespace orca; 
using namespace std;
using namespace orcaqgui2d;

Localise2dPainter::Localise2dPainter( bool beginDisplayHistory )
    : isDataAvailable_(false),
      basicColor_(Qt::blue),
      isDisplayHistory_(beginDisplayHistory),
      isDisplayMultiHypothesis_(true),
      xOff_(0.0),
      yOff_(0.0),
      thetaOff_(0.0)
{
}

void
Localise2dPainter::clear()
{
    isDataAvailable_ = false;
}

void 
Localise2dPainter::setOffsets( double xOff, double yOff, double thetaOff )
{
    xOff_ = xOff;
    yOff_ = yOff;
    thetaOff_ = thetaOff;
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
            const Pose2dHypothesis &h = orcaice::mlHypothesis( data );
            history_.addPoint( h.mean.p.x, h.mean.p.y );
        }
        else
            throw orcaqgui::Exception( "Localise2dPainter::setData(): Localise2dData had zero hypotheses" );
    }

}

void 
Localise2dPainter::paintHypothesis( QPainter* p, const orca::Pose2dHypothesis &hypothesis )
{
    float weight = hypothesis.weight;

    // Don't bother painting _really_ unlikely hypotheses
    // if ( weight < 0.02 ) continue;

    const Frame2d      &mean = hypothesis.mean;
    const Covariance2d &cov  = hypothesis.cov;

    // Translate to where the hypothesis is at
    {
        ScopedSaver translateSaver(p);
        
        // CS transformation: localise CS to global CS
        // (they are usually the same)
        p->translate(xOff_,yOff_);
        p->rotate(thetaOff_);
        
        // translate to mean
        p->translate( mean.p.x, mean.p.y );

        QColor color;
        if (useTransparency_) {
            color = getTransparentVersion( currentColor_, weight );
        } else {
            color = currentColor_;
        }

        // Need to get the world matrix before we rotate
        QMatrix m2win = p->worldMatrix();
        {            
            // Rotate to draw the platform correctly
            ScopedSaver rotateSaver(p);
            
            p->rotate( RAD2DEG( mean.o ) );
            paintPlatformPose( m2win,
                               p, 
                               color,
                               weight );
        }

        paintUncertaintyInfo( m2win,
                              p,
                              color,
                              mean.o,
                              cov.xx,
                              cov.xy,
                              cov.yy,
                              cov.tt );
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

    
