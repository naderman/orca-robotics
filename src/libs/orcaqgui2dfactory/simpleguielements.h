/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_SIMPLE_GUI_ELEMENTS_H
#define ORCAGUI_SIMPLE_GUI_ELEMENTS_H

#include <QPointF>
#include <QObject>

// for setInit()
#include <orcaobj/orcaobj.h>
// for getPropertyAsCartesianPoint
#include <orcaice/orcaice.h>

#include <orcaqgui2d/ptricestormelement.h>
#include <orcaqgui2d/icestormelement.h>
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui2d/iknowsplatformposition2d.h>
#include <orcaqgui2dfactory/connectutils.h>

// Include various painter types
#include <orcaqgui2dfactory/laserscanner2dpainter.h>
#include <orcaqgui2dfactory/localise3dpainter.h>
#include <orcaqgui2dfactory/odometry2dpainter.h>
#include <orcaqgui2dfactory/particle2dpainter.h>
#include <orcaqgui2dfactory/polarfeature2dpainter.h>
#include <orcaqgui2dfactory/qgraphics2dpainter.h>

//
// A list of simple gui elements that listen to one icestorm topic, for one object type.
//

namespace orcaqgui2d
{

////////////////////////////////////////////////////////////////////////////////
class PolarFeature2dElement
    : public PtrIceStormElement<PolarFeature2dPainter,
                               orca::PolarFeature2dData,
                               orca::PolarFeature2dDataPtr,
                               orca::PolarFeature2dPrx,
                               orca::PolarFeature2dConsumer,
                               orca::PolarFeature2dConsumerPrx>
{
public:
    PolarFeature2dElement( const orcaice::Context  &context,
                           const std::string       &proxyString,
                           int                      timeoutMs=30000 )
        : PtrIceStormElement<PolarFeature2dPainter,
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
class LaserScanner2dElement
    : public PtrIceStormElement<LaserScanner2dPainter,
                             orca::RangeScanner2dData,
                             orca::RangeScanner2dDataPtr,
                             orca::LaserScanner2dPrx,
                             orca::RangeScanner2dConsumer,
                             orca::RangeScanner2dConsumerPrx>
{           
public:
    LaserScanner2dElement( const orcaice::Context  &context,
                  const std::string       &proxyString,
                  int                      timeoutMs=30000,
                  QColor                   outlineColor=QColor( 102,102,153, 255 ),
                  float                    outlineThickness=-1,
                  float                    brightReturnWidth=0.2 )
        : PtrIceStormElement<LaserScanner2dPainter,
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
class RangeScanner2dElement
    : public PtrIceStormElement<LaserScanner2dPainter,
                                orca::RangeScanner2dData,
                                orca::RangeScanner2dDataPtr,
                                orca::RangeScanner2dPrx,
                                orca::RangeScanner2dConsumer,
                                orca::RangeScanner2dConsumerPrx>
{           
public:
    RangeScanner2dElement( const orcaice::Context  &context,
                  const std::string       &proxyString,
                  int                      timeoutMs=30000,
                  QColor                   outlineColor=QColor( 102,102,153, 255 ),
                  float                    outlineThickness=-1,
                  float                    brightReturnWidth=0.2 )
        : PtrIceStormElement<LaserScanner2dPainter,
                             orca::RangeScanner2dData,
                             orca::RangeScanner2dDataPtr,
                             orca::RangeScanner2dPrx,
                             orca::RangeScanner2dConsumer,
                             orca::RangeScanner2dConsumerPrx>(context, proxyString, painter_, timeoutMs ),
          painter_( outlineColor, outlineThickness, brightReturnWidth )
        {};

    virtual bool isInGlobalCS() { return false; }
    virtual void actionOnConnection() { getScannerInfo(); }
    virtual QStringList contextMenu();
    virtual void execute( int action ) { painter_.execute( action ); };
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };

private:
    void getScannerInfo();
    LaserScanner2dPainter painter_;
    
};


////////////////////////////////////////////////////////////////////////////////
class Localise3dElement
    : public IceStormElement<Localise3dPainter,
                             orca::Localise3dData,
                             orca::Localise3dPrx,
                             orca::Localise3dConsumer,
                             orca::Localise3dConsumerPrx>,
      public IKnowsPlatformPosition2d
{
public:
    Localise3dElement( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       bool                     beginDisplayHistory=false,
                       int                      timeoutMs=30000 )
        : IceStormElement<Localise3dPainter,
                            orca::Localise3dData,
                            orca::Localise3dPrx,
                            orca::Localise3dConsumer,
                            orca::Localise3dConsumerPrx>(context, proxyString, painter_, timeoutMs ),
          painter_( beginDisplayHistory ),
          x_(0),
          y_(0),
          theta_(0)
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.";

    orcaice::setInit( origin_ );
    orcaice::getPropertyAsCartesianPoint( prop, prefix+"General.Offset", origin_ );

}
    virtual bool isInGlobalCS() { return true; }
    virtual void actionOnConnection();
    virtual QStringList contextMenu();
    virtual void execute( int action );
    
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setFocus( bool inFocus ) { painter_.setFocus( inFocus ); };
    virtual void setTransparency( bool useTransparency ) { painter_.setTransparency( useTransparency ); };

    // Need a special update function to set (x,y,theta)
    // The Localise3dElement needs this because it's special kind of GuiElement:
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
    Localise3dPainter painter_;

    double x_;
    double y_;
    double theta_;
    
    void tryToGetGeometry();
    bool haveGeometry_;

    orca::CartesianPoint origin_; 
};

///////////////////////////////////////////////////////////////////////////////

class Particle2dElement
    : public IceStormElement<Particle2dPainter,
                             orca::Particle2dData,
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
    virtual void setTransparency( bool useTransparency ) { painter_.setTransparency( useTransparency ); };

private:
    Particle2dPainter painter_;
};

////////////////////////////////////////////////////////////////////////////////


class QGraphics2dElement
    : public IceStormElement<QGraphics2dPainter,
                             orca::QGraphics2dData,
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

////////////////////////////////////////////////////////////////////////////////
class Odometry2dElement
    : public IceStormElement<Odometry2dPainter,
                             orca::Odometry2dData,
                             orca::Odometry2dPrx,
                             orca::Odometry2dConsumer,
                             orca::Odometry2dConsumerPrx>
{
public:
    Odometry2dElement( const orcaice::Context  &context,
                       const std::string       &proxyString,
                       int                      timeoutMs=60000 )
        : IceStormElement<Odometry2dPainter,
                            orca::Odometry2dData,
                            orca::Odometry2dPrx,
                            orca::Odometry2dConsumer,
                            orca::Odometry2dConsumerPrx>(context, proxyString, painter_, timeoutMs )
        {};

    virtual bool isInGlobalCS() { return false; }
    virtual void actionOnConnection()
    {
        paintInitialData<orca::Odometry2dPrx, Odometry2dPainter>
            ( context_, listener_.interfaceName(), painter_ );
    }
    virtual void setColor( QColor color ) { painter_.setColor(color); }
    virtual void setTransparency( bool useTransparency ) { painter_.setTransparency( useTransparency ); };

private:
    Odometry2dPainter painter_;
};

} // namespace

#endif
