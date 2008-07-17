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
#include <orcaqgui3d/guielement3d.h>

using namespace std;

namespace orcaview3d {

WorldView::WorldView( orcaqgui3d::PlatformCSFinder              &platformCSFinder,
                      ::hydroqguielementutil::MouseEventManager &mouseEventManager,
                      hydroqgui::GuiElementSet                  &guiElementSet,
                      hydroqgui::CoordinateFrameManager         &coordinateFrameManager,
                      ::hydroqguielementutil::IHumanManager     &humanManager,
                      hydroqgui::PlatformFocusManager           &platformFocusManager,
                      int                                        displayRefreshTime,
                      QWidget                                   *parent )
    : QGLWidget(parent),
      platformCSFinder_(platformCSFinder),
      coordinateFrameManager_(coordinateFrameManager),
      mouseEventManager_(mouseEventManager),
      guiElementSet_(guiElementSet),
      humanManager_(humanManager),
      platformFocusManager_(platformFocusManager),
      isAntialiasingEnabled_(true)
{
    // Allow receipt of keyboard events
    setFocusPolicy( Qt::StrongFocus );

    displayTimer_ = new QTimer( this );
    QObject::connect( displayTimer_,SIGNAL(timeout()), this,SLOT(reDisplay()) );
    displayTimer_->start( displayRefreshTime );    

    // Antialiasing
    QAction* antiAliasing = new QAction(tr("&Anti-Aliasing"),this);
    antiAliasing->setCheckable(true);
    antiAliasing->setShortcut( QKeySequence("Ctrl+A") );
    connect(antiAliasing,SIGNAL(toggled(bool)), this, SLOT(setAntiAliasing(bool)) );
    humanManager_.displayMenu()->addAction(antiAliasing);
}

CoordinateFrame
WorldView::getCameraPose(bool &isCameraPoseLocalised)
{
    if ( coordinateFrameManager_.coordinateFramePlatform() == "global" )
    {
        isCameraPoseLocalised = true;
        return viewHandler_.pose();
    }
    else
    {
        float x,y,z, roll,pitch,yaw;
        if ( platformCSFinder_.findPlatformCS( guiElementSet_.elements(),
                                               coordinateFrameManager_.coordinateFramePlatform(),
                                               x, y, z,
                                               roll, pitch, yaw ) )
        {
            assert( !coordinateFrameManager_.ignoreCoordinateFrameRotation() && 
                    "Huh?  What does this do?" );

            isCameraPoseLocalised = true;

            // Begin with the platform's CF
            CoordinateFrame cf( Vector3(x,y,z), roll,pitch,yaw );

            // Add the user's input
            cf.multMatrix( viewHandler_.pose().homogeneousMatrix() );

            return cf;
        }
        else
        {
            isCameraPoseLocalised = false;
            return viewHandler_.pose();
        }
    }
}

void 
WorldView::initializeGL()
{
    // Set the default (background) color
    glClearColor(0.70f, 0.7f, 0.7f, 0.0f);
    
    // Enable the depth buffer: to handle hidden surface removal
    glEnable(GL_DEPTH_TEST);
}

void 
WorldView::resizeGL(int w, int h)
{
    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // sets the perspective:
    //  (left,right,bottom,top,near,far)
    glFrustum (-1.0, 1.0, -1.0, 1.0, 1.0, 200.0); 

    // back to modelview matrix
    glMatrixMode (GL_MODELVIEW);
    // define the viewport
    glViewport (0, 0, w, h);

#if 1
    // Not sure what difference this makes exactly...
    GLenum hint = GL_DONT_CARE;
    glHint( GL_POINT_SMOOTH_HINT, hint );
    glHint( GL_LINE_SMOOTH_HINT, hint );
    glHint( GL_POLYGON_SMOOTH_HINT, hint );
    glHint( GL_FOG_HINT, hint );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, hint );
#endif
}

void 
WorldView::paintGL()
{
    // Antialiasing
    if ( isAntialiasingEnabled_ )
    {
        glEnable( GL_POINT_SMOOTH );
        glEnable( GL_LINE_SMOOTH );
        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }
    else
    {
        glDisable( GL_POINT_SMOOTH );
        glDisable( GL_LINE_SMOOTH );
        glDisable( GL_BLEND );
    }

    // Start from a clean slate
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // Clear the viewing transformation
    glLoadIdentity();

    // Work out where to put the camera
    bool isCameraPoseLocalised = false;
    cameraPose_ = getCameraPose( isCameraPoseLocalised );

    // Put the camera in position
    Vector3 center = cameraPose_.pos() + cameraPose_.fwd();
    gluLookAt( cameraPose_.pos().x(),
               cameraPose_.pos().y(),
               cameraPose_.pos().z(),
               center.x(), 
               center.y(), 
               center.z(),
               cameraPose_.up().x(), 
               cameraPose_.up().y(), 
               cameraPose_.up().z() );

    paintAllGuiElements( isCameraPoseLocalised );
}

