/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAVIEW_WORLDVIEW_H
#define ORCA2_ORCAVIEW_WORLDVIEW_H

#include <QMouseEvent>
#include <QFileDialog>
#include <QString>

#include <orcaqgui2d/zoomwidget.h>

namespace orcaqgui {

class GuiElementModel;
class HumanInput;
class MainWindow;
class PlatformCSFinder;
class GuiQPainter;

class WorldView : public orcaqgui::ZoomWidget
{
   Q_OBJECT
public:
    WorldView( PlatformCSFinder* platformCSFinder,
               GuiElementModel* model,
               QWidget* parent=0, 
               MainWindow* mainWin=0 );
  ~WorldView();
  
public slots:
    // abstract zoom functions
    void zoomFitWin();
    void zoomAllPix();
    
    void setAntiAliasing(bool antiAliasing);
    
private:

    void printStatusMessage(bool antiAliasing);
    void setupInterface();

    // finds coord system of a platform
    PlatformCSFinder *platformCSFinder_;
          
    // model
    GuiElementModel* model_;
    
    MainWindow *mainWin_;

    void paintAllGuiElements( QPainter *painter, int z, bool isCoordinateFramePlatformLocalised );
    QString nearestComponent( const QPointF& pclick, const double & radius );

    // Returns true if the platform which owns the coordinate system is localised
    // or if "global" owns the coordinate system 
    bool transformToPlatformOwningCS( QPainter *p );

    // event handlers
    void paintEvent( QPaintEvent* paintEvent );
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* );
    bool event(QEvent *event);

    QPointF mouseDownPnt_;
    QPointF mouseMovePnt_;

    QPoint prevDragPos_;
    
    QPainter *painter_;
    
    bool antiAliasing_;
      
};

}

#endif
