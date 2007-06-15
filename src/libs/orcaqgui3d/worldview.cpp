/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream> // debugging only
#include <cmath>  // for floor()
#include <map>

#include <QPainter>
#include <QAction>
#include <QTimer>
#include <QPaintEvent>
#include <QInputDialog>
#include <QStatusBar>
#include <QDialog>
#include <QToolTip>

#include <orcaobj/mathdefs.h>
#include <orcaqgui/guielementmodel.h>
#include <orcaqgui/mainwin.h>
#include <orcaqgui/ipermanentelement.h>

#include "guielement3d.h"
#include "glutil.h"
#include "worldview.h"

using namespace std;

namespace orcaqgui3d {

    namespace {
        const int INIT_ZOOM  = 5;
        const int INIT_YAW   = 30;
        const int INIT_PITCH = -60;
    }

WorldView::WorldView( PlatformCSFinder *platformCSFinder,
                      orcaqgui::GuiElementModel* model,
                      QWidget* parent, 
                      orcaqgui::MainWindow* mainWin )
    : QGLWidget(QGLFormat(QGL::AlphaChannel), parent),
      platformCSFinder_(platformCSFinder),
      model_(model),
      mainWin_(mainWin),
      zoomFactor_(INIT_ZOOM),
      xOffset_(0),
      yOffset_(0),
      zOffset_(0),
      yaw_(INIT_YAW),
      pitch_(INIT_PITCH),
//      fudgeFactor_(3),
      showOGs_(true),
      showSnaps_(true),
      showLabels_(true),
      showGrids_(true),
      showRobots_(true),
      showPointclouds_(true),
      showPatchBorders_(true),
      pointSize_(3)
{
    setMinimumSize( 600, 600 );
    setMaximumSize( 1400, 1050 );
    setFocusPolicy(Qt::StrongFocus);
//    makeCurrent();
 
    //connect( this,SIGNAL(platformFocusChanged(const QString&)), this, SLOT(changePlatformFocus(const QString&)) );
    QObject::connect( this,SIGNAL(platformFocusChanged(const QString&)), mainWin_, SLOT(changePlatformFocusFromView(const QString&)) );

    // to show tooltips with robot names, make this configurable?
    // setMouseTracking(true);
    
    // reset();
    // update();
}

WorldView::~WorldView()
{
}

// void
// WorldView::paintEvent( QPaintEvent* e )
// {
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);
// 
//     // set world martrix
//     // painter.setMatrix( wm_ );
// 
//     model_->transformToFocusPlatformCS( &painter );
// 
//     // simple z-buffering, higher-z items obscure (are in front of) lower-z items (just like QCanvas)
//     for ( int z=orcaqgui::Z_BACKGROUND; z<=orcaqgui::Z_FOREGROUND; z+=1 )
//     {
//         // paint guiElements
//         model_->paintGuiElements( &painter, z );
// //         if (humanInput_!=NULL) {
// //             humanInput_->paint( &painter, z);
// //         }
//     }
// }

void 
WorldView::initializeGL()
{
    glClearColor(0.70f, 0.7f, 0.7f, 1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    {
        // light
        glShadeModel( GL_SMOOTH );

        GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

        GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };    
//         GLfloat light_position[] = { 1.0, 0, 0, 0.0 };    
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);    

    }
    
    // Some stupid crap for QT
//    renderText(0,0,0,"");
//     displayList_ = glGenLists(1);
//     cerr << "Display list is: " <<  displayList_ << endl;
}

void 
WorldView::resizeGL(int w, int h)
{
    cout<<"TRACE(worldview3d.cpp): resizeGL()" << endl;
//     screenWidth_ = w;
//     screenHeight_ = h;
    float aspectRatio = ((float) w)/((float) h);

    // update projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //qDebug("Aspect ratio set to %f", aspectRatio); 
    //glOrtho(-aspectRatio, aspectRatio, -1, 1, -1000, 1000); 
    gluPerspective(60, aspectRatio, 1,1000);

    // update model/view matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0,0,-2);

    // viewport
    glViewport(0,0,w,h);

    // Qt call to redraw
    updateGL();
}

