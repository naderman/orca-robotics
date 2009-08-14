/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QTimer>
#include <QToolTip>
#include <gbxutilacfr/mathdefs.h>
#include <hydroqguielementutil/definitions2d.h>
#include <hydroqguielementutil/paintutils.h>
#include <hydroqguielementutil/guielement2d.h>
#include <hydroqguielementutil/iknowsplatformposition2d.h>
#include <hydroqgui/platformcsfinder.h>
#include "worldview.h"

using namespace std;

namespace hydroqgui {
    
XYDisplay::XYDisplay()
{
    // we want to use standard text independently from any parent
    // (in particular, when our parent is a StatusBar whose font color
    // changes when warns/erros are displayed.)
    QPalette pal;
    setPalette( pal );
}

void
XYDisplay::setCoordinates( double x, double y )
{
    setText( "x: " + QString::number(x,'f',3) + " y:" + QString::number(y,'f',3) );
}

/////////////////////////////////

WorldView::WorldView(  PlatformCSFinder                          &platformCSFinder,
                       ::hydroqguielementutil::MouseEventManager &mouseEventManager,
                       GuiElementSet                             &guiElementSet,
                       CoordinateFrameManager                    &coordinateFrameManager,
                       ::hydroqguielementutil::IHumanManager     &humanManager,
                       PlatformFocusManager                      &platformFocusManager,
                       int                                        displayRefreshTime,
                       const Config                              &cfg,
                       QWidget                                   *parent )
    : ZoomWidget(parent),
      mouseEventManager_(mouseEventManager),
      guiElementSet_(guiElementSet),
      platformCSFinder_(platformCSFinder),
      coordinateFrameManager_(coordinateFrameManager),
      humanManager_(humanManager),
      platformFocusManager_(platformFocusManager),
      antiAliasing_(cfg.initWithAntiAliasing)
{   

    // Use mouse tracking to show tooltips with robot names
    setMouseTracking(true);

    setupInterface(cfg);

    // zoom and move according to configuration
    zoomFitWin( cfg.initialWorldWidth );
    moveViewWorldCs( QPointF( cfg.initialWorldWidth/2.0, -cfg.initialWorldWidth/2.0 ) );

    update();

    displayTimer_ = new QTimer( this );
    QObject::connect( displayTimer_,SIGNAL(timeout()), this,SLOT(reDisplay()) );
    displayTimer_->start( displayRefreshTime );
}

void
WorldView::reDisplay()
{
    updateAllGuiElements();

    // issue a QPaintEvent, all painting must be done from paintEvent().
    update();    
}

void
WorldView::updateAllGuiElements()
{
    for ( int i=0; i<guiElementSet_.elements().size(); ++i )
    {
        ::hydroqguielementutil::GuiElement *element = guiElementSet_.elements()[i];

        if ( element ) {
            std::stringstream ss;
            try {
                element->update();
            }
            catch ( std::exception &e )
            {
                ss<<"WorldView: during update of "
                        <<element->uniqueId().toStdString()<<": " << e.what() << std::endl;
                humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
            catch ( ... )
            {
                ss<<"WorldView: Caught unknown exception during update of "
                        <<element->uniqueId().toStdString()<<": " << std::endl;
                humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
            }
        }
    }
}

void
WorldView::setupInterface( const Config &cfg )
{
    QAction* antiAliasing = new QAction(tr("&Anti-Aliasing"),this);
    antiAliasing->setCheckable(true);
    antiAliasing->setShortcut( QKeySequence("Ctrl+Alt+A") );
    connect(antiAliasing,SIGNAL(toggled(bool)), this, SLOT(setAntiAliasing(bool)) );
    humanManager_.displayMenu()->addAction(antiAliasing);
    antiAliasing->setChecked(cfg.initWithAntiAliasing);
    
    QAction* transparency = new QAction(tr("&Transparency"),this);
    transparency->setCheckable(true);
    guiElementSet_.setUseTransparency(cfg.initWithTransparency);
    transparency->setShortcut( QKeySequence("Ctrl+Alt+T") );
    connect(transparency,SIGNAL(toggled(bool)), this, SLOT(setUseTransparency(bool)) );
    humanManager_.displayMenu()->addAction(transparency);   
    transparency->setChecked(cfg.initWithTransparency);

    // add widgets to the status bar to display real-time x,y coordinates of the mouse.
    xyDisplay_ = new XYDisplay();
    humanManager_.statusBar()->addPermanentWidget( xyDisplay_ );
    xyDisplay_->setCoordinates(0,0);
}

bool
WorldView::transformToPlatformOwningCS( QPainter *p )
{    
    bool gotValidCS=true;
    
    QMatrix platformCsMatrix;
    
    // special case: global CS
    if ( coordinateFrameManager_.coordinateFramePlatform() == "global" )
    {
        // TODO: should we set to identity here??
        platformCsMatrix = QMatrix();
    }
    else
    {
        float x, y, theta;
        if ( platformCSFinder_.findPlatformCS( guiElementSet_.elements(),
                                               coordinateFrameManager_.coordinateFramePlatform(),
                                               x,
                                               y,
                                               theta ) )
        {
            QMatrix m;
            m.translate( x, y);
            if (!coordinateFrameManager_.ignoreCoordinateFrameRotation())
                m.rotate( RAD2DEG(theta) + 90);
            platformCsMatrix = m.inverted();
        }
        else
        {
            gotValidCS=false;
        }
    }
    
    if ( platformCsMatrix != QMatrix() ) {
        bool combine = true;
        p->setWorldMatrix( platformCsMatrix, combine );
    }
    
    return gotValidCS;
}

void
WorldView::paintEvent( QPaintEvent* e )
{
    // instantiate a painter every time an event occurs
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,antiAliasing_);

    // set world matrix
    painter.setWorldMatrix( wm_ );

    const bool isCoordinateFramePlatformLocalised = transformToPlatformOwningCS( &painter );

    // simple z-buffering, higher-z items obscure (are in front of) lower-z items (just like QCanvas)
    for ( int z=::hydroqguielementutil::Z_BACKGROUND; z<=::hydroqguielementutil::Z_FOREGROUND; ++z )
    {
        paintAllGuiElements(&painter,z,isCoordinateFramePlatformLocalised);
    }
}

void
WorldView::paintAllGuiElements( QPainter *painter, int z, bool isCoordinateFramePlatformLocalised )
{
    const QList< ::hydroqguielementutil::GuiElement*> &elements = guiElementSet_.elements();

    for ( int i=0; i<elements.size(); ++i )
    {
        if ( !elements[i] ) {
            continue;
        }

        stringstream ss;
        try {
            // paint all elements in the world if platform that owns coordinate system is localised 
            // also paint all elements of the platform that owns coordinate system even if it's not localised
            // always paint the permanent elements                     
            if ( isCoordinateFramePlatformLocalised || 
                 elements[i]->platform()==coordinateFrameManager_.coordinateFramePlatform() || 
                 elements[i]->isPermanentElement() ) 
            {
                // check whether we have a GuiElement2d. If yes, we paint it. If no, proceed to the next element.
                ::hydroqguielementutil::GuiElement2d *elem = 
                      dynamic_cast< ::hydroqguielementutil::GuiElement2d*>(elements[i]);
                if (elem==NULL) continue;

                if ( !elem->paintThisLayer(z) ) continue;

                // Save the state of the painter with ScopedSaver, in case an exception is thrown.
                ::hydroqguielementutil::ScopedSaver scopedPainterSaver( painter );

                if ( elem->isInGlobalCS() )
                {
                    elem->paint( painter, z );
                }
                else
                {
                    // This GuiElement is in a local coordinate system: find the platform's CS
                    float x, y, theta;
                    const bool platformCSFound = platformCSFinder_.findPlatformCS( guiElementSet_.elements(), 
                                                                                   elements[i]->platform(), 
                                                                                   x, y, theta );
                    if ( !platformCSFound )
                    {
                        // Can't find the location of this platform -- paint in the global CS
                        //cout<<"TRACE(worldview.cpp): Warning: couldn't find CS of element '"<<elements[i]->uniqueId().toStdString()<<"' on platform '"<<elements[i]->platform().toStdString()<<"', painting in global CS." << endl;
                        elem->paint( painter, z );
                    }
                    else
                    {
                        painter->translate( x, y );
                        painter->rotate( RAD2DEG(theta));
                        elem->paint( painter, z );
                    }
                }
            }
        }
        catch ( std::exception &e )
        {
            ss<<"WorldView: during painting of "
              <<elements[i]->uniqueId().toStdString()<<": " << e.what() << std::endl;
            humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
        }
        catch ( ... )
        {
            ss<<"WorldView: during painting of "
              <<elements[i]->uniqueId().toStdString()<<": unknown exception." << std::endl;
            humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Warning,ss.str().c_str());
        }
    }
}

