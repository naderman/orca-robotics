/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream> // debugging only
#include <cmath>  // for floor()
#include <map>
#include <orcaobj/mathdefs.h>

#include <QPainter>
#include <QAction>
#include <QTimer>
#include <QPaintEvent>
#include <QInputDialog>
#include <QStatusBar>
#include <QDialog>
#include <QToolTip>

#include <orcaqgui2dfactory/gridelement.h>
#include "worldview.h"
#include <orcaqgui/guielementmodel.h>
#include <orcaqgui/mainwin.h>
#include <orcaqgui/guiicons.h>
#include <orcaqgui2d/definitions2d.h>
#include <orcaqgui2d/platformcsfinder.h>
#include <orcaqgui2d/iknowsplatformposition2d.h>

using namespace std;
using namespace orcaqgui;

namespace orcaqgui2d {
    
WorldView::WorldView( PlatformCSFinder* platformCSFinder,
                      GuiElementModel* model,
                      QWidget* parent, 
                      MainWindow* mainWin )
    : ZoomWidget(parent),
      platformCSFinder_(platformCSFinder),
      model_(model),
      mainWin_(mainWin),
      antiAliasing_(false)
{
    setMinimumSize( 600, 600 );
    setMaximumSize( 1400, 1050 );
 
    // Use mouse tracking to show tooltips with robot names.
    // TODO: make this configurable?
    //
    // Tobi: causes a segfault in qt version 4.3.0-2+b1 when calling event()
    //       disabling for now, have to investigate
    //       event() is called even with mouse tracking off 
    //          -> commenting out event() member function
    setMouseTracking(false);

    setupInterface();

    zoomFitWin();

    // Get the origin on-screen
    moveLeft();moveLeft();
    moveDown();moveDown();

    update();
}

WorldView::~WorldView()
{
}

void
WorldView::setupInterface()
{
    // view menu icons    
    QPixmap leftIcon( left_xpm );
    QPixmap rightIcon( right_xpm );    
    QPixmap upIcon( up_xpm );
    QPixmap downIcon( down_xpm );
    QPixmap zoomInIcon( zoomIn_xpm );
    QPixmap zoomOutIcon( zoomOut_xpm );

    QAction* moveUp = new QAction(upIcon, tr("Move &up"), this );
    connect(moveUp, SIGNAL(activated()), this, SLOT(moveUp()));
    moveUp->setShortcut( Qt::CTRL | Qt::Key_Up );
    QAction* moveDown = new QAction(downIcon, tr("Move &down"), this );
    connect(moveDown, SIGNAL(activated()), this, SLOT(moveDown()));
    moveDown->setShortcut( Qt::CTRL | Qt::Key_Down );
    QAction* moveLeft = new QAction(leftIcon, tr("Move &left"), this );
    connect(moveLeft, SIGNAL(activated()), this, SLOT(moveLeft()));
    moveLeft->setShortcut( Qt::CTRL | Qt::Key_Left );
    QAction* moveRight = new QAction(rightIcon, tr("Move &right"), this );
    connect(moveRight, SIGNAL(activated()), this, SLOT(moveRight()));
    moveRight->setShortcut( Qt::CTRL | Qt::Key_Right );
    QAction* zoomIn = new QAction(zoomInIcon, tr("Zoom &in"), this );
    connect(zoomIn, SIGNAL(activated()), this, SLOT(zoomIn()));
    zoomIn->setShortcut( Qt::CTRL | Qt::Key_Equal );
    QAction* zoomOut = new QAction(zoomOutIcon, tr("Zoom &out"), this );
    connect(zoomOut, SIGNAL(activated()), this, SLOT(zoomOut()));
    zoomOut->setShortcut( Qt::CTRL | Qt::Key_Minus );

    QAction* antiAliasing = new QAction(tr("&Anti-Aliasing"),this);
    antiAliasing->setCheckable(true);
    antiAliasing->setShortcut( QKeySequence("Ctrl+A") );
    connect(antiAliasing,SIGNAL(toggled(bool)), this, SLOT(setAntiAliasing(bool)) );
    mainWin_->displayMenu()->addAction(antiAliasing);
    
    QAction* transparency = new QAction(tr("&Transparency"),this);
    transparency->setCheckable(true);
    // initialize to false, could be configurable
    transparency->setChecked(false);
    model_->setTransparency( false );
    transparency->setShortcut( QKeySequence("Ctrl+T") );
    connect(transparency,SIGNAL(toggled(bool)), model_, SLOT(setTransparency(bool)) );
    mainWin_->displayMenu()->addAction(transparency);   

    mainWin_->displayMenu()->addAction(moveUp);
    mainWin_->displayMenu()->addAction(moveDown);
    mainWin_->displayMenu()->addAction(moveLeft);
    mainWin_->displayMenu()->addAction(moveRight);
    mainWin_->displayMenu()->addAction(zoomIn);
    mainWin_->displayMenu()->addAction(zoomOut);

//     mainWin_->addToToolbar(moveUp);
//     mainWin_->addToToolbar(moveDown);
//     mainWin_->addToToolbar(moveLeft);
//     mainWin_->addToToolbar(moveRight);
//     mainWin_->addToToolbar(zoomIn);
//     mainWin_->addToToolbar(zoomOut);
}

bool
WorldView::transformToPlatformOwningCS( QPainter *p )
{    
    bool flag=true;
    
    QMatrix platformCsMatrix;
    
    // special case: global CS
    if ( model_->coordinateFramePlatform() == "global" )
    {
        // TODO: should we set to identity here??
        platformCsMatrix = QMatrix();
    }
    else
    {
        float x, y, theta;
        
        if ( platformCSFinder_->findPlatformCS( model_->elements(), model_->coordinateFramePlatform(), x, y, theta ) )
        {
            QMatrix m;
            m.translate( x, y);
            if (!model_->ignoreCoordinateFrameRotation())
                m.rotate( RAD2DEG(theta) + 90);
            platformCsMatrix = m.inverted();
        }
        else
        {
            flag=false;;
        }
    }
    
    if ( platformCsMatrix != QMatrix() ) {
        bool combine = true;
        p->setWorldMatrix( platformCsMatrix, combine );
    }
    
    return flag;
}

void
WorldView::paintEvent( QPaintEvent* e )
{
    // instantiate a painter every time an event occurs
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,antiAliasing_);