void 
WorldView::paintGL()
{
    cout<<"TRACE(worldview3d.cpp): ==================== paintGL() ====================" << endl;

    {
        //qDebug("paintGL on mapview called");

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glPushMatrix();

        // Put the camera in position
        glScalef(1.0/zoomFactor_, 1.0/zoomFactor_, 1.0/zoomFactor_);
        glRotatef(pitch_,1,0,0);
        glRotatef(yaw_,0,0,1);
        glTranslatef(xOffset_, yOffset_, zOffset_);
    }

//     GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
//     GLfloat mat_specular[] = { 0.0, 0.0, 1, 1 };
//     GLfloat mat_shininess[] = { 50.0 };
//     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//     glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

//     glColor3f( 0.0, 0.0, 1.0 );

    GLfloat diffuseMaterial[4] = { 0.1, 0.1, 0.5, 1.0 };
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuseMaterial);

    glutil::drawBox( 1, 1, 1, true, true );
//     glutil::drawIcosahedron();

    glPopMatrix();
}

// void WorldView::paintGL()
// {
//     cout<<"TRACE(worldview3d.cpp): ==================== paintGL() ====================" << endl;

//     {
//         //qDebug("paintGL on mapview called");

//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//         glEnable(GL_BLEND);
//         glDisable(GL_DEPTH_TEST);
//         glEnable(GL_POINT_SMOOTH);
//         glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
//         glEnable(GL_LINE_SMOOTH);
//         glEnable(GL_POLYGON_SMOOTH);
// //        glMatrixMode(GL_MODELVIEW);

//         glPushMatrix();
//         glScalef(1.0/zoomFactor_, 1.0/zoomFactor_, 1.0/zoomFactor_);

// //         // Try to get light working...
// //         {
// //             GLfloat light_position[] = { 10.0, 10.0, 10.0, 0.0 };
// //             glLightfv(GL_LIGHT0, GL_POSITION, light_position);

// //             glEnable(GL_LIGHTING);
// //             glEnable(GL_LIGHT0);
// //             glDepthFunc(GL_LEQUAL);
// //         }

//         glRotatef(pitch_,1,0,0);
//         glRotatef(yaw_,0,0,1);
//         glTranslatef(xOffset_, yOffset_, zOffset_);
//     }

// //    glutil::drawBox( 1, 1, 1, true, true );

//    bool isFocusLocalised = transformToFocusPlatformCS();
//    paintAllGuiElements( isFocusLocalised );

//     glDisable(GL_BLEND);
//     glEnable(GL_DEPTH_TEST);
//     glDisable(GL_POINT_SMOOTH);
//     glDisable(GL_LINE_SMOOTH);
//     glDisable(GL_POLYGON_SMOOTH);
//     glPopMatrix();
// }

void
WorldView::paintAllGuiElements( bool isFocusLocalised )
{
    const QList<orcaqgui::GuiElement*> &elements = model_->elements();

    for ( int i=0; i<elements.size(); ++i )
    {
        if ( !elements[i] ) {
            continue;
        }
        
        orcaqgui::IPermanentElement *permElement = dynamic_cast<orcaqgui::IPermanentElement*>(elements[i]);
        
        try {
           // paint the platform that has focus and the background grid         
            
            if ( isFocusLocalised || elements[i]->platform()==model_->coordinateFramePlatform() || (permElement!=NULL) )
            {
                GuiElement3d *elem = dynamic_cast<GuiElement3d*>(elements[i]);
                assert(elem != NULL);

                if ( elem->isInGlobalCS() )
                {
                    elem->paint( this );
                }
                else
                {
                    glPushMatrix();
                    {
                        // if a localiser element is not found, the coords of the painter
                        // will not be transformed and the element will be painted at the
                        // saved coords
                        float x, y, z, roll, pitch, yaw;
                        if ( platformCSFinder_->findPlatformCS( model_->elements(),
                                                                model_->coordinateFramePlatform(),
                                                                x,
                                                                y,
                                                                z,
                                                                roll,
                                                                pitch,
                                                                yaw ) )
                        {
                            orcaqgui3d::glutil::transform( x,y,z,roll,pitch,yaw );
                        }
                        elem->paint( this );
                    }
                    glPopMatrix();
                }
            }
        }
        catch ( Ice::Exception &e )
        {
            std::cout<<"TRACE(worldview3d.cpp): Caught some ice exception during painting of ";
//                      <<elements[i]->id.toStdString()<<": " << e << std::endl;
        }
        catch ( std::exception &e )
        {
            std::cout<<"TRACE(worldview3d.cpp): Caught some std exception during painting of ";
//                      <<elements[i]->id.toStdString()<<": " << e.what() << std::endl;
        }
        catch ( std::string &e )
        {
            std::cout<<"TRACE(worldview3d.cpp): Caught std::string during painting of ";
//                      <<elements[i]->id.toStdString()<<": " << e << std::endl;
        }
        catch ( char *e )
        {
            std::cout<<"TRACE(worldview3d.cpp): Caught char * during painting of ";
//                      <<elements[i]->id.toStdString()<<": " << e << std::endl;
        }
        catch ( ... )
        {
            std::cout<<"TRACE(worldview3d.cpp): Caught some other exception during painting of ";
//                      <<elements[i]->id.toStdString()<<": " << std::endl;
        }
    }
}

