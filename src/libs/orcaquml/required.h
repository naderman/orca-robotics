/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQUML_REQUIRED_H
#define ORCAQUML_REQUIRED_H

#include <orcadef/componentdefinition.h>
#include <orcaquml/model.h>
#include <orcaquml/interface.h>

namespace orcaquml
{

class Required: public Interface
{
public:
    Required( Component* parent,
            QGraphicsScene* scene, 
            orcadef::RequiredDef &def,
            orcadef::RequiredCfg &cfg,
            InterfaceGeom        &geom );

    QString interface() { return QString::fromStdString( def_.type ); };
    QString tag() { return QString::fromStdString( cfg_.tag ); };
    QString name() { return QString::fromStdString( cfg_.proxy ); };
    
    bool isClient() const { return true; };
    void checkValid();
    bool isHidable() const { return !isNecessary_ && !isLinked(); };

//     void writeConfig( QDomElement & );
//     void getConfig( QDomElement & );
    void setLabel();
    
    void linkEvent( Interface &toInterface );

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    virtual int rtti () const { return RTTI_REQUIRED_INTERFACE; };

    // Returns a human-readable description (useful for debugging)
//     virtual const std::string description() const;

private:
    
    orcadef::RequiredDef &def_;
    orcadef::RequiredCfg &cfg_;
    
    void drawBubble( QPainter* p );
    void setValid( bool yes );
    
    bool isNecessary_;

    static QPen _validPen;
    static QBrush _validBrush;
    static QPen _invalidPen;
    static QBrush _invalidBrush;
};

} // namespace

#endif
