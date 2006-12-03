/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <QFile>
#include <QTextStream>

#include <orcaice/orcaice.h>
#include <orcaobj/mathdefs.h>

#include <orcaqgui/features.h>
#include <orcaqgui/ihumanmanager.h>

#include "featuremap2dpainter.h"
#include <orcaqgui2d/paintutils.h>

using namespace std;
using namespace orca;
using namespace orcaice;

namespace orcaqgui {

FeatureMap2dPainter::FeatureMap2dPainter()
    : data_( NULL ),
      displayFeatureNumbers_(true),
      displayUncertainty_(true)
{
}

FeatureMap2dPainter::~FeatureMap2dPainter()
{
}

void 
FeatureMap2dPainter::clear()
{
}


void 
FeatureMap2dPainter::setData( const orca::FeatureMap2dDataPtr &data )
{
    assert( data != 0 );

    //cout<<"TRACE(featuremap2dpainter.cpp): got data: " << orcaice::toString(data) << endl;

    data_ = data;
}

void FeatureMap2dPainter::paint( QPainter *painter, const int z )
{
    if ( z != orcaqgui::Z_SLAM_MAP ) return;

    if (data_ == 0) return;

    const float boxWidth = 0.2;

    QColor color;

    for ( unsigned int i=0; i < data_->features.size(); i++ )
    {
        if ( data_->features[i]->ice_isA( "::orca::CartesianPointFeature2d" ) )
        {
            const orca::CartesianPointFeature2d *f = 
                dynamic_cast<const orca::CartesianPointFeature2d*>(&(*(data_->features[i])));
            assert( f != NULL );

            // draw a little square on the mean, with weight proportional to pExists.
            QPen pen(featureColour(f->type));
            double widthFactor = f->pExists-0.5;
            double newWidth =  0.1 * widthFactor;
            pen.setWidthF( newWidth );
            painter->setPen( pen );
            painter->drawRect( QRectF( f->p.x-boxWidth/2, f->p.y-boxWidth/2, boxWidth, boxWidth ) );

            // Draw the covariance ellipse and feature number
            painter->save();
            {
                painter->translate( f->p.x, f->p.y );
                if ( displayUncertainty_ )
                {
                    //cout<<"TRACE(featuremap2dpainter.cpp): painting: " << f->c.xx << ","<<f->c.xy<<","<<f->c.yy << endl;

                    paintCovarianceEllipse( painter,
                                            featureColour(f->type),
                                            f->c.xx,
                                            f->c.xy,
                                            f->c.yy );
                }

                if ( displayFeatureNumbers_ )
                {
                    painter->setPen(getTransparentVersion(featureColour(f->type),0.3));
                    painter->setFont( QFont("Helvetica [Cronyx]", 12) );
                    const double offset = 0.3;
                    QMatrix m = painter->matrix();  // this is m2win matrix
                    QPointF labelPos = QPointF(offset,offset) * m;       // x-label position in window cs
                    painter->setMatrix( QMatrix() );
                    painter->drawText( labelPos, QString::number(i) );
                }
            }
            painter->restore();
        }
        else
        {
            assert( false );
        }
    }
}

int FeatureMap2dPainter::saveMap( const QString fileName, IHumanManager *humanManager ) const
{
    cout << "INFO(featuremap2dpainter.cpp): saveMap, fileName is " << fileName.toStdString() << endl;
    
    int size = data_->features.size();
    if (size==0) return -2;
    
    FILE *f = fopen( fileName.toStdString().c_str(), "w" );
    if (!f)
    {
        humanManager->showBoxMsg(Error, "Cannot create file " + fileName );
        cout << "ERROR(featuremap2dpainter.cpp): cannot create file" <<endl;
        return -1;
    } 
    else 
    {
        orcaice::saveToFile( data_, f );
        humanManager->showStatusMsg(Information, "Saving feature map to " + fileName );
        fclose( f );
    }
    return 0;
}

}
