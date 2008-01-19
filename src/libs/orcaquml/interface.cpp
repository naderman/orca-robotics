/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <cmath>
#include <QtGui>

#include "interface.h"
#include "component.h"
#include "link.h"
#include "model.h"

using namespace std;
using namespace orcaquml;

double Interface::_stemSize( 18.0 );
double Interface::_bubbleRadius( 6.0 );
bool Interface::_showInterfaceName( true );
bool Interface::_showServiceName( false );
bool Interface::_showTag( false );

Interface::Interface( Component* parent, 
                        QGraphicsScene* scene,
                        orcadef::InterfaceDef &def,
                        orcadef::InterfaceCfg &cfg,
                        InterfaceGeom &geom )
        : QAbstractGraphicsShapeItem(parent,scene),
          parent_( parent ),
          def_(def),
          cfg_(cfg),
          geom_( geom )
{
    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptsHoverEvents(true);
    setCursor( Qt::OpenHandCursor );

    // make label (before motion starts)
    label_ = new QGraphicsSimpleTextItem( "Interface", this );
    this->scene()->addItem(label_);
    label_->setPen( QPen(Qt::darkGray) );
    label_->setToolTip( QString("label for ")+QString::fromStdString( def_.type ) );
    label_->setCursor( Qt::OpenHandCursor );

//     setPos( geom.x, geom.y );

    // make sure port is above the component (is visible)
    setZValue( parent_->zValue()+10.0 );

    // move the text into position, after moving has settled down
    label_->setZValue( zValue()+10.0 );
    placeLabelAuto();
}

void 
Interface::mouseMoveEvent ( QGraphicsSceneMouseEvent* e )
{
    QGraphicsItem::mouseMoveEvent( e );
    snapToParent();
}

QRectF 
Interface::boundingRect() const
{
    switch ( orientation_ )
    {
    case OrientUp :
        return QRectF( -_bubbleRadius, 0.0, 2.0*_bubbleRadius, _stemSize+2.0*_bubbleRadius );
    case OrientDown :
        return QRectF( -_bubbleRadius, -_stemSize-2.0*_bubbleRadius, 2.0*_bubbleRadius, _stemSize+2.0*_bubbleRadius );
    case OrientLeft :
        return QRectF( -_stemSize-2.0*_bubbleRadius, -_bubbleRadius, _stemSize+2.0*_bubbleRadius, 2.0*_bubbleRadius );
    case OrientRight :
        return QRectF( 0.0, -_bubbleRadius, _stemSize+2.0*_bubbleRadius, 2.0*_bubbleRadius );
    }
    return QRectF();
}

void 
Interface::prepareToDie() {
    // delete links 
    for ( int i=0; i<inLinks_.size(); ++i ) {
        inLinks_[i]->prepareToDie();
        delete inLinks_.at(i);
    }
    for ( int i=0; i<outLinks_.size(); ++i ) {
        outLinks_[i]->prepareToDie();
        delete outLinks_.at(i);
    }
    delete label_;
}

void 
Interface::addInEdge( Link* link ) {
    inLinks_.append( link );
    checkValid();
}
void 
Interface::addOutEdge( Link* link ) {
    outLinks_.append( link );
    checkValid();
}
void 
Interface::removeInEdge( Link* link ) {
    inLinks_.removeAll( link );
    checkValid();
}
void 
Interface::removeOutEdge( Link* link ) {
    outLinks_.removeAll( link );
    checkValid();
}

/*!
    Range to port in parent coord. sys. Measured from parent center to port center.
*/
double 
Interface::rangeToParent() const
{
    return sqrt( x()*x() + y()*y() );
}

/*!
    Bearing to port in parent coord. sys. Measured from parent center to port center.
*/
double 
Interface::bearingToParent() const
{
    return atan2( y(), x() );
}

QPointF 
Interface::bubblePos() const
{
    switch ( orientation_ )
    {
    case OrientUp :
        return QPointF( 0.0, _stemSize + _bubbleRadius );
    case OrientDown :
        return QPointF( 0.0, -_stemSize - _bubbleRadius );
    case OrientLeft :
        return QPointF( -_stemSize - _bubbleRadius, 0.0 );
    case OrientRight :
        return QPointF( _stemSize + _bubbleRadius, 0.0 );
    }   
    return QPointF();
}

/*!
    Use constraints to place the port on a bearing.
    Won't work if constraints in this->moveBy() change.
*/
void 
Interface::setBearingConstrained( double bear )
{
    setPos( 200.0*cos(bear), 200.0*sin(bear) );
    snapToParent();
}