// COMMON VIEW MANIPULATION FUNCTIONS ====================

void
WorldView::mousePressEvent( QMouseEvent* e )
{
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        mouseEventManager_.mouseEventReceiver()->mousePressEvent( e );
        return;
    }
    else
    {
        mouseDownPnt_ = e->pos();
        prevDragPos_ = e->pos();
    }
}

void
WorldView::mouseMoveEvent( QMouseEvent* e )
{
//     mouseDownPnt_ = e->pos();

    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        //mouseEventManager_.mouseEventReceiver()->mouseMoveEvent( e );
        return;
    }

    // mouse position in the world CS.
    QPointF pMouse = QPointF( e->pos() ) * wm_.inverted();

    if ( e->buttons() & Qt::LeftButton )
    {
        QPoint diff = e->pos() - prevDragPos_;
        moveView( diff );
        prevDragPos_ = e->pos();
        update();
    }
    else if ( e->buttons() & Qt::RightButton )
    {
        QPoint diff = e->pos() - prevDragPos_;
        double zoomAmount = (75.0-diff.y())/75.0;
        const double maxAmount = 1.2;
        const double minAmount = 0.8;
        if ( zoomAmount < minAmount ) zoomAmount = minAmount;
        if ( zoomAmount > maxAmount ) zoomAmount = maxAmount;
        zoomView( zoomAmount );
        prevDragPos_ = e->pos();
        xyDisplay_->setCoordinates( pMouse.x(), pMouse.y() );
        update();
    }
    else
    {
        // just update xy display when no buttons are pressed 
        xyDisplay_->setCoordinates( pMouse.x(), pMouse.y() );
    }
}

