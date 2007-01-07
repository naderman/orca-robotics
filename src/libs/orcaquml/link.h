/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQUML_LINK_H
#define ORCAQUML_LINK_H

#include <QGraphicsItem>
// #include <orcaquml/debuggable.h>

namespace orcaquml
{

// a simple forward declaration will do here.
// Interface on the other hand must include this header file to keep a list of links.
class Interface;

class Link: public QGraphicsLineItem //, public QorcaDebuggable
{
public:
    Link( Interface* from, Interface* to );

    virtual int rtti () const { return 7777; };
    
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    void prepareToDie();

    void setFromInterface( Interface* p ) { from_ = p; };
    void setToInterface( Interface* p ) { to_ = p; };

    void setFromPoint( double x, double y ) ;
    void setToPoint( double x, double y );

    void setSelected( bool yes );
    void setLabel();

//     void getFromInterfaceConfig( QDomElement &conf );
//     void getToInterfaceConfig( QDomElement &conf );

    void getFromInterfaceNaming( QString & platform, QString & name );
    void getToInterfaceNaming( QString & platform, QString & name );

    void moveBy(double dx, double dy) {};
    void placeLabelAuto();

    static void toggleSignature() { _showInterfaceName = !_showInterfaceName; };

//     virtual const std::string description() const { return "link"; }

private:
    
    Interface* from_;
    Interface* to_;

    QGraphicsSimpleTextItem *label_;

    static bool _showInterfaceName;

    static QPen _defaultPen;
    static QPen _selectedPen;

};

}

#endif
