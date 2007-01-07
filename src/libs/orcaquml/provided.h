/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQUML_PROVIDED_H
#define ORCAQUML_PROVIDED_H

#include <orcadef/componentdefinition.h>
#include <orcaquml/model.h>
#include <orcaquml/interface.h>

namespace orcaquml
{

class Provided: public Interface
{
public:
    Provided( Component* parent,
            QGraphicsScene* scene,
            orcadef::ProvidedDef &def,
            orcadef::ProvidedCfg &cfg,
            InterfaceGeom        &geom );

    QString interface() { return QString::fromStdString( def_.type ); };
    QString tag() { return QString::fromStdString( cfg_.tag ); };
    QString name() { return QString::fromStdString( cfg_.name ); };

    bool isClient() const { return false; };
    int maxConnections() const { return maxConnections_; };
    void checkValid() { setValid( true ); };

//     void writeConfig( QDomElement & );
//     void getConfig( QDomElement & );
    void setLabel();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget);

    virtual int rtti () const { return RTTI_PROVIDED_INTERFACE; };

    // Returns a human-readable description (useful for debugging)
//     virtual const std::string description() const;

    const orcadef::ProvidedCfg &cfg() const { return cfg_; }

private:
    
    orcadef::ProvidedDef &def_;
    orcadef::ProvidedCfg &cfg_;
    
    void drawBubble( QPainter* p );
    void setValid( bool yes ) {};
    int maxConnections_;

    static QPen _Pen;
    static QBrush _Brush;
};

} // namespace

#endif
