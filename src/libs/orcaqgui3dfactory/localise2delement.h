/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_LOCALISE2D_GUI_ELEMENT_H
#define ORCAGUI_LOCALISE2D_GUI_ELEMENT_H

#include <orcaice/context.h>

#include <orcaqgui3d/iknowsplatformposition.h>
#include <orcaqgui3d/icestormelement3d.h>
#include <orcaqgui3dfactory/localisepainter.h>

namespace orcaqgui3d
{

class Localise2dElement
    : public orcaqgui3d::IceStormElement3d<LocalisePainter,
                             orca::Localise2dData,
                             orca::Localise2dPrx,
                             orca::Localise2dConsumer,
                             orca::Localise2dConsumerPrx>,
      public orcaqgui3d::IKnowsPlatformPosition
{
public:
    Localise2dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                       const orcaice::Context                     &context,
                       bool                                        beginDisplayHistory = false,
                       int                                         timeoutMs = 30000 )
        : orcaqgui3d::IceStormElement3d<LocalisePainter,
                            orca::Localise2dData,
                            orca::Localise2dPrx,
                            orca::Localise2dConsumer,
                            orca::Localise2dConsumerPrx>( guiElementInfo, context, painter_, timeoutMs ),
          painter_( beginDisplayHistory ),
          x_(0),
          y_(0),
          yaw_(0),
          haveGeometry_(false)
        {}

    virtual bool isInGlobalCS() { return true; }
    virtual void iceStormConnectedEvent();
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };
    virtual void setUseTransparency( bool useTransparency ) { painter_.setUseTransparency( useTransparency ); };

    // Need a special update function to set (x,y,z,roll,pitch,yaw)
    // The Localise2dElement needs this because it's special kind of GuiElement:
    //   A platform has a position in the world and various other things are
    //   drawn with respect to that position.
    virtual void update();

    // Access to ML estimate.
    virtual float x() const { return x_; }
    virtual float y() const { return y_; }
    // HACK: z hard-coded to 0.35 !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    virtual float z() const { return 0.35; }
    virtual float roll() const { return 0.0; }
    virtual float pitch() const { return 0.0; }
    virtual float yaw() const { return yaw_; }
    virtual int platformKnowledgeReliability() const { return 7; }

    osg::Node *osgNode() const { return painter_.osgNode(); }    

private:

    LocalisePainter painter_;

    // We're visualising a 2D thing -- don't know z/roll/pitch
    double x_;
    double y_;
    double yaw_;
    
    void tryToGetGeometry();
    bool haveGeometry_;
};

}

#endif
