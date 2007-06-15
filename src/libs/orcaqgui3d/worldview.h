/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAGUI3D_WORLDVIEW3D_H
#define ORCA2_ORCAGUI3D_WORLDVIEW3D_H

#include <QMouseEvent>
#include <QFileDialog>
#include <QString>
#include <QtOpenGL>
#include <GL/glut.h>
#include <orcaqgui3d/platformcsfinder.h>

namespace orcaqgui
{
    class GuiElementModel;
    class WaypointSettings;
    class HumanInput;
    class MainWindow;
}

namespace orcaqgui3d 
{

class WorldView : public QGLWidget
{
   Q_OBJECT
public:
    WorldView( PlatformCSFinder* platformCSFinder,
                orcaqgui::GuiElementModel* model,
                QWidget* parent=0, 
                orcaqgui::MainWindow* mainWin=0 );
    ~WorldView();

    QImage captureMap();

signals:
    void platformFocusChanged( QString& );

protected:
    // from QGLWidget
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    // from QWidget
//    virtual void paintEvent( QPaintEvent* paintEvent );
    virtual void mousePressEvent( QMouseEvent* );
    virtual void mouseMoveEvent( QMouseEvent* );
//    virtual void mouseReleaseEvent( QMouseEvent* );
//    virtual void mouseDoubleClickEvent( QMouseEvent* );
//    virtual bool event(QEvent *event);
//    virtual QSize sizeHint () const { return QSize( 600,600 ); };
//    virtual QSize minimumSizeHint () const { return QSize( 200,200 ); };
    virtual void keyPressEvent(QKeyEvent *e);
//     virtual void focusInEvent(QFocusEvent *fe);
//     virtual void focusOutEvent(QFocusEvent *fe);

private:

    void paintAllGuiElements( bool isFocusLocalised );
    bool transformToFocusPlatformCS();

    PlatformCSFinder *platformCSFinder_;

    // model
    orcaqgui::GuiElementModel* model_;
    orcaqgui::MainWindow *mainWin_;

    // void reset();

    QPointF         prevMousePt_;
    Qt::MouseButton currentButton_;
//    QPointF mouseMovePnt_;
    
    QPainter *painter_;
    
    // void renderPointCloud();
//     int screenWidth_;
//     int screenHeight_;
    float zoomFactor_;
    float xOffset_, yOffset_, zOffset_;
    float yaw_, pitch_;
    // float fudgeFactor_;
    //int displayList; 
    bool showOGs_;
    bool showSnaps_;
    bool showLabels_;
    bool showGrids_;
    bool showRobots_;
    bool showPointclouds_;
    bool showPatchBorders_;
    float pointSize_;
};

} // namespace

#endif
