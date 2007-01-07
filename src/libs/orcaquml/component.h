/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQUML_COMPONENT_H
#define ORCAQUML_COMPONENT_H

#include <QGraphicsItem>
#include <QFont>
#include <QColor>

#include <orcadef/componentdefinition.h>
#include <orcaquml/interface.h>

namespace orcaquml
{

class ComponentModel;
class ComponentGeom;

/*!
    @brief UML Component element.
*/
class Component: public QGraphicsRectItem
{
public:
    //! Create component given its full description and the scene to which it belongs.
    Component( ComponentModel & model, QGraphicsScene* scene );

    virtual int rtti () const { return 5555; };

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    const orcadef::ComponentDef& def() const { return def_; };
    const orcadef::ComponentCfg& cfg() const { return cfg_; };
    const ComponentGeom &geom() const { return geom_; }; // Geometry( id_, (int)x(), (int)y() ); };

    QString tag() const { return QString::fromStdString(def_.tag); };
    QString name() const { return QString::fromStdString(cfg_.fqname.component); };
    QString platformName() const { return QString::fromStdString(cfg_.fqname.platform); };
    QString hostName() const { return QString::fromStdString(cfg_.fqexecutable.host); };
    QString exeFileName() const { return QString::fromStdString(cfg_.fqexecutable.executable); };
    
    void setPlatformName( const QString &s );
    void setHostName( const QString &s );
    void toggleHighlighted();

    QString configFileName() const { return QString( exeFileName() + QString::number( id_ ) + ".xml" ); };

    bool hasInterface( const QString &name, const QString &pat, const QString &role );
    void prepareToDie();

    void setSelected( bool yes );
    void toggleSelected();
    void setColor();

    // Hide itself and the children
    void reallyHide();
    // Show itself and the children
    void reallyShow();

    static void resetCounter() { _count = 0; };

    static void toggleColorScheme();
    static void toggleNaming();
    static void toggleDeployment();

    void placeInterfaceAuto();

    // Need to be public so we can link them.
    QList<Interface*> provided_;
    QList<Interface*> required_;

//     virtual const std::string description() const { return "Component (tag " + def_.tag + ")"; }

private:

    void initInterfaces();

    void drawDebug( QPainter *p );
    void drawUmlSymbol( QPainter *p );
    void drawLabel( QPainter *p );
    
    int id_;

    ComponentModel &model_;

    // Convenient access to fields of model_.
    orcadef::ComponentDef &def_;
    orcadef::ComponentCfg &cfg_;
    ComponentGeom         &geom_;

    bool isHighlighted_;

    // a system may have multiple instances of the same component running on the same
    // platform. Standard filenames (e.g. mycomponent.xml) will overwrite each other in
    // this case. Need to add something unique (e.g. mycomponent0.xml, mycomponent1.xml)
    // It's easier to make a number unique to all components, e.g. by having a counter.
    static int _count;

    // 0 = all default colors, 1 = color-coded by hostname, 2 = color-coded by platform name
    static int _colorScheme;
    
    static bool _showNamingInfo;
    static bool _showDeploymentInfo;
    
    static double _defaultWidth;
    static double _defaultHeight;
    static QPen _defaultPen;
    static QPen _selectedPen;
    static QBrush _defaultBrush;
    static QPen _highlightedPen;
    static QBrush _highlightedBrush;
    
    static QFont _normalFont;
    static QFont _typeFont;

};

} // namespace

#endif
