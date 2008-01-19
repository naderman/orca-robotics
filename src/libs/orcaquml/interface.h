/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAQUML_INTERFACE_H
#define ORCAQUML_INTERFACE_H

#include <QAbstractGraphicsShapeItem>
#include <orcaquml/model.h>
// #include <orcaquml/debuggable.h>

// Can't just do forward declaration because we need to keep a list of pointers.
// Qt will complain and gcc-3.4 produce an error.
#include <orcaquml/link.h>

namespace orcaquml
{

class Component;

class Interface: public QAbstractGraphicsShapeItem //, public QorcaDebuggable
{
public:
    Interface( Component* parent, 
                QGraphicsScene* scene,
                orcadef::InterfaceDef &def,
                orcadef::InterfaceCfg &cfg,
                InterfaceGeom &geom );

    virtual QRectF boundingRect() const;
    // paint() is implemented in derived classes
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    
    void setBearingConstrained( double theta );
    void snapToParent();
    void prepareToDie();
    
    enum Orientation{ OrientUp, OrientDown, OrientLeft, OrientRight };
    Orientation orientation() const { return orientation_; };

    bool isLinked() const {
        return !( inLinks_.isEmpty() && outLinks_.isEmpty()) ;
    };
    int linkCount() const {
        return inLinks_.count() + outLinks_.count();
    };

    // Hide itself and the label
    void reallyHide();
    // Show itself and the label
    void reallyShow();

    // A unique ID to identify a port. Used to reconnect component links after
    // save/load operation.
    int id() const {
        return id_;
    };

    virtual bool isClient() const = 0;
    virtual void checkValid() = 0;
//     virtual void getConfig( QDomElement & ) = 0;
//     virtual void writeConfig( QDomElement & ) = 0;
    virtual void setLabel() = 0;

    // Returns a human-readable description (useful for debugging)
//     virtual const std::string description() const;

    void getNaming( QString & platform, QString & name ) { platform=nsPlatform_; name=nsName_; };

    // provided and requried will respond to this event differently
    virtual void linkEvent( Interface &toInterface ) {};
    
    void addInEdge( Link* link );
    void addOutEdge( Link* link );

    void removeInEdge( Link* link );
    void removeOutEdge( Link* link );

    static void toggleTag() {
        _showTag = !_showTag;
    };
    static void toggleSignature() {
        _showInterfaceName = !_showInterfaceName;
    };
    static void toggleServiceName() {
        _showServiceName = !_showServiceName;
    };

    QPointF bubblePos() const;

    Component &parent() const { return *parent_; }
    const orcadef::InterfaceCfg &cfg() const { return cfg_; }

    // obsolete
//     QString signature() const {
//         return ( QString("dummy") + "<" + object_ + ">");
//     };

protected:

    virtual void setValid( bool yes ) = 0;

    void drawDebug( QPainter* p );
    void drawStem( QPainter* p );

    // geometry functions
    double rangeToParent() const;
    double bearingToParent() const;
    
    Orientation orientation_;
    
    void moveLinksBy( double dx, double dy );
    void placeLabelAuto();

    // a unique ID to reconnect links after save/load operation
    int id_;
    QString tag_;
    QString interface;
    // this is my platform/name, only makes sense for servers, but put it here so we can set label_ appropriately
    QString nsPlatform_;
    QString nsName_;
    Component* parent_;

    // Hook into the model
    orcadef::InterfaceDef  &def_;
    orcadef::InterfaceCfg  &cfg_;
    InterfaceGeom &geom_;

    QList<Link*> inLinks_;
    QList<Link*> outLinks_;

    QGraphicsSimpleTextItem *label_;

    static double _stemSize;
    static double _bubbleRadius;
    static bool _showTag;
    static bool _showInterfaceName;
    static bool _showServiceName;
};

}

#endif