    // set world matrix
    painter.setWorldMatrix( wm_ );

    bool isCoordinateFramePlatformLocalised = transformToPlatformOwningCS( &painter );

    // simple z-buffering, higher-z items obscure (are in front of) lower-z items (just like QCanvas)
    for ( int z=Z_BACKGROUND; z<=Z_FOREGROUND; z++ )
    {
        paintAllGuiElements(&painter,z,isCoordinateFramePlatformLocalised);
    }
}

void
WorldView::paintAllGuiElements( QPainter *painter, int z, bool isCoordinateFramePlatformLocalised )
{
    const QList<GuiElement*> &elements = model_->elements();

    for ( int i=0; i<elements.size(); ++i )
    {
        if ( !elements[i] ) {
            continue;
        }
        
        try {
           // paint all elements in the world if platform that owns coordinate system is localised 
           // also paint all elements of the platform that owns coordinate system even if it's not localised
           // always paint the permanent elements         
            
            IPermanentElement *permElement = dynamic_cast<IPermanentElement*>(elements[i]);
            
            if ( isCoordinateFramePlatformLocalised || elements[i]->platform()==model_->coordinateFramePlatform() || (permElement!=NULL) ) 
            {
                GuiElement2d *elem = dynamic_cast<GuiElement2d*>(elements[i]);
                assert(elem != NULL);

                if ( !elem->paintThisLayer(z) ) continue;

                if ( elem->isInGlobalCS() )
                {
                    elem->paint( painter, z );
                }
                else
                {
                    painter->save();
                    {
                        // if a localiser element is not found, the coords of the painter
                        // will not be transformed and the element will be painted at the
                        // saved coords
                        float x, y, theta;
                        if ( platformCSFinder_->findPlatformCS( model_->elements(), elements[i]->platform(), x, y, theta ) )
                        {
                            // cout<<"TRACE(worldview.cpp): transforming painter by " << x << ", " << y << ", " << theta*180.0/M_PI << endl;
                            painter->translate( x, y );
                            painter->rotate( RAD2DEG(theta));
                            elem->paint( painter, z );
                        }
                        else
                        {
                            painter->save();
                            // local view is aligned with y-axis
                            painter->rotate( 90.0 );
                            elem->paint( painter, z );
                            painter->restore();
                        }
                    }
                    painter->restore();
                }
            }
        }
        catch ( Ice::Exception &e )
        {
            std::cout<<"TRACE(worldview.cpp): Caught some ice exception during painting of "
                     <<elements[i]->details().toStdString()<<": " << e << std::endl;
        }
        catch ( std::exception &e )
        {
            std::cout<<"TRACE(worldview.cpp): Caught some std exception during painting of "
                    <<elements[i]->details().toStdString()<<": " << e.what() << std::endl;
        }
        catch ( std::string &e )
        {
            std::cout<<"TRACE(worldview.cpp): Caught std::string during painting of "
                    <<elements[i]->details().toStdString()<<": " << e << std::endl;
        }
        catch ( char *e )
        {
            std::cout<<"TRACE(worldview.cpp): Caught char * during painting of "
                    <<elements[i]->details().toStdString()<<": " << e << std::endl;
        }
        catch ( ... )
        {
            std::cout<<"TRACE(worldview.cpp): Caught some other exception during painting of "
                    <<elements[i]->details().toStdString()<< std::endl;
        }
    }
}

