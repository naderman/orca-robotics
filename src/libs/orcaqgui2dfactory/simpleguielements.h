/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_SIMPLE_GUI_ELEMENTS_H
#define ORCAGUI_SIMPLE_GUI_ELEMENTS_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

#include <QPointF>
#include <QObject>

#include <orcaqgui2d/icestormelement.h>
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2d/iknowsplatformposition2d.h>
#include <orcaqgui2dfactory/connectutils.h>

// Include various painter types
#include <orcaqgui2dfactory/laserscanner2dpainter.h>
#include <orcaqgui2dfactory/localise2dpainter.h>
#include <orcaqgui2dfactory/particle2dpainter.h>
#include <orcaqgui2dfactory/polarfeature2dpainter.h>
#include <orcaqgui2dfactory/qgraphics2dpainter.h>

//
// A list of simple gui elements that listen to one icestorm topic, for one object type.
//

namespace orcaqgui
{

class PolarFeature2dElement
    : public IceStormElement<PolarFeature2dPainter,
                               orca::PolarFeature2dData,
                               orca::PolarFeature2dDataPtr,
                               orca::PolarFeature2dPrx,
                               orca::PolarFeature2dConsumer,
                               orca::PolarFeature2dConsumerPrx>
{
public:
    PolarFeature2dElement( const orcaice::Context  &context,
                           const std::string       &proxyString,
                           int                      timeoutMs=60000 )
        : IceStormElement<PolarFeature2dPainter,
                            orca::PolarFeature2dData,
                            orca::PolarFeature2dDataPtr,
                            orca::PolarFeature2dPrx,
                            orca::PolarFeature2dConsumer,
                            orca::PolarFeature2dConsumerPrx>( context, proxyString, painter_, timeoutMs )
          {};

    virtual bool isInGlobalCS() { return false; }
    virtual void actionOnConnection() 
    { 
        paintInitialData<orca::PolarFeature2dPrx, PolarFeature2dPainter>
            ( context_, listener_.interfaceName(), painter_ ); 
    }

    virtual QStringList contextMenu();
//     virtual void execute( int action ) { painter_.execute( action ); };

private:

    PolarFeature2dPainter painter_;
};

////////////////////////////////////////////////////////////////////////////////
// A laser GUI element
class LaserScanner2dElement
    : public IceStormElement<LaserScanner2dPainter,
                             orca::RangeScanner2dData,
                             orca::RangeScanner2dDataPtr,
                             orca::LaserScanner2dPrx,
                             orca::RangeScanner2dConsumer,
                             orca::RangeScanner2dConsumerPrx>
{           
public:
    LaserScanner2dElement( const orcaice::Context  &context,
                  const std::string       &proxyString,
                  int                      timeoutMs=60000,
                  QColor                   outlineColor=QColor( 102,102,153, 255 ),
                  float                    outlineThickness=-1,
                  float                    brightReturnWidth=0.2 )
        : IceStormElement<LaserScanner2dPainter,
                          orca::RangeScanner2dData,
                          orca::RangeScanner2dDataPtr,
                          orca::LaserScanner2dPrx,
                          orca::RangeScanner2dConsumer,
                          orca::RangeScanner2dConsumerPrx>(context, proxyString, painter_, timeoutMs ),
          painter_( outlineColor, outlineThickness, brightReturnWidth )
        {};

    virtual bool isInGlobalCS() { return false; }
    virtual void actionOnConnection() { getLaserInfo(); }
    virtual QStringList contextMenu();
    virtual void execute( int action ) { painter_.execute( action ); };
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };

private:
    void getLaserInfo();
    LaserScanner2dPainter painter_;
    
};

////////////////////////////////////////////////////////////////////////////////


class Localise2dElement
    : public IceStormElement<Localise2dPainter,
                             orca::Localise2dData,
                             orca::Localise2dDataPtr,
                             orca::Localise2dPrx,
                             orca::Localise2dConsumer,
                             orca::Localise2dConsumerPrx>,
      public IKnowsPlatformPosition2d
{
public:
    Localise2dElement( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       bool                     beginDisplayHistory=false,
                       int                      timeoutMs=60000 )
        : IceStormElement<Localise2dPainter,
                            orca::Localise2dData,
                            orca::Localise2dDataPtr,
                            orca::Localise2dPrx,
                            orca::Localise2dConsumer,
                            orca::Localise2dConsumerPrx>(context, proxyString, painter_, timeoutMs ),
          painter_( beginDisplayHistory )
        {};

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection()
    {
        paintInitialData<orca::Localise2dPrx, Localise2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };

    // Need a special update function to set (x,y,theta)
    // The Localise2dElement needs this because it's special kind of GuiElement:
    //   A platform has a position in the world and various other things are
    //   drawn with respect to that position.
    virtual void update();

    // Access to ML estimate.
    virtual float x() const { return x_; }
    virtual float y() const { return y_; }
    virtual float theta() const { return theta_; }
    virtual int platformKnowledgeReliability() const { return 7; }
    virtual QPointF pos() const { return QPointF(x_,y_); };

private:
    Localise2dPainter painter_;

    float x_;
    float y_;
    float theta_;
};

////////////////////////////////////////////////////////////////////////////////


class Particle2dElement
    : public IceStormElement<Particle2dPainter,
                             orca::Particle2dData,
                             orca::Particle2dDataPtr,
                             orca::Particle2dPrx,
                             orca::Particle2dConsumer,
                             orca::Particle2dConsumerPrx>
{
public:
    Particle2dElement( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       int                      timeoutMs=60000 )
        : IceStormElement<Particle2dPainter,
                            orca::Particle2dData,
                            orca::Particle2dDataPtr,
                            orca::Particle2dPrx,
                            orca::Particle2dConsumer,
                            orca::Particle2dConsumerPrx>(context, proxyString, painter_, timeoutMs )
        {};

    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection()
    {
        paintInitialData<orca::Particle2dPrx, Particle2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }
    virtual void setColor( QColor color ) { painter_.setColor(color); }

private:
    Particle2dPainter painter_;
};

////////////////////////////////////////////////////////////////////////////////


class QGraphics2dElement
    : public IceStormElement<QGraphics2dPainter,
                             orca::QGraphics2dData,
                             orca::QGraphics2dDataPtr,
                             orca::QGraphics2dPrx,
                             orca::QGraphics2dConsumer,
                             orca::QGraphics2dConsumerPrx>
{
public:
    QGraphics2dElement( const orcaice::Context  &context,
                        const std::string       &proxyString,
                        int                      timeoutMs=-1 )
    : IceStormElement<QGraphics2dPainter,
                          orca::QGraphics2dData,
                          orca::QGraphics2dDataPtr,
                          orca::QGraphics2dPrx,
                          orca::QGraphics2dConsumer,
                          orca::QGraphics2dConsumerPrx>(context, proxyString, painter_, timeoutMs )
    {
    };

    virtual bool isInGlobalCS() { return painter_.isInGlobalCS(); }
    virtual void actionOnConnection() 
    { 
        paintInitialData<orca::QGraphics2dPrx,QGraphics2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }

private:
    void getInitialAppearance();
    QGraphics2dPainter painter_;
};

} // namespace

#endif
