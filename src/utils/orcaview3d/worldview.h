/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAGUI3D_WORLDVIEW3D_H
#define ORCA_ORCAGUI3D_WORLDVIEW3D_H

#include <QMouseEvent>
#include <QFileDialog>
#include <QString>
#include <QtOpenGL>
//#include <GL/glut.h>
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui3d/platformcsfinder.h>

namespace orcaview3d 
{

class WorldView : public QGLWidget
{
   Q_OBJECT
public:
    WorldView( orcaqgui3d::PlatformCSFinder              &platformCSFinder,
               ::hydroqguielementutil::MouseEventManager &mouseEventManager,
               hydroqgui::GuiElementSet                  &guiElementSet,
               ::hydroqguielementutil::IHumanManager     &humanManager,
               hydroqgui::PlatformFocusManager           &platformFocusManager,
               int                                        displayRefreshTime,
               QWidget                                   *parent = NULL );

    QSize sizeHint() const { return QSize( 400, 400 ); }

private slots:

 void updateAllGuiElements();

private:

    void paintAllGuiElements( bool isFocusLocalised );

    const orcaqgui3d::PlatformCSFinder &platformCSFinder_;

    // handles distribution of mouse events to GuiElements
    ::hydroqguielementutil::MouseEventManager &mouseEventManager_;

    hydroqgui::GuiElementSet     &guiElementSet_;

    ::hydroqguielementutil::IHumanManager &humanManager_;
    hydroqgui::PlatformFocusManager &platformFocusManager_;

    // from QGLWidget
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    // event handlers
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
    void mouseReleaseEvent( QMouseEvent* );
    void mouseDoubleClickEvent( QMouseEvent* );
    void keyPressEvent(QKeyEvent *e);
    
    QPointF mouseDownPnt_;
    QPoint prevDragPos_;
    
    QTimer *displayTimer_;

    // Current viewpoint
    float zoomFactor_;
    float xOffset_, yOffset_, zOffset_;
    float yaw_, pitch_;
    


//     // void renderPointCloud();
// //     int screenWidth_;
// //     int screenHeight_;
//     // float fudgeFactor_;
//     //int displayList; 
//     bool showOGs_;
//     bool showSnaps_;
//     bool showLabels_;
//     bool showGrids_;
//     bool showRobots_;
//     bool showPointclouds_;
//     bool showPatchBorders_;
//     float pointSize_;
};

} // namespace

#endif
