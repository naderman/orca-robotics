/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <QPainter>

#include "link.h"
#include "interface.h"

using namespace orcaquml;

bool Link::_showInterfaceName( false );

QPen Link::_defaultPen( Qt::black, 1, Qt::DashLine );
QPen Link::_selectedPen( Qt::black, 2, Qt::DashLine );

Link::Link( Interface *from, Interface *to )
        : from_(from), to_(to)
{
    setPen( _defaultPen );

    from_->addOutEdge( this );
    to_->addInEdge( this );

    // make port label (before motion starts)
    label_ = new QGraphicsSimpleTextItem( "LinkLabel", this );
    // assume here that from_ and to_ have the same signature (should've been tested already)
//     label_->setText( from_->signature() );
//     label_->setTextFlags( Qt::AlignVCenter | Qt::AlignHCenter );
//     label_->setColor( Qt::gray );
    // set visibility according to global settings
    setLabel();

    setLine( from_->bubblePos().x(),from_->bubblePos().y(),to_->bubblePos().x(),to_->bubblePos().y() );
    setZValue( from_->zValue()-5.0 );

    // move the text into position, after moving has settled down
    label_->setZValue( zValue()+2.0 );
    placeLabelAuto();
    // set visibility according to global settings
    setLabel();
}

void 
Link::prepareToDie()
{
    from_->removeOutEdge( this );
    to_->removeInEdge( this );

    delete label_;
}

void 
Link::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
    QGraphicsLineItem::paint( painter, item, widget );

    // mid-point
    //QPoint midpoint = ( startPoint() + endPoint() ) / 2;

    // draw component and host names
    //p.drawText( midpoint, "ServerPush<Pose2d>" );
}

void 
Link::placeLabelAuto() {
    // mid-point
    QPointF midpoint = ( line().p1() + line().p2() ) / 2;
    label_->moveBy( midpoint.x(), midpoint.y() );
}

void 
Link::setSelected( bool yes ) {
//     Q3CanvasItem::setSelected( yes );    

    if ( isSelected() ) {
        setPen( _selectedPen );
    } else {
        setPen( _defaultPen );
    }
    update();
};

void 
Link::setLabel() {
    if ( _showInterfaceName == true ) {
        label_->show();
    } else {
        label_->hide();
    }
}

void 
Link::getFromInterfaceNaming( QString & platform, QString & name )
{
    from_->getNaming( platform, name );  
}

void 
Link::getToInterfaceNaming( QString & platform, QString & name )
{
    to_->getNaming( platform, name );  
}

void 
Link::setFromPoint( double x, double y ) {
    setLine( x,y, line().p2().x(), line().p2().y() );
}

void 
Link::setToPoint( double x, double y ) {
    setLine( line().p1().x(), line().p1().y(), x, y );
}
