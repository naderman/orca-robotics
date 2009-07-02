/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_LOCALISE3D_GUI_ELEMENT_H
#define ORCAGUI_LOCALISE3D_GUI_ELEMENT_H

#include <orcaice/context.h>

#include <orcaqgui3d/iknowsplatformposition.h>
#include <orcaqgui3d/icestormelement3d.h>
#include <orcaqgui3dfactory/localisepainter.h>
#include <hydroqguielementutil/ihumanmanager.h>

namespace orcaqgui3d
{

class Localise3dElement
    : public orcaqgui3d::IceStormElement3d<LocalisePainter,
                             orca::Localise3dData,
                             orca::Localise3dPrx,
                             orca::Localise3dConsumer,
                             orca::Localise3dConsumerPrx>,
      public orcaqgui3d::IKnowsPlatformPosition
{
public:
    Localise3dElement( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                       const orcaice::Context                     &context,
                       const std::string                          &proxyString,
                       hydroqguielementutil::IHumanManager        *humanManager,
                       bool                                        beginDisplayHistory = false,
                       int                                         timeoutMs = 30000 )
        : orcaqgui3d::IceStormElement3d<LocalisePainter,
                            orca::Localise3dData,
                            orca::Localise3dPrx,
                            orca::Localise3dConsumer,
                            orca::Localise3dConsumerPrx>( guiElementInfo, context, proxyString, painter_, timeoutMs ),
          humanManager_(humanManager),
          painter_( beginDisplayHistory ),
          x_(0),
          y_(0),
          yaw_(0),
          haveGeometry_(false)
        {}

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection();
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };
    virtual void setUseTransparency( bool useTransparency ) { painter_.setUseTransparency( useTransparency ); };

    // Need a special update function to set (x,y,z,roll,pitch,yaw)
    // The Localise3dElement needs this because it's special kind of GuiElement:
    //   A platform has a position in the world and various other things are
    //   drawn with respect to that position.
    virtual void update();

    // Access to ML estimate.
    virtual float x() const { return x_; }
    virtual float y() const { return y_; }
    virtual float z() const { return z_; }
    virtual float roll() const { return roll_; }
    virtual float pitch() const { return pitch_; }
    virtual float yaw() const { return yaw_; }
    virtual int platformKnowledgeReliability() const { return 8; }

    osg::Node *osgNode() const { return painter_.osgNode(); }    

private:

    hydroqguielementutil::IHumanManager *humanManager_;
    LocalisePainter painter_;

    double x_;
    double y_;
    double z_;
    double roll_;
    double pitch_;
    double yaw_;
    
    void tryToGetGeometry();
    bool haveGeometry_;
};

}

#endif
