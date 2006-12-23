/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_POSITION3D_ELEMENT_H
#define ORCAGUI3D_POSITION3D_ELEMENT_H

#include <orcaqgui3d/icestormelement3d.h>
#include <orcaqgui3dfactory/position3dpainter.h>
#include <orcaqgui3d/iknowsplatformposition.h>

namespace orcaqgui3d {

//!
//! @author Alex Brooks
//!
class Position3dElement
    : public IceStormElement3d<Position3dPainter,
                               orca::Position3dData,
                               orca::Position3dPrx,
                               orca::Position3dConsumer,
                               orca::Position3dConsumerPrx>,
    public IKnowsPlatformPosition
{

public: 

    Position3dElement( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       int                      timeoutMs=5000 )
        : IceStormElement3d<Position3dPainter,
                            orca::Position3dData,
                            orca::Position3dPrx,
                            orca::Position3dConsumer,
                            orca::Position3dConsumerPrx>(context, proxyString, painter_, timeoutMs ),
          painter_("3dmodels/jeep.3ds")
        {}

    virtual bool isInGlobalCS() { return false; }
    // virtual QStringList contextMenu();
    // virtual void execute( int action ) { painter_.execute( action ); };
    // virtual GuiElementType type() { return orcaqgui::LaserType; };
    // virtual void setColor( QColor color ) { painter_.setColor(color); }
    // virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };
    virtual void actionOnConnection() {}

    // Access to platform location estimate.
    virtual float x() const { return painter_.pose().p.x; }
    virtual float y() const { return painter_.pose().p.y; }
    virtual float z() const { return painter_.pose().p.z; }
    virtual float roll()  const { return painter_.pose().o.r; }
    virtual float pitch() const { return painter_.pose().o.p; }
    virtual float yaw()   const { return painter_.pose().o.y; }

    virtual int platformKnowledgeReliability() const { return 2; }
    // virtual QPointF pos() const { return QPointF(x_,y_); };

private: 

    Position3dPainter painter_;

};

}

#endif