void
WorldView::mouseReleaseEvent( QMouseEvent* e )
{
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
    {
        mouseEventManager_.mouseEventReceiver()->mouseReleaseEvent( e );
        return;
    }

    QPointF mouseUpPnt = e->pos();

    if ( e->button() == Qt::LeftButton )          // left button released
    {  
        // select component
        const double SELECTED_RADIUS_PIXEL = 20.0;
        QString platform = nearestComponent( mouseDownPnt_, SELECTED_RADIUS_PIXEL);
        //cout << "TRACE(worldview.cpp): nearestComponent is: " << platform.toStdString() << endl;
        // alert others
        if ( platform != "" )
        {
            platformFocusManager_.setFocusPlatform( platform );
        }
    } 
}

void
WorldView::mouseDoubleClickEvent( QMouseEvent* e )
{
    if ( mouseEventManager_.mouseEventReceiverIsSet() )
        mouseEventManager_.mouseEventReceiver()->mouseDoubleClickEvent( e );
}

void
WorldView::zoomFitWin( double width )
{
    const double worldWidth = width;  // [m]
    const double worldHeight = width; // [m]

    zoomWorldSize( QSizeF(worldWidth,worldHeight) );
}

void
WorldView::zoomAllPix()
{
    const double pixelSize = 0.10; // [m]
    
    zoomPixelSize( QSizeF(pixelSize,pixelSize) );
}

QString 
WorldView::nearestComponent( const QPointF& pclick, const double & pixelRadius )
{
    const QList< ::hydroqguielementutil::GuiElement*> &elements = guiElementSet_.elements();
    
    // potential candidates are stored with distance/platformName key/value pairs
    QMap<double,QString> candidates;
    
    QPointF delta;
    double dist;
    
    for ( int i=0; i<elements.size(); ++i )
    {
        // only certain types are real 'platform elements' 
        const ::hydroqguielementutil::IKnowsPlatformPosition2d* platformElem = 
                dynamic_cast<const ::hydroqguielementutil::IKnowsPlatformPosition2d*>(elements[i]);
        if ( platformElem != NULL )
        {
            //cout << "TRACE(worldview.cpp): We have a platform element" << endl;
            // We know that really it's a GuiElement2d...
            const ::hydroqguielementutil::GuiElement2d *elem2d = 
                    dynamic_cast<const ::hydroqguielementutil::GuiElement2d*>(elements[i]);
            assert( elem2d != NULL );
            //cout << "TRACE(worldview.cpp): pClick is " << pclick.x() << " " << pclick.y() << endl;
            //cout << "TRACE(worldview.cpp): elem2d->pos() is " << elem2d->pos().x() << " " << elem2d->pos().y() << endl;
            //cout << "TRACE(worldview.cpp): elem2d->pos() transformed is " << wm_.map(elem2d->pos()).x() << " " << wm_.map(elem2d->pos()).y() << endl;
            delta = pclick - wm_.map(elem2d->pos());
            dist = sqrt( pow(delta.x(),2.0) + pow(delta.y(),2.0) );
            if ( dist<pixelRadius ) candidates[dist] = elements[i]->platform();        
        }
    }
    
    // if user clicked around the origin (0,0) we switch to a global view (all platforms in focus)
//     delta = pclick - wm_.map(QPointF());
//     dist = sqrt( pow(delta.x(),2.0) + pow(delta.y(),2.0) );
//     if (dist<pixelRadius) candidates[dist] = "global";
    
    // return the closest candidate, ie the one with the lowest key (QMaps are sorted by key)
    if (!candidates.isEmpty())
    {
        QMap<double,QString>::const_iterator i = candidates.constBegin();
        return i.value();
    }
    
    // neither a platform nor (0,0) was close to where the user clicked
    return "";
}

void 
WorldView::setAntiAliasing(bool antiAliasing)
{
    antiAliasing_ = antiAliasing;

    QString str;
    if (antiAliasing==false) {
        str = "off";
    } else {
        str = "on";
    }
    
    humanManager_.showStatusMsg(::hydroqguielementutil::IHumanManager::Information, "Anti-aliasing is turned " + str);   
}

void
WorldView::setUseTransparency(bool useTransparency)
{
    guiElementSet_.setUseTransparency(useTransparency);
}

//
// This code causes problems
//

// bool WorldView::event(QEvent *event)
// {
//     if (event->type() == QEvent::ToolTip)
//     {
//         QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
//         const double SELECTED_RADIUS_PIXEL = 20.0;
//         QString platform = nearestComponent( mouseMovePnt_, SELECTED_RADIUS_PIXEL );
//         if (platform!="") QToolTip::showText(helpEvent->globalPos(), platform);
//     }
//     return QWidget::event(event);
// }

}
