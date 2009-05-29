/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_LASERSCANNER2D_ELEMENT_H
#define ORCAGUI3D_LASERSCANNER2D_ELEMENT_H

#include <orcaqgui3d/icestormelement3d.h>
#include <orcaqgui3dfactory/laserscanner2dpainter.h>

namespace orcaqgui3d {

//!
//! @brief a 3d laser element
//!
//! @author Alex Brooks
//!
class LaserScanner2dElement
    : public IceStormElement3d<LaserScanner2dPainter,
                               orca::RangeScanner2dDataPtr,
                               orca::LaserScanner2dPrx,
                               orca::RangeScanner2dConsumer,
                               orca::RangeScanner2dConsumerPrx>
{           
public:
    LaserScanner2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                           const orcaice::Context                     &context,
                           const std::string                          &proxyString,
                           int                                         timeoutMs    = 5000,
                           QColor                                      outlineColor = QColor( 102,102,153, 255 ) )
        : IceStormElement3d<LaserScanner2dPainter,
                               orca::RangeScanner2dDataPtr,
                               orca::LaserScanner2dPrx,
                               orca::RangeScanner2dConsumer,
                               orca::RangeScanner2dConsumerPrx>( guiElementInfo, context, proxyString, painter_, timeoutMs ),
          painter_( outlineColor )
        {};

    virtual bool isInGlobalCS() { return false; }
    virtual void actionOnConnection() { getLaserInfo(); }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };

    virtual osg::Node *osgNode() const { return painter_.osgNode(); }

private:
    void getLaserInfo();
    LaserScanner2dPainter painter_;
    
};

}

#endif
