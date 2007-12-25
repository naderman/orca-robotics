/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAVIEW_WORLDVIEW_H
#define ORCA_ORCAVIEW_WORLDVIEW_H

#include <QMouseEvent>
#include <QFileDialog>
#include <QString>

#include <orcaqgui2d/zoomwidget.h>
#include <hydroqgui/hydroqgui.h>

namespace orcaqgui {
    class GuiElementModel;
}

namespace orcaqgui2d {

class HumanInput;
class PlatformCSFinder;
class GuiQPainter;

//!
//! @brief Displays a set of GuiElements in a geometrically-correct way
//!
class WorldView : public orcaqgui2d::ZoomWidget
{
   Q_OBJECT
public:
    WorldView( PlatformCSFinder                   *platformCSFinder,
               hydroqgui::MouseEventManager       &mouseEventManager,
               hydroqgui::GuiElementSet           &guiElementSet,
               hydroqgui::CoordinateFrameManager  &coordinateFrameManager,
               hydroqgui::IHumanManager           &humanManager,
               QWidget                            *parent );
  ~WorldView();
  
public slots:
    // abstract zoom functions
    void zoomFitWin();
    void zoomAllPix();
    void setAntiAliasing(bool antiAliasing);
    void setUseTransparency(bool useTransparency);
    
private:

    void printStatusMessage(bool antiAliasing);
    void setupInterface();

    // finds coord system of a platform
    PlatformCSFinder *platformCSFinder_;

    // handles distribution of mouse events to GuiElements
    hydroqgui::MouseEventManager &mouseEventManager_;

    hydroqgui::GuiElementSet          &guiElementSet_;
    hydroqgui::CoordinateFrameManager &coordinateFrameManager_;

    hydroqgui::IHumanManager &humanManager_;

    void paintAllGuiElements( QPainter *painter, int z, bool isCoordinateFramePlatformLocalised );
    
    // finds the nearest platform to the click, pclick and pixelRadius are in window CS
    QString nearestComponent( const QPointF& pclick, const double & pixelRadius );

    // Returns true if the platform which owns the coordinate system is localised
    // or if "global" owns the coordinate system 
    bool transformToPlatformOwningCS( QPainter *p );

    // event handlers
    void paintEvent( QPaintEvent* paintEvent );
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* );
//     bool event(QEvent *event);

    QPointF mouseDownPnt_;
    QPointF mouseMovePnt_;

    QPoint prevDragPos_;
    
    bool antiAliasing_;
      
};

}

#endif