/*!
    Constrain port motion by the boundary of the parent component.
*/
void 
Interface::snapToParent()
{
    // desired bearing relative to the parent
    double bwish = atan2( pos().y(), pos().x() );
    // parent diagonal bearing
    double pdiag = atan2( parent_->rect().height(), parent_->rect().width() );
    
    // right quadrant
    if ( bwish>=-pdiag && bwish<pdiag ) {
        setPos( parent_->rect().width()/2.0, parent_->rect().width()/2.0*tan(bwish) );
        orientation_ = OrientRight;
//         cout<<"DEBUG: "<<def_.type<<" right ("<<pos().x()<<":"<<pos().y()<<":"<<bwish<<")"<<endl;
    }
    // left quadrant (notice OR because the set is discontinuous)
    else if ( bwish>=(M_PI-pdiag) || bwish<(-M_PI+pdiag) ) {
        setPos( -parent_->rect().width()/2.0, -parent_->rect().width()/2.0*tan(bwish) );
        orientation_ = OrientLeft;
//         cout<<"DEBUG: "<<def_.type<<" left ("<<pos().x()<<":"<<pos().y()<<":"<<bwish<<")"<<endl;
    }
    // top quadrant (coord. system is upside-down!)
    else if ( bwish>=pdiag && bwish<(M_PI-pdiag) ) {
//         double x1 = parent_->rect().height()/2.0*tan(M_PI/2.0-bwish);
//         cout<<"DEBUG: "<<def_.type<<" up ("<<pos().x()<<","<<pos().y()<<")->"<<x1<<")"<<endl;
        setPos( parent_->rect().height()/2.0*tan(M_PI/2.0-bwish), parent_->rect().height()/2.0 );
        orientation_ = OrientUp;
    }
    // bottom quadrant (coord. system is upside-down!)
    else if ( bwish>=(-M_PI+pdiag) && bwish<-pdiag ) {
        setPos( -parent_->rect().height()/2.0*tan(M_PI/2.0-bwish), -parent_->rect().height()/2.0 );
        orientation_ = OrientDown;
//         cout<<"DEBUG: "<<def_.type<<" down ("<<pos().x()<<":"<<pos().y()<<":"<<bwish<<")"<<endl;
    }

    // move links
//     moveLinksBy( dxp, dyp );
    // move text
    placeLabelAuto();
}

/*!
    Call this after port moved itself
*/
void 
Interface::moveLinksBy( double dx, double dy ) {
    // find center
    double xc = bubblePos().x();
    double yc = bubblePos().y();

    for ( int i=0; i<inLinks_.size(); ++i ) {
        inLinks_[i]->setToPoint( xc, yc );
        inLinks_[i]->placeLabelAuto();
    }
    for ( int i=0; i<outLinks_.size(); ++i ) {
        outLinks_[i]->setToPoint( xc, yc );
        outLinks_[i]->placeLabelAuto();
    }
}

/*!
    Places the text label on a radius from parent's center
*/
void 
Interface::placeLabelAuto() 
{
    double labelDist = 25.0;
    double bp = bearingToParent();
    // place it near where we want it, then we'll twick it a bit.
    label_->setPos( bubblePos().x()+labelDist*cos(bp), bubblePos().y()+labelDist*sin(bp) - 5 );

    switch ( orientation_ )
    {
    case OrientUp :
    case OrientDown :
        label_->moveBy( -label_->boundingRect().width()/2.0, -5.0 );
        break;
    case OrientLeft :
        label_->moveBy( -label_->boundingRect().width(), -5.0 );
        break;
    case OrientRight :
        label_->moveBy( 0.0, -5.0 );
        break;
    } //switch

//     std::cout<<"placeLabelAuto:"<<endl;
//     std::cout<<"r="<<rp<<" b="<<bp*180.0/3.14<<std::endl;
//     std::cout<<"("<<centerX()<<","<<centerY()<<") ("<<xToParent()<<","<<yToParent()<<
//        ") ("<<rp*cos(bp)<<","<<rp*sin(bp)<<")"<<std::endl;
}

void 
Interface::drawDebug( QPainter *p )
{
    p->save();
    p->setBrush( Qt::NoBrush );
    p->setPen( Qt::blue );

    p->drawLine( QPointF(-_stemSize/2.0,0.0), QPointF(_stemSize/2.0,0.0) );
    p->drawLine( QPointF(0.0,-_stemSize/2.0), QPointF(0.0,_stemSize/2.0) );

    p->setPen( Qt::red );
    p->drawRect( boundingRect() );
    p->restore();
}

void 
Interface::drawStem( QPainter* p )
{
    switch ( orientation_ )
    {
    case OrientUp :
        p->drawLine( QPointF(), QPointF(0.0,_stemSize) );
        break;
    case OrientDown :
        p->drawLine( QPointF(), QPointF(0.0,-_stemSize) );
        break;
    case OrientLeft :
        p->drawLine( QPointF(), QPointF(-_stemSize,0.0) );
        break;
    case OrientRight :
        p->drawLine( QPointF(), QPointF(_stemSize,0.0) );
        break;
    }    
}

void 
Interface::reallyHide() {
    hide();
    label_->hide();
}

void 
Interface::reallyShow() {
    show();
    label_->show();
}

// const std::string 
// Interface::description() const
// {
//     std::stringstream ss;
// 
//     ss<<"Interface of Component '"<<parent_->def().tag<<"', interfaceTag '"<<def_.tag<<"'";
// 
//     return ss.str();
// }
