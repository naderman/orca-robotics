/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_ZOOM_WIDGET_H
#define ORCAGUI_ZOOM_WIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QMatrix>

namespace orcaqgui
{

/*!
  \brief A widget with zoom and pan capabilities.
  \author Alex Makarenko
*/
class ZoomWidget : public QWidget
{
    Q_OBJECT
public:
    ZoomWidget( QWidget* parent=0 );

    QMatrix matrix() const { return wm_; };

signals:
    // Use this signal to broadcast a change in the world matrix.
    void matrixChanged( const QMatrix & );
    // Use this signal to broadcast a change in the size of the widget.
    void windowSizeChanged( const QSize & );

public slots:
    // view manipulation
    void zoomIn( const double & z=4.0/3.0 ) {
        zoomView(z);
    };
    void zoomOut( const double & z=3.0/4.0 ) {
        zoomView(z);
    };
    void zoomWorldSize( const QSizeF & worldSize );
    void zoomWorldSize( const QSizeF & worldSize, const QPointF & pcenter );
    
    void zoomPixelSize( const QSizeF & pixSize ) {
        setZoom( 1.0/pixSize.width() );
    };

    void moveUp() {
        moveView( QPointF(0.0,25.0) );
    };
    void moveDown() {
        moveView( QPointF(0.0,-25.0) );
    };
    void moveLeft() {
        moveView( QPointF(25.0,0.0) );
    };
    void moveRight() {
        moveView( QPointF(-25.0,0.0) );
    };
    
    // Responds to an outside request to broadcast the current world matrix.
    void transmitMatrix();

protected:

    // world-to-window transformation matrix
    QMatrix wm_;

    // view manipulation utilities
    void setZoom( const double & );
    void zoomView( const double & );
    
    // Translates the widget view by d(x,y), where d = p2 - p1
    //   p1 - original coord. of widget center
    //   p2 - final coord. of widget center
    void moveView( const QPointF & );
    
    // given a point in WORLD CS, position it in the center of the window
    void centerView( const QPointF & );

    void resizeEvent( QResizeEvent* );

};

} // namespace

#endif
