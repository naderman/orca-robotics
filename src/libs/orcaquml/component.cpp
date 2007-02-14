/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include  <math.h>

#include <QtGui>

#include "component.h"
#include "model.h"
#include "provided.h"
#include "required.h"


using namespace std;
using namespace orcaquml;

int Component::_count = 0;
int Component::_colorScheme = 0;

bool Component::_showNamingInfo=true;
bool Component::_showDeploymentInfo=false;
    

double Component::_defaultWidth = 200;
double Component::_defaultHeight = 100;
QPen Component::_defaultPen( Qt::black, 1 );
QPen Component::_selectedPen( Qt::black, 2 );
QBrush Component::_defaultBrush( QColor( 252, 242, 227 ) );

QPen   Component::_highlightedPen( Qt::black, 3, Qt::DashLine );
QBrush Component::_highlightedBrush( QColor( 255, 255, 0 ) );

QFont Component::_normalFont;
QFont Component::_typeFont( QString("Sans Serif"), 11, QFont::Bold );

void Component::toggleColorScheme()
{
    _colorScheme++;
    if ( _colorScheme > 2 ) {
        _colorScheme = 0;
    }
}
void Component::toggleNaming()
{
    _showNamingInfo = !_showNamingInfo;
};
void Component::toggleDeployment()
{
    _showDeploymentInfo = !_showDeploymentInfo;
};


Component::Component( ComponentModel &model, QGraphicsScene* scene )
        : QGraphicsRectItem(0,scene),
          model_(model),
          def_(model.def),
          cfg_(model.cfg),
          geom_(model.geom)
{
    setFlags(ItemIsSelectable | ItemIsMovable);
    setAcceptsHoverEvents(true);

    setToolTip( QString("type: ")+tag() );
    setCursor( Qt::OpenHandCursor );

    id_ = _count++;
        
    setRect( -_defaultWidth/2.0, -_defaultHeight/2.0, _defaultWidth, _defaultHeight );
    setPen( _defaultPen );
    setBrush( _defaultBrush );
    setZValue( 100.0 );

    isHighlighted_ = false;

    moveBy( geom_.left, geom_.top );

    initInterfaces();
    placeInterfaceAuto();
    
    id_ = geom_.id;

    setHostName( QString(cfg_.fqexecutable.host.c_str()) );
    setPlatformName( QString(cfg_.fqname.platform.c_str()) );
}

void 
Component::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsRectItem::paint( painter, option, widget );

//     drawDebug( painter );
    drawUmlSymbol( painter );
    // draw component type, name, and deployment info
    drawLabel( painter );
}

void 
Component::initInterfaces()
{
    Interface* q;

    // provided interfaces
    for ( unsigned int i=0; i<def_.provided.size(); ++i ) 
    {
        q = new Provided(this, scene(),
                        def_.provided[i], 
                        cfg_.provided[i], 
                        geom_.provided[i] );
        // add this iface to out list
        provided_.append( q );
    }
    
    // required interfaces
    //cout<<"comp "<<def_.tag<<" has "<<def_.required.size()<<" required interfaces"<<endl;
    for ( unsigned int i=0; i<def_.required.size(); ++i ) 
    {
        q = new Required(this, scene(),
                        def_.required[i], 
                        cfg_.required[i], 
                        geom_.required[i] );
        // add this iface to out list
        required_.append( q );
    }
}

void 
Component::prepareToDie()
{
//     Interface* iface;
// 
//     // delete provided interfaces
//     Q3PtrListIterator<Interface> its( provided_ );
//     while ( (iface = its.current()) ) 
//     {
//         ++its;
//         iface->prepareToDie();
//         delete iface;
//     }
//     // delete required interfaces
//     Q3PtrListIterator<Interface> itc( required_ );
//     while ( (iface = itc.current()) ) 
//     {
//         ++itc;
//         iface->prepareToDie();
//         delete iface;
//     }
}

bool 
Component::hasInterface( const QString &name,
                                   const QString &interface,
                                   const QString &role ) 
{
//     // special case: "select all"
//     if ( (name=="" || name=="*" ) && interface=="*" && role=="*" )
//     {
//         return true;
//     }
//     
//     // check the name first, only if there's a match check interfaces
//     QString myname = QString::fromStdString( cfg_.fqname.component );
//     if ( name!="" && name!="*" && !myname.startsWith( name, false ) )
//     {
//         return false;
//     }
// 
//     Interface* q;
//     if ( role=="Provided" || role=="*" ) 
//     {
//         // provided interfaces
//         Q3PtrListIterator<Interface> it( provided_ );
//         while ( (q = it.current()) ) {
//             ++it;
//             if ( q->hasInterface( interface ) ) {
//                 return true;
//             }
//         }
//     }
//     if ( role=="Required" || role=="*" ) 
//     {
//         // required interfaces
//         Q3PtrListIterator<Interface> it( required_ );
//         while ( (q = it.current()) ) {
//             ++it;
//             if ( q->hasInterface( interface ) ) {
//                 return true;
//             }
//         }
//     }
    return false;
}

void 
Component::setHostName( const QString &s )
{
    cfg_.fqexecutable.host = s.toStdString();
/*
    QorcaCanvas *c = (QorcaCanvas*)canvas();
    c->addHostName( s );
    setColor();

    update();*/
};

void 
Component::setPlatformName( const QString &s )
{
    cfg_.fqname.platform = s.toStdString();
/*
    QorcaCanvas *c = (QorcaCanvas*)canvas();
    c->addPlatformName( s );
    setColor();

    update();
    cout<<"TRACE(qorcacomponent.cpp): TODO: Have to update others' required interfaces!" << endl;*/
};

