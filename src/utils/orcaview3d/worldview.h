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
#include <hydroqgui/hydroqgui.h>
#include <orcaqgui3d/platformcsfinder.h>
#include <orcaqgui3d/view.h>
#include "viewhandler.h"
#include <osgUtil/SceneView>
#include <osg/Group>
#include <osg/LightSource>

namespace orcaview3d 
{

class WorldView : public QGLWidget, 
                  public orcaqgui3d::View
{
   Q_OBJECT
public:
    WorldView( orcaqgui3d::PlatformCSFinder              &platformCSFinder,
               ::hydroqguielementutil::MouseEventManager &mouseEventManager,
               hydroqgui::GuiElementSet                  &guiElementSet,
               hydroqgui::CoordinateFrameManager         &coordinateFrameManager,
               ::hydroqguielementutil::IHumanManager     &humanManager,
               hydroqgui::PlatformFocusManager           &platformFocusManager,
               int                                        displayRefreshTime,
               QWidget                                   *parent = NULL );

    QSize sizeHint() const { return QSize( 400, 400 ); }

    // All in S.I. units and Orca-style coordinate system
    virtual double cameraX() const { return cameraPose_.pos().x(); }
    virtual double cameraY() const { return cameraPose_.pos().y(); }
    virtual double cameraZ() const { return cameraPose_.pos().z(); }
    virtual double cameraRoll()  const { return cameraPose_.roll(); }
    virtual double cameraPitch() const { return cameraPose_.pitch(); }
    virtual double cameraYaw()   const { return cameraPose_.yaw(); }

    virtual bool isAntialiasingEnabled() const { return isAntialiasingEnabled_; }

public slots:

// void setAntiAliasing( bool antiAliasing );

private slots:

    void reDisplay();

private:

    void updateAllGuiElements();
    void paintAllGuiElements( bool isCoordinateFramePlatformLocalised,
                              osg::Group *root );

    // finds coord system of a platform
    const orcaqgui3d::PlatformCSFinder &platformCSFinder_;
    hydroqgui::CoordinateFrameManager &coordinateFrameManager_;

    // handles distribution of mouse events to GuiElements
    ::hydroqguielementutil::MouseEventManager &mouseEventManager_;

    hydroqgui::GuiElementSet     &guiElementSet_;

    ::hydroqguielementutil::IHumanManager &humanManager_;
    hydroqgui::PlatformFocusManager &platformFocusManager_;

    orcaqgui3d::CoordinateFrame getCameraPose( bool &isCameraPoseLocalised );

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
    
    QTimer *displayTimer_;

    bool isAntialiasingEnabled_;

    // Current user input in terms of viewpoint manipulation
    ViewHandler viewHandler_;

    // Current camera pose
    orcaqgui3d::CoordinateFrame cameraPose_;

    // SceneView: Literally the view of the scene, encapsulating
    // how it's rendered.
    osg::ref_ptr<osgUtil::SceneView> sceneView_;

    osg::ref_ptr<osg::LightSource> lightSource_;
};

} // namespace

#endif
