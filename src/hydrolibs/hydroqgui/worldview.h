/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROQGUI_WORLDVIEW_H
#define HYDROQGUI_WORLDVIEW_H

#include <QString>
#include <QLabel>

#include <hydroqgui/zoomwidget.h>
#include <hydroqgui/guielementset.h>
#include <hydroqgui/platformfocusmanager.h>
#include <hydroqgui/coordinateframemanager.h>
#include <hydroqguielementutil/mouseeventmanager.h>
#include <hydroqguielementutil/ihumanmanager.h>


namespace hydroqgui {

class PlatformCSFinder;

class XYDisplay : public QLabel
{
public:
    XYDisplay();
    void setCoordinates( double x, double y );
private:
};

//!
//! @brief Displays a set of GuiElements in a geometrically-correct way
//!
class WorldView : public hydroqgui::ZoomWidget
{
   Q_OBJECT
public:

    struct Config {
        Config()
            : initWithTransparency(false),
              initWithAntiAliasing(false),
              initialWorldWidth(20.0)
            {
            }

        bool initWithTransparency;
        bool initWithAntiAliasing;

        // Sets initial zoom settings
        double initialWorldWidth;
    };
    
    WorldView(  PlatformCSFinder                          &platformCSFinder,
                ::hydroqguielementutil::MouseEventManager &mouseEventManager,
                GuiElementSet                             &guiElementSet,
                CoordinateFrameManager                    &coordinateFrameManager,
                ::hydroqguielementutil::IHumanManager     &humanManager,
                PlatformFocusManager                      &platformFocusManager,
                int                                        displayRefreshTime,
                const Config                              &cfg,
                QWidget                                   *parent = NULL );

    virtual QSize sizeHint() const { return QSize( 400, 400 ); }
  
public slots:
    // abstract zoom functions
    void zoomFitWin( double width=20.0 );
    void zoomAllPix();
    void setAntiAliasing(bool antiAliasing);
    void setUseTransparency(bool useTransparency);

private slots:

    // this function gives each guielement a chance to update its internal
    // state and then issues a paint event (which will result in all guielements
    // getting a chance to repaint themselves).
    // it is driven off a clock.
    void reDisplay();

private:

    void setupInterface( const Config &cfg );

    // handles distribution of mouse events to GuiElements
    ::hydroqguielementutil::MouseEventManager &mouseEventManager_;

    GuiElementSet          &guiElementSet_;

    // finds coord system of a platform
    const PlatformCSFinder &platformCSFinder_;
    CoordinateFrameManager &coordinateFrameManager_;

    ::hydroqguielementutil::IHumanManager &humanManager_;
    PlatformFocusManager &platformFocusManager_;

    void updateAllGuiElements();
    void paintAllGuiElements( QPainter *painter, int z, bool isCoordinateFramePlatformLocalised );
    
    // finds the nearest platform to the click, pclick and pixelRadius are in window CS
    QString nearestComponent( const QPointF& pclick, const double & pixelRadius );

    // Returns true if the platform which owns the coordinate system is localised
    // or if "global" owns the coordinate system 
    bool transformToPlatformOwningCS( QPainter *p );

    // Qt event handlers
    virtual void paintEvent( QPaintEvent* paintEvent );
    virtual void mousePressEvent( QMouseEvent* );
    virtual void mouseMoveEvent( QMouseEvent* );
    virtual void mouseReleaseEvent( QMouseEvent* );
    virtual void mouseDoubleClickEvent( QMouseEvent* );
//     virtual bool event(QEvent *event);

    QPointF mouseDownPnt_;
//     QPointF mouseMovePnt_;
    QPoint prevDragPos_;
    
    bool antiAliasing_;

    XYDisplay* xyDisplay_;
    QTimer *displayTimer_;
};

}

#endif
