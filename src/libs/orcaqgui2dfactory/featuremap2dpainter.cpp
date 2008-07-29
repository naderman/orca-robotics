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

#include <QFile>
#include <QTextStream>

#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <orcaqguielementutil/featurecolours.h>
#include <hydroqgui/hydroqgui.h>
#include <hydroqguielementutil/paintutils.h>
#include "featuremap2dpainter.h"

using namespace std;

namespace orcaqgui2d {

FeatureMap2dPainter::FeatureMap2dPainter()
    : displayFeatureNumbers_(false),
      displayUncertainty_(false),
      isDisplay_(true),
      isFlashing_(false),
      isFlashOn_(false)
{
}

void 
FeatureMap2dPainter::setData( const orca::FeatureMap2dData &data )
{
    // cout<<"TRACE(featuremap2dpainter.cpp): got data: " << orcaobj::toString(data) << endl;

    data_ = data;
}

void
paintFeatureNum( QPainter *painter, int featureType, int featureNum, bool useTransparency )
{
    if ( useTransparency ) {
        painter->setPen( hydroqguielementutil::getTransparentVersion(orcaqguielementutil::featureColour(featureType),0.3));
    } else {
        painter->setPen(orcaqguielementutil::featureColour(featureType));
    }
    painter->setFont( QFont("Helvetica [Cronyx]", 12) );
    const double offset = 0.3;
    QMatrix m = painter->matrix();  // this is m2win matrix
    QPointF labelPos = QPointF(offset,offset) * m;       // x-label position in window cs
    painter->setMatrix( QMatrix() );
    painter->drawText( labelPos, QString::number(featureNum) );
}

void 
FeatureMap2dPainter::paintPointFeature( QPainter *painter, 
                                        int featureType,
                                        int featureNum,
                                        double probExists,
                                        double centreX,
                                        double centreY,
                                        double covXX,
                                        double covXY,
                                        double covYY )
{
    // draw a little square on the mean, with weight proportional to pExists.
    const float boxWidth = 0.2;
    QPen pen(orcaqguielementutil::featureColour(featureType));
    double newWidth =  MAX(0.1, 0.1 * (probExists-0.2));
    pen.setWidthF( newWidth );
    painter->setPen( pen );
    painter->drawRect( QRectF( centreX-boxWidth/2, centreY-boxWidth/2, boxWidth, boxWidth ) );

    // Draw the covariance ellipse and feature number
    painter->save();
    {
        painter->translate( centreX, centreY );
        if ( displayUncertainty_ )
        {
            //cout<<"TRACE(featuremap2dpainter.cpp): painting: " << f.c.xx << ","<<f.c.xy<<","<<f.c.yy << endl;

//             QMatrix m2win = painter->worldMatrix();
//             const float lineThickness = 2.0/m2win.m11();
            hydroqguielementutil::paintCovarianceEllipse( painter,
                                                    covXX,
                                                    covXY,
                                                    covYY );
        }

        // Numbers
        if ( displayFeatureNumbers_ )
            paintFeatureNum( painter, featureType, featureNum, useTransparency_ );
    }
    painter->restore();
}

void 
FeatureMap2dPainter::paintPointFeature( QPainter *painter, 
                                        const orca::CartesianPointFeature2d &f,
                                        int featureNum )
{
    paintPointFeature( painter, f.type, featureNum, f.pExists,
                       f.p.x, f.p.y, f.c.xx, f.c.xy, f.c.yy );
}

void
checkForDodgyEndpoints( const orca::CartesianLineFeature2d &f )
{
    if ( isnan( f.start.x ) ||
         isnan( f.start.y ) ||
         isnan( f.end.x ) ||
         isnan( f.end.y ) )
    {
        stringstream ss;
        ss << "NaN found in start/end of line: " << orcaobj::toString(f);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

void 
FeatureMap2dPainter::paintLineFeature( QPainter *painter,
                                       const orca::CartesianLineFeature2d &f,
                                       int featureNum )
{
    checkForDodgyEndpoints( f );

    // draw the line
    QPen pen(orcaqguielementutil::featureColour(f.type));
    double newWidth =  MAX(0.1, 0.2 * (f.pExists-0.2));
    pen.setWidthF( newWidth );
    painter->setPen( pen );
    painter->drawLine( QLineF( f.start.x, f.start.y, f.end.x, f.end.y ) );

    // Have the endpoints been seen?  Draw boxes if they have.
    const float boxWidth = 0.2;
    if ( f.startSighted )
        painter->drawRect( QRectF( f.start.x-boxWidth/2, f.start.y-boxWidth/2, boxWidth, boxWidth ) );
    if ( f.endSighted )
        painter->drawRect( QRectF( f.end.x-boxWidth/2, f.end.y-boxWidth/2, boxWidth, boxWidth ) );

    // Need to represent the uncertainty and direction
    painter->save();
    {
        double midpointX = (f.start.x+f.end.x)/2.0;
        double midpointY = (f.start.y+f.end.y)/2.0;
        painter->translate( midpointX, midpointY );
        
        painter->save();
        {
            // The direction from start point to end point.
            double angleStoE = atan2( f.end.y-f.start.y, f.end.x-f.start.x );

            // Face perpendicular to the line, 90deg right of of StoE
            painter->rotate( RAD2DEG(angleStoE) - 90 );

            if ( displayUncertainty_ )
            {
                // rho uncertainty: lines at the ends
                double halfLineLength = hypot( f.end.y-midpointY, f.end.x-midpointX );
                double uncertaintyLength = 3*f.c.xx;
                painter->drawLine( QLineF( -uncertaintyLength, -halfLineLength,
                                           uncertaintyLength, -halfLineLength ) );
                painter->drawLine( QLineF( -uncertaintyLength, halfLineLength,
                                           uncertaintyLength, halfLineLength ) );

                // alpha uncertainty: a wedge on the back (non-visible) side of the line
                painter->setPen( QPen(orcaqguielementutil::featureColour(f.type)) );
                const double length = 2.0;
                hydroqguielementutil::paintUncertaintyWedge( painter,
                                                       f.c.yy,
                                                       length );
            }
            else
            {
                // A little line on the back (non-visible) side of the line
                const double BACK_MARKER_LENGTH = 0.3;
                painter->drawLine( QLineF( 0.0, 0.0, BACK_MARKER_LENGTH, 0.0 ) );
            }
        }
        painter->restore();

        // Numbers
        if ( displayFeatureNumbers_ )
            paintFeatureNum( painter, f.type, featureNum, useTransparency_ );
    }
    painter->restore();
}

void 
FeatureMap2dPainter::paintPoseFeature( QPainter *painter, 
                                       const orca::CartesianPoseFeature2d &f,
                                       int featureNum )
{
    paintPointFeature( painter, f.type, featureNum, f.pExists,
                       f.p.p.x, f.p.p.y, f.c.xx, f.c.xy, f.c.yy );

    painter->save();
    {
        painter->translate( f.p.p.x, f.p.p.y );
        painter->rotate( RAD2DEG(f.p.o) );
        if ( displayUncertainty_ )
        {
            const float length = 2.0;
            painter->setPen( QPen(orcaqguielementutil::featureColour(f.type)) );
            hydroqguielementutil::paintUncertaintyWedge( painter,
                                                   f.c.tt,
                                                   length );
        }
        else
        {
            // Draw an arrow
            hydroqguielementutil::paintArrow( painter );
        }
    }
    painter->restore();
}

void FeatureMap2dPainter::paint( QPainter *painter, const int z )
{
    if ( !isDisplay_ ) return;
    if ( z != hydroqguielementutil::Z_SLAM_MAP ) return;
    if ( isFlashing_ )
    {
        isFlashOn_ = !isFlashOn_;
        if ( !isFlashOn_ ) return;
    }

    QColor color;

    // Paint all lines first, so the less-obvious point features are on top.

    for ( unsigned int i=0; i < data_.features.size(); i++ )
    {
        if ( data_.features[i]->ice_isA( "::orca::CartesianLineFeature2d" ) )
        {
            const orca::CartesianLineFeature2d *f = 
                dynamic_cast<const orca::CartesianLineFeature2d*>(&(*(data_.features[i])));
            assert( f != NULL );
            assert( f->type == orca::feature::LINE || f->type == orca::feature::LINE+10 );
            
            paintLineFeature( painter, *f, i );
        }
    }
    for ( unsigned int i=0; i < data_.features.size(); i++ )
    {
        if ( data_.features[i]->ice_isA( "::orca::CartesianPointFeature2d" ) )
        {
            const orca::CartesianPointFeature2d *f = 
                dynamic_cast<const orca::CartesianPointFeature2d*>(&(*(data_.features[i])));
            assert( f != NULL );

            paintPointFeature( painter, *f, i );
        }
        else if ( data_.features[i]->ice_isA( "::orca::CartesianPoseFeature2d" ) )
        {
            const orca::CartesianPoseFeature2d *f = 
                dynamic_cast<const orca::CartesianPoseFeature2d*>(&(*(data_.features[i])));
            assert( f != NULL );

            paintPoseFeature( painter, *f, i );
        }
    }
}

int FeatureMap2dPainter::saveMap( const QString fileName, hydroqguielementutil::IHumanManager *humanManager ) const
{
    cout << "INFO(featuremap2dpainter.cpp): saveMap, fileName is " << fileName.toStdString() << endl;
    
    int size = data_.features.size();
    if (size==0) return -2;
    
    FILE *f = fopen( fileName.toStdString().c_str(), "w" );
    if (!f)
    {
        humanManager->showDialogError( "Cannot create file " + fileName );
        cout << "ERROR(featuremap2dpainter.cpp): cannot create file" <<endl;
        return -1;
    } 
    else 
    {
        orcaobj::saveToFile( data_, f );
        humanManager->showStatusMsg(hydroqguielementutil::IHumanManager::Information, "Saving feature map to " + fileName );
        fclose( f );
    }
    return 0;
}

}
