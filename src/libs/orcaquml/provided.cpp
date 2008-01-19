/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QtGui>

#include "provided.h"
#include "component.h"
#include "link.h"
#include "model.h"

// debug
#include <iostream>

using namespace std;
using namespace orcaquml;

QPen Provided::_Pen( Qt::black );
QBrush Provided::_Brush( QColor( 252, 242, 227 ) ); //QColor( 252/2, 242/2, 227/2 ) );


Provided::Provided( Component* parent, 
                    QGraphicsScene* scene,
                    orcadef::ProvidedDef &def,
                    orcadef::ProvidedCfg &cfg,
                    InterfaceGeom        &geom )
        : Interface( parent, scene, def, cfg, geom ), 
          def_(def),
          cfg_(cfg)
{
    setToolTip( QString("provided: ")+interface() );

    setLabel();
}

void 
Provided::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    
    p->setPen( _Pen );
    p->setBrush( _Brush );
    
    //cout<<"Provided::drawShape"<<endl;
    p->setRenderHint( QPainter::Antialiasing );
    p->setRenderHint( QPainter::TextAntialiasing );

//     drawDebug( p );
    drawStem( p );
    drawBubble( p );
}

void
Provided::drawBubble( QPainter* p )
{
    p->save();

    double radius = Interface::_bubbleRadius;
    double diameter = 2.0*radius;
    switch ( orientation() )
    {
    case OrientUp :
        p->drawEllipse( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter) );
        break;
    case OrientDown :
        p->drawEllipse( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter) );
        break;
    case OrientLeft :
        p->drawEllipse( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter) );
        break;
    case OrientRight :
        p->drawEllipse( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter) );
        break;
    }

    p->restore();
}

void 
Provided::setLabel() {
    QString s;
    bool someText = false;
    
    if ( _showInterfaceName ) {
        s += interface();
        someText = true;
    }
    if ( _showServiceName ) 
    {
        if ( someText ) 
        {
            s += "\n";  
        }
        s += name();
        someText = true;
    }
    if ( _showTag ) 
    {
        if ( someText ) 
        {
            s += "\n";  
        }
        s += ( "\"" + tag() + "\"" );
    }
    label_->setText( s );
}

// const std::string 
// Provided::description() const
// {
//     std::stringstream ss;
// 
//     ss<<"Provided "<<Interface::description();
// 
//     return ss.str();
// }