bool
WorldView::transformToFocusPlatformCS()
{
    // special case: global CS
    if ( model_->coordinateFramePlatform() == "global" )
    {
        // TODO: should we set to identity here??
        // focusMatrix = QMatrix();
        return true;
    }
    else
    {
        float x, y, z, roll, pitch, yaw;
        if ( platformCSFinder_->findPlatformCS( model_->elements(),
                                                model_->coordinateFramePlatform(),
                                                x,
                                                y,
                                                z,
                                                roll,
                                                pitch,
                                                yaw ) )
        {
            orcaqgui3d::glutil::transform( x,y,z,roll,pitch,yaw );
            return true;
        }
        else
        {
            return false;
        }
    }
}

void 
WorldView::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_W){
        yOffset_ += 0.1*zoomFactor_;
    }
    else if(e->key() == Qt::Key_S){
        yOffset_ -= 0.1*zoomFactor_;
    }
    else if(e->key() == Qt::Key_A){
        xOffset_ -= 0.1*zoomFactor_;
    }
    else if(e->key() == Qt::Key_D){
        xOffset_ += 0.1*zoomFactor_;
    }
    else if(e->key() == Qt::Key_P){
        pointSize_ += 1;
    }
    else if(e->key() == Qt::Key_O){
        pointSize_ -= 1;
        if(pointSize_ < 1) pointSize_ = 1;
    }
    else if(e->key() == Qt::Key_BracketLeft){
        zoomFactor_ *= 1.1;
    }
    else if(e->key() == Qt::Key_BracketRight){
        zoomFactor_ /= 1.1;
    }
    else if(e->key() == Qt::Key_Left){
        yaw_ += 5;
    
    }
    else if(e->key() == Qt::Key_Right){
        yaw_ -= 5;
    }
    else if(e->key() == Qt::Key_Up){
        pitch_ += 5;
        if(pitch_ > 0) pitch_ = 0;
    }
    else if(e->key() == Qt::Key_Down){
        pitch_ -= 5;
        if(pitch_ < -90) pitch_ = -90;
    }
    else if(e->key() == Qt::Key_R){
        zoomFactor_=INIT_ZOOM;
        xOffset_= yOffset_=zOffset_=0;
        yaw_=INIT_YAW;
        pitch_=INIT_PITCH;
    }

// AlexB: what does the fudgefactor do??
#if 0
    else if(e->text() == "="){
        fudgeFactor_ *=1.25;
        qDebug("Fudge factor set to %f", fudgeFactor_);
    }
    else if(e->text()=="-"){
        fudgeFactor_ /=1.25;
        qDebug("Fudge factor set to %f", fudgeFactor_);
    }
    else if(e->text() == "0"){
        fudgeFactor_=3;
        qDebug("Fudge factor set to %f", fudgeFactor_);
    }
#endif    
    
    update();
}

// void WorldView::focusInEvent(QFocusEvent *fe){
//     makeCurrent();
//     glClearColor(0.7f,0.7f,0.7f,1.0f);
//     updateGL();
// }

