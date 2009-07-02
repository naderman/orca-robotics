/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI3D_POINT_CLOUD_ELEMENT_H
#define ORCAGUI3D_POINT_CLOUD_ELEMENT_H

#include <orcaqgui3d/icestormelement3d.h>
#include <orcaqgui3dfactory/pointcloudpainter.h>

namespace orcaqgui3d {

//!
//! @brief a 3d element
//!
//! @author Alex Brooks
//!
class PointCloudElement
    : public IceStormElement3d<PointCloudPainter,
                                  orca::PointCloudData,
                                  orca::PointCloudPrx,
                                  orca::PointCloudConsumer,
                                  orca::PointCloudConsumerPrx>
{           
public:
    PointCloudElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                       const orcaice::Context                     &context,
                       const std::string                          &proxyString,
                       int                                         timeoutMs    = 5000 )
        : IceStormElement3d<PointCloudPainter,
                               orca::PointCloudData,
                               orca::PointCloudPrx,
                               orca::PointCloudConsumer,
                               orca::PointCloudConsumerPrx>( guiElementInfo, context, proxyString, painter_, timeoutMs )
        {};
    virtual ~PointCloudElement(){};

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection() { }
    virtual QStringList contextMenu();
    virtual void execute( int action );
//     virtual void setColor( QColor color ) { painter_.setColor(color); }
//     virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };

    virtual osg::Node *osgNode() const { return painter_.osgNode(); }

private:
    PointCloudPainter painter_;
    
};

}

#endif
