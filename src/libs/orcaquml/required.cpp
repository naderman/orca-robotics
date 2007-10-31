/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <QtGui>

#include "required.h"
#include "component.h"
#include "link.h"
#include "model.h"
#include "provided.h"

// debug
#include <iostream>
#include <orcaobj/orcaobj.h>    // just for toString()

using namespace std;
using namespace orcaquml;

QPen Required::_validPen( Qt::black );
QBrush Required::_validBrush( QColor( 252, 242, 227 ) );
QPen Required::_invalidPen( Qt::black );
QBrush Required::_invalidBrush( QColor( 255, 255, 51 ) );

Required::Required( Component* parent, 
                    QGraphicsScene* scene,
                    orcadef::RequiredDef &def,
                    orcadef::RequiredCfg &cfg,
                    InterfaceGeom        &geom )
        : Interface( parent, scene, def, cfg, geom ), 
          def_(def),
          cfg_(cfg)
{
    setToolTip( QString("required: ")+interface() );

    setLabel();
}

void 
Required::checkValid() {
    if ( isNecessary_ && !isLinked() ) {
        setValid( false );
    } else {
        setValid( true );
    }
}

void 
Required::setValid( bool yes ) {
    if ( yes ) {
        setPen( _validPen );
        setBrush( _validBrush );
    } else {
        setPen( _invalidPen );
        setBrush( _invalidBrush );
    }
}

/*!
    On link event, looks up the naming settings of the server and sets local variables accordingly.
 */
void 
Required::linkEvent( Interface &toInterface )
{
    cout<<"Required: handling link event"<<endl;
//     cout<<"  Linking to Interface: "<<toInterface.description() << endl;
//     cout<<"  On: " << toInterface.parent().description() << endl;
    cout<<"  With cfg: "<<endl<<orcadef::toString(toInterface.parent().cfg())<<endl;
    cout<<"  With fqname: "<<orcaice::toString(toInterface.parent().cfg().fqname)<<endl;

    cfg_.proxy  = ((Provided&)toInterface).cfg().name + "@" +
                    toInterface.parent().cfg().fqname.platform + "/" +
                    toInterface.parent().cfg().fqname.component;

    cout<<"TRACE(qorcarequired.cpp): After linking, our cfg looks like:" << endl;
    cout<<orcadef::toString(cfg_)<<endl;

    // 'in' means, I am 'to', means we need 'from'
    if ( !inLinks_.isEmpty() ) 
    {
        inLinks_.first()->getFromInterfaceNaming( nsPlatform_, nsName_ );
        return;
    }
    // 'out' means, I am 'from', means we need 'to'
    if ( !outLinks_.isEmpty() ) 
    {
        outLinks_.first()->getToInterfaceNaming( nsPlatform_, nsName_ );
        return;
    }
    setLabel();
}

void 
Required::paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    p->setPen( _validPen );
    p->setBrush( _validBrush );
    
    p->setRenderHint( QPainter::Antialiasing );
    p->setRenderHint( QPainter::TextAntialiasing );

//     drawDebug( p );
    drawStem( p );
    drawBubble( p );
}

void
Required::drawBubble( QPainter* p )
{
    p->save();

    double radius = Interface::_bubbleRadius;
    double diameter = 2.0*radius;
    int spanAngle = 180*16; // 180deg measured in 1/16 of a degree.
    int startAngle; // depends on the orientation
    switch ( orientation() )
    {
    case OrientUp :
        startAngle = 0;
        p->drawArc( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter), startAngle, spanAngle );
        break;
    case OrientDown :
        startAngle = 180*16;
        p->drawArc( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter), startAngle, spanAngle );
        break;
    case OrientLeft :
        startAngle = -90*16;
        p->drawArc( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter), startAngle, spanAngle );
        break;
    case OrientRight :
        startAngle = 90*16;
        p->drawArc( QRectF(bubblePos().x()-radius, bubblePos().y()-radius, diameter,diameter), startAngle, spanAngle );
        break;
    }    

    p->restore();
}

void 
Required::setLabel() {
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
// Required::description() const
// {
//     std::stringstream ss;
// 
//     ss<<"Required "<<Interface::description();
// 
//     return ss.str();
// }
