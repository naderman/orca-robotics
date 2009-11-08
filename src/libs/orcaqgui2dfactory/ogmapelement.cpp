/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "ogmapelement.h"
#include <iostream>
#include <QFileDialog>
#include <hydroqgui/exceptions.h>

using namespace std;

namespace orcaqgui2d {

OgMapElement::OgMapElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                            const orcaice::Context  &context )
    : orcaqguielementutil::IceStormGuiElement2d<OgMapPainter,
                      orca::OgMapData,
                      orca::OgMapPrx,
                      orca::OgMapConsumer,
                      orca::OgMapConsumerPrx>(guiElementInfo, context, painter_, -1 ),
      mapFileName_("/home"),
      mapFileNameSet_(false),
      ownMouseEvents_(false)
{
}

QStringList
OgMapElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Map"<<"Start Painting"<<"Stop Painting"<<"Save Map"<<"Save Map As ...";
    return s;
}

void
OgMapElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
        painter_.toggleDisplayMap();
        break;
    case 1 :
        startPainting();
        break;
    case 2 :
        stopPainting();
        break;
    case 3 :
        saveMap();
        break;
    case 4 :
        saveMapAs();
        break;
    default:
        throw hydroqgui::Exception( ERROR_INFO, "OgMapElement::execute(): What the hell? bad action." );
    }
}

void
OgMapElement::startPainting()
{
    if ( ownMouseEvents_ )
        return;

    ownMouseEvents_ = _stuff.mouseEventManager->requestBecomeMouseEventReceiver( this );

    if ( !ownMouseEvents_ )
        _stuff.humanManager->showStatusWarning( "Couldn't take over the mouse for PathFollower waypoints!" );
}

void
OgMapElement::stopPainting()
{
    if ( !ownMouseEvents_ )
        return;

    _stuff.mouseEventManager->relinquishMouseEventReceiver( this );
    ownMouseEvents_ = false;
}

void OgMapElement::saveMapAs()
{
    mapFileName_ = QFileDialog::getSaveFileName(
        0,
        //this,
        "Choose a filename to save under",
        mapFileName_,
        "*.png *.bmp *.jpg *.jpeg *.ppm *.xbm *.xpm *.bin");

    if ( mapFileName_ != "" )
    {
        painter_.saveMap( context_, mapFileName_, _stuff.humanManager );
        mapFileNameSet_ = true;
    }
}

void OgMapElement::saveMap()
{
    if (!mapFileNameSet_)
    {
        saveMapAs();
    }
    else
    {
        painter_.saveMap( context_, mapFileName_, _stuff.humanManager );
    }
}

void
OgMapElement::noLongerMouseEventReceiver()
{
    if ( ownMouseEvents_ ) {
        _stuff.mouseEventManager->relinquishMouseEventReceiver( this );
        ownMouseEvents_ = false;
    }
}

void
OgMapElement::mousePressEvent(QMouseEvent *e)
{
    cout<<"mousePressEvent"<<endl;

    // assume circular brush with a diameter
    qreal diameter = 10.0;
    painter_.drawCircle ( e->pos(), diameter );
}

void
OgMapElement::mouseMoveEvent(QMouseEvent *e)
{
    cout<<"mouseMoveEvent"<<endl;
}

void
OgMapElement::mouseReleaseEvent(QMouseEvent *e)
{
    cout<<"mouseReleaseEvent"<<endl;
}

void
OgMapElement::mouseDoubleClickEvent(QMouseEvent *e)
{
    cout<<"mouseDoubleClickEvent"<<endl;
}

}
