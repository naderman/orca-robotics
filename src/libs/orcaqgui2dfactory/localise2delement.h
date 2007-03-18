/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_LOCALISE2D_GUI_ELEMENT_H
#define ORCAGUI_LOCALISE2D_GUI_ELEMENT_H

#include <QPointF>

#include <orcaobj/orcaobj.h>
#include <orcaice/orcaice.h>

#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2d/icestormelement.h>
#include <orcaqgui2d/iknowsplatformposition2d.h>
#include <orcaqgui2dfactory/connectutils.h>
#include <orcaqgui2dfactory/localise2dpainter.h>

namespace orcaqgui2d
{

class Localise2dElement
    : public IceStormElement<Localise2dPainter,
                             orca::Localise2dData,
                             orca::Localise2dPrx,
                             orca::Localise2dConsumer,
                             orca::Localise2dConsumerPrx>,
      public IKnowsPlatformPosition2d
{
public:
    Localise2dElement( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       orcaqgui::IHumanManager *humanManager,
                       bool                     beginDisplayHistory=false,
                       int                      timeoutMs=3000 )
        : IceStormElement<Localise2dPainter,
                            orca::Localise2dData,
                            orca::Localise2dPrx,
                            orca::Localise2dConsumer,
                            orca::Localise2dConsumerPrx>(context, proxyString, painter_, timeoutMs ),
          humanManager_(humanManager),
          painter_( beginDisplayHistory ),
          gotDescription_(false)
        {
        };

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection();
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };
    virtual void setTransparency( bool useTransparency ) { painter_.setTransparency( useTransparency ); };

    // Need a special update function to set (x,y,theta)
    // The Localise2dElement needs this because it's special kind of GuiElement:
    //   A platform has a position in the world and various other things are
    //   drawn with respect to that position.
    virtual void update();

    // Access to ML estimate.
    virtual double x() const { return x_; }
    virtual double y() const { return y_; }
    virtual double theta() const { return theta_; }
    virtual int platformKnowledgeReliability() const { return 7; }
    virtual QPointF pos() const { return QPointF(x_,y_); };

private:
    orcaqgui::IHumanManager *humanManager_;
    Localise2dPainter painter_;
    bool gotDescription_;
    orca::Frame2d localiseOff_;

    double x_;
    double y_;
    double theta_;
};

}

#endif
