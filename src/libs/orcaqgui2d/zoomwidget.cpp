/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <cmath>
#include <QResizeEvent>

#include "zoomwidget.h"

using namespace orcaqgui;

ZoomWidget::ZoomWidget( QWidget* parent )
    : QWidget(parent)
{
}

void
ZoomWidget::transmitMatrix()
{
    emit matrixChanged( wm_ );
}

void
ZoomWidget::setZoom( const double & z )
{
    // set world matrix, reverse y direction
    wm_.setMatrix( z, 0.0, 0.0, -z, 0.0, height() );
    emit matrixChanged( wm_ );
}

void
ZoomWidget::zoomView( const double & z )
{
    if ( z == 1.0 )
        return;

    const double MAX_ZOOM_OUT = 1.0/64.0;
    const double MAX_ZOOM_IN = 64.0;
    QPoint pcenter;

    if ( wm_.m11()>=MAX_ZOOM_IN && z>1.0 )
        return;
    if ( wm_.m11()<=MAX_ZOOM_OUT && z<1.0 )
        return;

    // remember current center point in MAP CS
    pcenter = QPoint( width()/2,height()/2 ) * wm_.inverted();

    // scale the matrix
    wm_.scale( z, z );

    // put the center point back in the center
    centerView( pcenter );

    emit matrixChanged( wm_ );
}

void
ZoomWidget::moveView( const QPointF & d )
{
    if ( d.isNull() )
        return;

    QMatrix m;

    m = wm_.inverted();
    m.translate( -d.x(), -d.y() );
    wm_ = m.inverted();
    emit matrixChanged( wm_ );
}

void
ZoomWidget::centerView( const QPointF & pmap )
{
    QPointF pwin, pctr;

    // current WIN position of the point to be centered
    pwin = pmap * wm_;
    pctr = QPointF( width()/2.0, height()/2.0 );
    moveView( pctr - pwin );

    // current center of the window (in WIN cs)
}

void
ZoomWidget::zoomWorldSize( const QSizeF & worldSize )
{
    double zx = (double)width()/worldSize.width();
    double zy = (double)height()/worldSize.height();

    // poor man's min(zx,zy)
    double z = (zx<zy) ? zx : zy;

    setZoom( z );
}

void
ZoomWidget::zoomWorldSize( const QSizeF & worldSize, const QPointF & pcenter )
{
    // first reset zoom
    zoomWorldSize( worldSize);

    // center on the given point
    centerView( pcenter );
}

void
ZoomWidget::resizeEvent( QResizeEvent* e ) 
{
    // send a signal so that others (QredOgMap) can also resize their internal storage
    emit windowSizeChanged( e->size() );
}
