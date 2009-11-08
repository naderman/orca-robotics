/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_RANGESCANNER2D_ELEMENT_H
#define ORCAGUI3D_RANGESCANNER2D_ELEMENT_H

#include <orcaqgui3d/icestormelement3d.h>
#include <orcaqgui3dfactory/laserscanner2dpainter.h>

namespace orcaqgui3d {

//!
//! @brief a 3d element
//!
//! @author Alex Brooks
//!
class RangeScanner2dElement
    : public IceStormElement3d<LaserScanner2dPainter,
                               orca::RangeScanner2dDataPtr,
                               orca::RangeScanner2dPrx,
                               orca::RangeScanner2dConsumer,
                               orca::RangeScanner2dConsumerPrx>
{           
public:
    RangeScanner2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                           const orcaice::Context                     &context,
                           int                                         timeoutMs    = 5000,
                           QColor                                      outlineColor = QColor( 102,102,153, 255 ) )
        : IceStormElement3d<LaserScanner2dPainter,
                               orca::RangeScanner2dDataPtr,
                               orca::RangeScanner2dPrx,
                               orca::RangeScanner2dConsumer,
                               orca::RangeScanner2dConsumerPrx>( guiElementInfo, context, painter_, timeoutMs ),
          painter_( outlineColor )
        {};

    virtual bool isInGlobalCS() { return false; }
    virtual void iceStormConnectedEvent() { getRangeInfo(); }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };

    virtual osg::Node *osgNode() const { return painter_.osgNode(); }

private:
    void getRangeInfo();
    LaserScanner2dPainter painter_;
    
};

}

#endif