// COMMON VIEW MANIPULATION FUNCTIONS ====================

void
WorldView::mousePressEvent( QMouseEvent* e )
{
    if ( mainWin_->modeIsOwned() )
    {
        //cout<<"TRACE(worldview.cpp): giving mousePressEvent to mode owner" << endl;
        mainWin_->modeOwner()->mousePressEvent( e );
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
    if ( mainWin_->modeIsOwned() )
    {
        //mainWin_->modeOwner()->mouseMoveEvent( e );
        return;
    }
    else
        mouseMovePnt_ = e->pos();
    // cout << "MouseMoveEvent(x,y): " << e->pos().x() << " " << e->pos().y() << endl;
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
        update();
    }
}

void
WorldView::mouseReleaseEvent( QMouseEvent* e )
{
    if ( mainWin_->modeIsOwned() )
    {
        //cout<<"TRACE(worldview.cpp): Giving mouseReleaseEvent to mode owner" << endl;
        mainWin_->modeOwner()->mouseReleaseEvent( e );
        return;
    }

    //cout<<"TRACE(worldview.cpp): Using mouseReleaseEvent ourself." << endl;

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
            mainWin_->changePlatformFocusFromView( platform );
        }
    } 
}


void
WorldView::mouseDoubleClickEvent( QMouseEvent* e )
{
    if ( mainWin_->modeIsOwned() )
        mainWin_->modeOwner()->mouseDoubleClickEvent( e );
}

void
WorldView::zoomFitWin()
{
    const double worldWidth = 20.0;  // [m]
    const double worldHeight = 20.0; // [m]

    zoomWorldSize( QSizeF(worldWidth,worldHeight) );
}

void
WorldView::zoomAllPix()
{
    const double pixelSize = 0.10; // [m]
    
    zoomPixelSize( QSizeF(pixelSize,pixelSize) );
}

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

QString 
WorldView::nearestComponent( const QPointF& pclick, const double & pixelRadius )
{
    const QList<GuiElement*> &elements = model_->elements();
    
    // potential candidates are stored with distance/platformName key/value pairs
    QMap<double,QString> candidates;
    
    QPointF delta;
    double dist;
    
    for ( int i=0; i<elements.size(); ++i )
    {
        // only certain types are real 'platform elements' 
        const IKnowsPlatformPosition2d* platformElem = dynamic_cast<const IKnowsPlatformPosition2d*>(elements[i]);
        if ( platformElem != NULL )
        {
            //cout << "TRACE(worldview.cpp): We have a platform element" << endl;
            // We know that really it's a GuiElement2d...
            const GuiElement2d *elem2d = dynamic_cast<const GuiElement2d*>(elements[i]);
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
    delta = pclick - wm_.map(QPointF());
    dist = sqrt( pow(delta.x(),2.0) + pow(delta.y(),2.0) );
    if (dist<pixelRadius) candidates[dist] = "global";
    
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
    
    mainWin_->showStatusMsg(Information, "Anti-aliasing is turned " + str);   
}

}
