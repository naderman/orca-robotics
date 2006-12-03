/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
  
//   virtual QSize sizeHint () const { return QSize( 600,600 ); };
//   virtual QSize minimumSizeHint () const { return QSize( 200,200 ); };

signals:
    void platformFocusChanged( QString& );
  
public slots:
      
    public:
        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);
        QSize sizeHint();
        QSize minimumSizeHint();
        void keyPressEvent(QKeyEvent *e);
//         void focusInEvent(QFocusEvent *fe);
//         void focusOutEvent(QFocusEvent *fe);
        QImage captureMap();

    private:

    void paintAllGuiElements( bool isFocusLocalised );
    bool transformToFocusPlatformCS();

    PlatformCSFinder *platformCSFinder_;

    // model
    orcaqgui::GuiElementModel* model_;
    orcaqgui::MainWindow *mainWin_;

    // void reset();

    // event handlers
//    void paintEvent( QPaintEvent* paintEvent );
    void mousePressEvent( QMouseEvent* );
    void mouseMoveEvent( QMouseEvent* );
//    void mouseReleaseEvent( QMouseEvent* );
//     void mouseDoubleClickEvent( QMouseEvent* );
//    bool event(QEvent *event);

    QPointF         prevMousePt_;
    Qt::MouseButton currentButton_;
//    QPointF mouseMovePnt_;
    
    QPainter *painter_;
    
    // void renderPointCloud();
    int screenWidth_;
    int screenHeight_;
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

}

#endif
