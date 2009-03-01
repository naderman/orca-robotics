/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
#include "viewhandler.h"
#include <osgUtil/SceneView>
#include <osg/Group>
#include <osg/LightSource>

namespace orcaview3d 
{

class WorldView : public QGLWidget
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

private slots:

    void reDisplay();
    void setOrthographicProjection(bool orthoProj);

private:

    void updateAllGuiElements();
    void attachAllGuiElements( bool isCoordinateFramePlatformLocalised,
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

    // Current user input in terms of viewpoint manipulation
    ViewHandler viewHandler_;

    // Current camera pose
    orcaqgui3d::CoordinateFrame cameraPose_;

    // SceneView: Literally the view of the scene, encapsulating
    // how it's rendered.
    osg::ref_ptr<osgUtil::SceneView> sceneView_;

    osg::ref_ptr<osg::LightSource> lightSource_;

    bool isOrthographicProjection_;
};

} // namespace

#endif