// void WorldView::focusOutEvent(QFocusEvent *fe){
//     makeCurrent();
//     //glClearColor(0.3f,0.3f,0.3f,1.0f);  
//     updateGL();
// }

QImage 
WorldView::captureMap()
{
    return grabFrameBuffer();
}


// COMMON VIEW MANIPULATION FUNCTIONS ====================


void
WorldView::mousePressEvent( QMouseEvent* e )
{
    prevMousePt_ = e->pos();
    currentButton_ = e->button();
}

void
WorldView::mouseMoveEvent( QMouseEvent* e )
{
    const float sensitivity = 0.02;

    float dx = e->pos().x() - prevMousePt_.x();
    float dy = e->pos().y() - prevMousePt_.y();
    prevMousePt_ = e->pos();

    // cout<<"TRACE(worldview3d.cpp): dx, dy: " << dx << "," << dy << endl;

    if ( currentButton_ == Qt::LeftButton )
    {
        // Rotate camera
        //cout<<"TRACE(worldview3d.cpp): rotate" << endl;
        yaw_   += sensitivity*dx*10;
        pitch_ += sensitivity*dy*10;
        if(pitch_ > 0) pitch_ = 0;
        if(pitch_ < -90) pitch_ = -90;
    }
    else if ( currentButton_ == Qt::RightButton )
    {
        // Pan camera (relative to the view-point)
        float x = sensitivity*dx*0.1*zoomFactor_;
        float y = sensitivity*dy*0.1*zoomFactor_;

        float yawRad = yaw_*M_PI/180.0;

        xOffset_ += x*cos(yawRad) - y*sin(yawRad);
        yOffset_ -= x*sin(yawRad) + y*cos(yawRad);
        //cout<<"TRACE(worldview3d.cpp): pan" << endl;
    }
    else if ( currentButton_ == Qt::MidButton )
    {
        // Zoom camera
        //cout<<"TRACE(worldview3d.cpp): zooming.  old zoomFactor: " << zoomFactor_<< endl;
        float multZoomFactor = (1.0+sensitivity*dy);

        multZoomFactor = CHECK_LIMITS( 1.2, multZoomFactor, 0.8 );
        zoomFactor_ *= multZoomFactor;
    }
}

// void
// WorldView::mouseReleaseEvent( QMouseEvent* e )
// {
    
//     if (humanInput_!=NULL)
//     {
//         humanInput_->processReleaseEvent( e );
//         return;
//     }

//     // here comes what you if no humaninput class is initiated
//     QPointF mouseUpPnt = e->pos();

//     if ( e->button() == Qt::LeftButton )          // left button released
//     {
//         const double MOVE_RADIUS_PIXEL = 10.0;

//         QPointF diff(mouseDownPnt_ - mouseUpPnt);
//         double dist = sqrt(diff.x()*diff.x() + diff.y()*diff.y());
//         //cout << "TRACE(worldview.cpp): Diff is " << diff.x() << " " << diff.y() << endl;
//         //cout << "TRACE(worldview.cpp): dist is " << dist << endl;

//         if (dist>MOVE_RADIUS_PIXEL)
//         { // move canvas
//             moveView( mouseUpPnt - mouseDownPnt_ );
//         }
  
//         // select component
//         const double SELECTED_RADIUS_M = 0.6;
//         QMatrix wmInv = wm_.inverted();
//         QString platform = model_->nearestComponent( wmInv.map(mouseDownPnt_), SELECTED_RADIUS_M );
//         //cout << "TRACE(worldview.cpp): nearestComponent is: " << platform.toStdString() << endl;
//         // alert others
//         if ( platform != "" ) 
//         {
//             cout << "Platform focus changed" << endl;
//             mainWin_->changePlatformFocusFromView( platform );
//         }
//     } 
// }


// bool WorldView::event(QEvent *event)
// {
//     if (event->type() == QEvent::ToolTip) 
//     {
//         QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
//         const double SELECTED_RADIUS_M = 0.6;
//         QMatrix wmInv = wm_.inverted();
//         QString platform = model_->nearestComponent( wmInv.map(mouseMovePnt_), SELECTED_RADIUS_M );
//         if (platform!="") QToolTip::showText(helpEvent->globalPos(), platform);
//     }
//     return QWidget::event(event);
// }

} // namespace