void
WorldView::reDisplay()
{
    updateAllGuiElements();

    // force a repaint
    updateGL();    
}

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
}

void
WorldView::paintAllGuiElements( bool isCoordinateFramePlatformLocalised )
{
    const QList< ::hydroqguielementutil::IGuiElement*> &elements = guiElementSet_.elements();

    for ( int i=0; i<elements.size(); ++i )
    {
        if ( !elements[i] ) {
            continue;
        }

        orcaqgui3d::GuiElement3d *elem = dynamic_cast<orcaqgui3d::GuiElement3d*>(elements[i]);
        assert(elem != NULL);

        stringstream ss;
        try {
            if ( elem->needsInit() )
            {
                elem->init(*this);
                elem->initialisationDone();
            }

            // paint all elements in the world if platform that owns coordinate system is localised 
            // also paint all elements of the platform that owns coordinate system even if it's not localised
            // always paint the permanent elements                     
            if ( isCoordinateFramePlatformLocalised || 
                 elem->platform()==coordinateFrameManager_.coordinateFramePlatform() || 
                 elem->isPermanentElement() ) 
            {
                // Save the matrix, in case an exception is thrown.
                orcaqgui3d::glutil::ScopedMatrixSave sms;

                if ( elem->isInGlobalCS() )
                {
                    elem->paint( *this, *this );
                }
                else
                {
                    // This GuiElement is in a local coordinate system: find the platform's CS
                    float x, y, z, roll, pitch, yaw;
                    const bool platformCSFound = platformCSFinder_.findPlatformCS( guiElementSet_.elements(),
                                                                                   elements[i]->platform(),
                                                                                   x, y, z, 
                                                                                   roll, pitch, yaw );
                    if ( !platformCSFound )
                    {
                        // Can't find the location of this platform -- paint in the global CS
                        elem->paint( *this, *this );
                    }
                    else
                    {
                        orcaqgui3d::glutil::transform(x,y,z,roll,pitch,yaw);
                        elem->paint( *this, *this );
                    }
                }

                // Raises an exception on an OpenGL error
                orcaqgui3d::glutil::checkGLError();
            }
        }
        catch ( IceUtil::Exception &e )
        {
            ss<<"WorldView: during painting of "
              <<elements[i]->details().toStdString()<<": " << e << std::endl;
            humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
        }
        catch ( std::exception &e )
        {
            ss<<"WorldView: during painting of "
              <<elements[i]->details().toStdString()<<": " << e.what() << std::endl;
            humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
        }
        catch ( std::string &e )
        {
            ss<<"WorldView: during painting of "
              <<elements[i]->details().toStdString()<<": " << e << std::endl;
            humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
        }
        catch ( char *e )
        {
            ss<<"WorldView: during painting of "
              <<elements[i]->details().toStdString()<<": " << e << std::endl;
            humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
        }
        catch ( ... )
        {
            ss<<"WorldView: during painting of "
              <<elements[i]->details().toStdString()<<": unknown exception." << std::endl;
            humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
        }
    }
}

void 
WorldView::setAntiAliasing(bool antiAliasing)
{
    isAntialiasingEnabled_ = antiAliasing;

    QString str;
    if (antiAliasing==false) {
        str = "off";
    } else {
        str = "on";
    }
    
    humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Information, "Anti-aliasing is turned " + str);   
}

void 
WorldView::keyPressEvent(QKeyEvent *e)
{
    cout<<"TRACE(worldview.cpp): "<<__func__ << endl;
    const bool needUpdate = viewHandler_.keyPressEvent(e);
    if ( needUpdate )
        updateGL();
    else
        QGLWidget::keyPressEvent(e);
}

void
WorldView::mousePressEvent( QMouseEvent* e )
{ 
    cout<<"TRACE(worldview.cpp): "<<__func__ << endl;
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        mouseEventManager_.mouseEventReceiver()->mousePressEvent( e );
        return;
    }
    else
    {
        //cout<<"TRACE(worldview.cpp): Using mousePressEvent ourself" << endl;
        viewHandler_.mousePressEvent(e);
    }
}

void
WorldView::mouseMoveEvent( QMouseEvent* e )
{
    cout<<"TRACE(worldview.cpp): "<<__func__ << endl;
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        //mouseEventManager_.mouseEventReceiver()->mouseMoveEvent( e );
        return;
    }
    else
    {
        const bool needUpdate = viewHandler_.mouseMoveEvent(e);
        if ( needUpdate )
            updateGL();
    }
}

void
WorldView::mouseReleaseEvent( QMouseEvent* e )
{
    cout<<"TRACE(worldview.cpp): "<<__func__ << endl;
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
    cout<<"TRACE(worldview.cpp): "<<__func__ << endl;
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
        mouseEventManager_.mouseEventReceiver()->mouseDoubleClickEvent( e );
}


} // namespace
