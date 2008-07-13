/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */


#include <gbxutilacfr/mathdefs.h>
#include "worldview.h"
#include <orcaqgui3d/glutil.h>

using namespace std;

namespace orcaview3d {

    namespace {
        const int INIT_ZOOM  = 5;
        const int INIT_YAW   = 30;
        const int INIT_PITCH = -60;
    }

WorldView::WorldView( orcaqgui3d::PlatformCSFinder              &platformCSFinder,
                      ::hydroqguielementutil::MouseEventManager &mouseEventManager,
                      hydroqgui::GuiElementSet                  &guiElementSet,
                      ::hydroqguielementutil::IHumanManager     &humanManager,
                      hydroqgui::PlatformFocusManager           &platformFocusManager,
                      int                                        displayRefreshTime,
                      QWidget                                   *parent )
    : QGLWidget(parent),
      platformCSFinder_(platformCSFinder),
      mouseEventManager_(mouseEventManager),
      guiElementSet_(guiElementSet),
      humanManager_(humanManager),
      platformFocusManager_(platformFocusManager),
      zoomFactor_(INIT_ZOOM),
      xOffset_(0),
      yOffset_(0),
      zOffset_(0),
      yaw_(INIT_YAW),
      pitch_(INIT_PITCH)
{
    displayTimer_ = new QTimer( this );
    QObject::connect( displayTimer_,SIGNAL(timeout()), this,SLOT(updateAllGuiElements()) );
    displayTimer_->start( displayRefreshTime );    
}
      
void 
WorldView::initializeGL()
{
    // Set the default (background) color
    glClearColor(0.70f, 0.7f, 0.7f, 0.0f);

    // Enable the depth buffer: to handle hidden surface removal
    glEnable(GL_DEPTH_TEST);



#if 0
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
#endif
}

void 
WorldView::resizeGL(int w, int h)
{
    cout<<"TRACE(worldview3d.cpp): resizeGL()" << endl;
//     screenWidth_ = w;
//     screenHeight_ = h;
//    float aspectRatio = ((float) w)/((float) h);

    // update projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //qDebug("Aspect ratio set to %f", aspectRatio); 
    //glOrtho(-aspectRatio, aspectRatio, -1, 1, -1000, 1000); 
    //gluPerspective(60, aspectRatio, 1,1000);

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

    // Start from a clean slate
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    orcaqgui3d::glutil::drawBox( 1, 1, 1, true, true );
    glFlush();

#if 0
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

    orcaqgui3d::glutil::drawBox( 1, 1, 1, true, true );
//     glutil::drawIcosahedron();

    glPopMatrix();
#endif
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
WorldView::updateAllGuiElements()
{
    for ( int i=0; i<guiElementSet_.elements().size(); ++i )
    {
        ::hydroqguielementutil::IGuiElement *element = guiElementSet_.elements()[i];

        if ( element ) {
            std::stringstream ss;
            try {
                element->update();
            }
            catch ( IceUtil::Exception &e )
            {
                ss<<"WorldView: during update of "
                <<element->details().toStdString()<<": " << e << std::endl;
                humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( std::exception &e )
            {
                ss<<"WorldView: during update of "
                <<element->details().toStdString()<<": " << e.what() << std::endl;
                humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( std::string &e )
            {
                ss<<"WorldView: during update of "
                   <<element->details().toStdString()<<": " << e << std::endl;
                humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( char *e )
            {
                ss<<"WorldView: during update of "
                   <<element->details().toStdString()<<": " << e << std::endl;
                humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( ... )
            {
                ss<<"WorldView: Caught unknown exception during update of "
                   <<element->details().toStdString()<<": " << std::endl;
                humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
        }
    }

    // issue a QPaintEvent to force a repaint
    updateGL();
}

void
WorldView::paintAllGuiElements( bool isFocusLocalised )
{
    cout<<"TRACE(worldview.cpp): "<<__func__<< endl;
    return;

#if 0
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
                        if ( platformCSFinder_.findPlatformCS( model_->elements(),
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
#endif
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
    
    updateGL();
}

// COMMON VIEW MANIPULATION FUNCTIONS ====================

void
WorldView::mousePressEvent( QMouseEvent* e )
{
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        //cout<<"TRACE(worldview.cpp): giving mousePressEvent to mode owner" << endl;
        mouseEventManager_.mouseEventReceiver()->mousePressEvent( e );
        return;
    }
    else
    {
        //cout<<"TRACE(worldview.cpp): Using mousePressEvent ourself" << endl;
        mouseDownPnt_ = e->pos();
        prevDragPos_ = e->pos();
    }
}

void
WorldView::mouseMoveEvent( QMouseEvent* e )
{
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        //mouseEventManager_.mouseEventReceiver()->mouseMoveEvent( e );
        return;
    }

    const float sensitivity = 0.02;

    float dx=0, dy=0;
    if ( e->buttons() & Qt::LeftButton ||
         e->buttons() & Qt::RightButton ||
         e->buttons() & Qt::MidButton )
    {
        dx = e->pos().x() - prevDragPos_.x();
        dy = e->pos().y() - prevDragPos_.y();        
        prevDragPos_ = e->pos();        
    }

    // cout<<"TRACE(worldview3d.cpp): dx, dy: " << dx << "," << dy << endl;

    if ( e->buttons() & Qt::LeftButton )
    {
        // Rotate camera
        //cout<<"TRACE(worldview3d.cpp): rotate" << endl;
        yaw_   += sensitivity*dx*10;
        pitch_ += sensitivity*dy*10;
        if (pitch_ > 0)   pitch_ = 0;
        if (pitch_ < -90) pitch_ = -90;
        updateGL();
    }
    else if ( e->buttons() & Qt::RightButton )
    {
        // Pan camera (relative to the view-point)
        float x = sensitivity*dx*0.1*zoomFactor_;
        float y = sensitivity*dy*0.1*zoomFactor_;

        float yawRad = yaw_*M_PI/180.0;

        xOffset_ += x*cos(yawRad) - y*sin(yawRad);
        yOffset_ -= x*sin(yawRad) + y*cos(yawRad);
        //cout<<"TRACE(worldview3d.cpp): pan" << endl;
        updateGL();
    }
    else if ( e->buttons() & Qt::MidButton )
    {
        // Zoom camera
        //cout<<"TRACE(worldview3d.cpp): zooming.  old zoomFactor: " << zoomFactor_<< endl;
        float multZoomFactor = (1.0+sensitivity*dy);

        CLIP_TO_LIMITS<float>( 0.8, multZoomFactor, 1.2 );
        zoomFactor_ *= multZoomFactor;
        updateGL();
    }
}

void
WorldView::mouseReleaseEvent( QMouseEvent* e )
{
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        //cout<<"TRACE(worldview.cpp): Giving mouseReleaseEvent to mode owner" << endl;
        mouseEventManager_.mouseEventReceiver()->mouseReleaseEvent( e );
        return;
    }
}


void
WorldView::mouseDoubleClickEvent( QMouseEvent* e )
{
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
        mouseEventManager_.mouseEventReceiver()->mouseDoubleClickEvent( e );
}


} // namespace
