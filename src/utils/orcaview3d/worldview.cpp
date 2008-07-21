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
#include <orcaqgui3d/osgutil.h>

using namespace std;

namespace orcaview3d {

namespace {

    osg::LightSource *getLighting()
    {
        osg::ref_ptr<osg::Light> light = new osg::Light;
        light->setLightNum(0);
        osg::Vec4f lightPosition (osg::Vec4f(0,0,30,1.0f));
//         light->setPosition(lightPosition);
//         light->setAmbient(osg::Vec4(1.0f,1.0f,1.0f,1.0f));
//         myLight->setAmbient(osg::Vec4(0.2f,0.2f,0.2f,1.0f));
//         myLight->setDiffuse(osg::Vec4(0.1f,0.4f,0.1f,1.0f));
//         myLight->setConstantAttenuation(1.0f);

        osg::LightSource *lightSource = new osg::LightSource;
        lightSource->setLight(light.get());
        lightSource->setLocalStateSetModes(osg::StateAttribute::ON); 
        return lightSource;

//        osg::ref_ptr<osg::StateSet> lightSS (root->getOrCreateStateSet());
//        lightSource->setStateSetModes(*lightSS,osg::StateAttribute::ON);
       
//         osg::Group *lightGroup(new osg::Group);
//         lightGroup->addChild(lightSource.get());
//         return lightGroup;
    }

}


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

    lightSource_ = getLighting();
}

orcaqgui3d::CoordinateFrame
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
            orcaqgui3d::CoordinateFrame cf( orcaqgui3d::Vector3(x,y,z), roll,pitch,yaw );

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
    sceneView_ = new osgUtil::SceneView;
    sceneView_->setDefaults();

    // Set the default (background) color
    sceneView_->setClearColor( osg::Vec4( 0.7, 0.7, 0.7, 1.0 ) );
}

void 
WorldView::resizeGL(int w, int h)
{
    // sets the perspective:
    //  (left,right,bottom,top,near,far)
    sceneView_->setProjectionMatrixAsFrustum( -1.0, 1.0, -1.0, 1.0, 1.0, 200.0 );

    // define the viewport
    sceneView_->setViewport( 0, 0, w, h );
}

void 
WorldView::paintGL()
{
    // Work out where to put the camera
    bool isCameraPoseLocalised = false;
    cameraPose_ = getCameraPose( isCameraPoseLocalised );

    // Put the camera in position
    orcaqgui3d::Vector3 center = cameraPose_.pos() + cameraPose_.fwd();
    sceneView_->setViewMatrixAsLookAt( osg::Vec3( cameraPose_.pos().x(),
                                                  cameraPose_.pos().y(),
                                                  cameraPose_.pos().z() ),
                                       osg::Vec3( center.x(),
                                                  center.y(),
                                                  center.z() ),
                                       osg::Vec3( cameraPose_.up().x(),
                                                  cameraPose_.up().y(),
                                                  cameraPose_.up().z() ) );

    osg::ref_ptr<osg::Group> root = new osg::Group();

    // Add lights
    root->addChild( lightSource_.get() );
    osg::ref_ptr<osg::StateSet> lightSS(root->getOrCreateStateSet());
    lightSource_->setStateSetModes(*lightSS,osg::StateAttribute::ON);

    paintAllGuiElements( isCameraPoseLocalised, root.get() );

    sceneView_->setSceneData( root.get() );

    // do the update traversal the scene graph - such as updating animations
    sceneView_->update();

    // do the cull traversal, collect all objects in the view frustum 
    // into a sorted set of rendering bins
    sceneView_->cull();
            
    // draw the rendering bins.
    sceneView_->draw();
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
WorldView::paintAllGuiElements( bool isCoordinateFramePlatformLocalised, osg::Group *root )
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
                if ( elem->isInGlobalCS() )
                {
                    elem->paint( *this, *this );
                    root->addChild( elem->osgNode() );
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
                        root->addChild( elem->osgNode() );
                    }
                    else
                    {
                        osg::ref_ptr<osg::PositionAttitudeTransform> pos = 
                            orcaqgui3d::getPositionAttitudeTransform( x, y, z,
                                                                      roll, pitch, yaw );
                        elem->paint( *this, *this );
                        root->addChild( pos.get() );
                        pos->addChild( elem->osgNode() );
                    }
                }
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

// void 
// WorldView::setAntiAliasing(bool antiAliasing)
// {
//     isAntialiasingEnabled_ = antiAliasing;

//     QString str;
//     if (antiAliasing==false) {
//         str = "off";
//     } else {
//         str = "on";
//     }
    
//     humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Information, "Anti-aliasing is turned " + str);   
// }

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