void 
Component::drawDebug( QPainter *p )
{
    p->save();
    p->setBrush( Qt::NoBrush );
    p->setPen( Qt::blue );

    p->drawLine( QPointF(-rect().width()/4.0,0.0), QPointF(rect().width()/4.0,0.0) );
    p->drawLine( QPointF(0.0,-rect().height()/4.0), QPointF(0.0,rect().height()/4.0) );

    p->setPen( Qt::red );
    p->drawEllipse( QRectF(rect().width()*0.5-rect().height()*0.1,rect().height()*0.4, rect().height()*0.2,rect().height()*0.2) );

    p->setPen( Qt::green );
    p->drawRect( childrenBoundingRect() );
    p->restore();
}

void 
Component::drawUmlSymbol( QPainter *p )
{
    p->save();
    p->setPen( _defaultPen );
    
    // our location in the widget
    QRectF r = rect();

    // draw UML component symbol
    QRectF biguml( r.right()-32, r.top()+10, 25, 20 );
    p->drawRect( biguml );
    p->drawRect( QRectF(r.right()-37, r.top()+13, 10, 5) );
    p->drawRect( QRectF(r.right()-37, r.top()+20, 10, 5) );

    p->restore();
}

void 
Component::drawLabel( QPainter *p )
{
//    cout<<"drawing label for "<<name().toStdString()<<endl;
    
    QString s;
    bool someText = false;
    
    // always show component type
    p->setFont( _typeFont );
    QRectF r = rect();
    r.setHeight( (int)round( (double)r.height()*0.75 ) );
    p->drawText( r, Qt::AlignCenter, tag() );
    p->setFont( _normalFont );
  
    if ( _showNamingInfo )  {
        s += "\n";
        s += platformName() + "/" + name();
        someText = true;
    }
    if ( _showDeploymentInfo ) {
        if ( someText )  {
            s += "\n";  
        }
        s += "["+ exeFileName() + "/" + hostName() +"]";
    }
    
    r = rect();
    //cout<<r.x()<<","<<r.y()<<" "<<r.width()<<" "<<r.height()<<endl;
    int half = (int)round( (double)r.height()*0.5 );
    int aBit = (int)round( (double)r.height()*0.1 );
    r.setY( r.y()+half-aBit  );
    r.setHeight( half+aBit );
    //cout<<r.x()<<","<<r.y()<<" "<<r.width()<<" "<<r.height()<<endl;
    p->drawText( r, Qt::AlignCenter, s );
}

void 
Component::setSelected( bool yes )
{
//     Q3CanvasItem::setSelected( yes );

    if ( isSelected() ) {
        setPen( _selectedPen );
    }
    else if ( isHighlighted_ ) {
        setPen( _highlightedPen );
    }
    else {
        setPen( _defaultPen );
    }
//     update();
};

void 
Component::toggleSelected()
{
    setSelected( !isSelected() );
};

void 
Component::toggleHighlighted()
{
    isHighlighted_ = !isHighlighted_;
    if ( isHighlighted_ ) {
        cout<<"highlight is ON"<<endl;
        setPen( _highlightedPen );
        setBrush( _highlightedBrush );
    } else {
        cout<<"highlight is OFF"<<endl;
        setPen( _defaultPen );
        setBrush( _defaultBrush );
    }
    geom_.isHighlighted = isHighlighted_;
//     update();
};

void 
Component::setColor()
{
//     QorcaCanvas *c = (QorcaCanvas*)canvas();
//     switch ( _colorScheme ) {
//     case 0:
//         setBrush( _defaultBrush );
//         break;
//     case 1:
//         setBrush( c->hostColor( hostName() ) );
//         break;
//     case 2:
//         setBrush( c->platformColor( platformName() ) );
//         break;
//     }
}

void 
Component::placeInterfaceAuto()
{
    // spread all interfaces evenly
    int n = provided_.count() + required_.count();
//     cout<<"auto-placing "<<n<<" interfaces"<<endl;

    int count = 0;
    // ccw rotation is used to put provided ports (servers) on top
    for ( int i=0; i<provided_.size(); ++i ) {
        provided_[i]->setBearingConstrained( count*2.0*M_PI/n );
        ++count;
    }
    for ( int i=0; i<required_.size(); ++i ) {
        required_[i]->setBearingConstrained( count*2.0*M_PI/n );
        ++count;
    }
}

// QRect 
// Component::boundingRect() const
// {
//     return Q3CanvasRectangle::boundingRect();
//     
//     // enflate rect size to make sure thick borders get redrawn
//     //QRect r = rect();
//     //return QRect( r.x()-2, r.y()-2, r.width()+4, r.height()+4 );
// }

void 
Component::reallyHide()
{
//     hide();
//     // clients
//     Required* qc;
//     Q3PtrListIterator<Interface> itc( required_ );
//     while ( (qc = (Required*)itc.current()) ) {
//         ++itc;
//         qc->reallyHide();
//     }
// 
//     // servers
//     Provided* qs;
//     Q3PtrListIterator<Interface> its( provided_ );
//     while ( (qs = (Provided*)its.current()) ) {
//         ++its;
//         qs->reallyHide();
//     }
}

void 
Component::reallyShow()
{
//     show();
//     // clients
//     Required* qc;
//     Q3PtrListIterator<Interface> itc( required_ );
//     while ( (qc = (Required*)itc.current()) ) {
//         ++itc;
//         qc->reallyShow();
//     }
// 
//     // servers
//     Provided* qs;
//     Q3PtrListIterator<Interface> its( provided_ );
//     while ( (qs = (Provided*)its.current()) ) {
//         ++its;
//         qs->reallyShow();
//     }
}
