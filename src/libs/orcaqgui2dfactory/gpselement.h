/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_GPS_ELEMENT_H
#define ORCAGUI_GPS_ELEMENT_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>
#include <orca/bros1.h>
#include <orca/gps.h>
#include <orcaqgui2d/icestormelement.h>

#include <orcaqgui2dfactory/gpspainter.h>
#include <orcaqgui2d/iknowsplatformposition2d.h>

// forward declarations
class QPainter;

namespace orcaqgui
{

class GpsSubscriptionMaker 
{
public:
    GpsSubscriptionMaker( orca::GpsPrx proxy, orca::GpsMapGridConsumerPrx callbackPrx )
    { 
        proxy->subscribeForMapGrid( callbackPrx );
    }
};

class GpsUnSubscriptionMaker 
{
public:
    GpsUnSubscriptionMaker( orca::GpsPrx proxy, orca::GpsMapGridConsumerPrx callbackPrx )
    { 
        proxy->subscribeForMapGrid( callbackPrx );
    }
};


// GpsElement connects to gps interface, passes info to gpsPainter
// This element is special because it uses non-standard subscriptions: 'subscribeForMapGrid'
// That's why we can't inherit from IceStormElement, we rather use the IceStormListener directly
class GpsElement : public GuiElement2d, public IKnowsPlatformPosition2d
{    
public:

    GpsElement( const orcaice::Context  &context,
                const std::string       &interfaceTag,
                int                      timeoutMs=60000 );

    // inherited from GuiElement
    virtual bool isInGlobalCS() { return true; }
    virtual QStringList contextMenu();
    virtual void execute( int action );

    // inherited from GuiElement2d
    void update();
    void paint( QPainter *p, int z )
    { 
        if ( displayGps_ )
            gpsPainter_.paint( p, z );
    }
    bool paintThisLayer( int z ) const
    {
        if ( !displayGps_ ) return false;
        return gpsPainter_.paintThisLayer(z);
    }
    virtual QPointF pos() const { return QPointF(x_,y_); };

    // inherited from IKnowsPlatformPosition2d
    // Access to coordinates in GUI coordinate system [m] [m] [rad]
    virtual float x() const { return x_; }
    virtual float y() const { return y_; }
    virtual float theta() const { return theta_; }
    virtual int platformKnowledgeReliability() const { return 7; }


private:
    std::string interfaceName_;

    GpsPainter gpsPainter_;

    double timeoutMs_;
    
    IceStormListener<orca::GpsMapGridData,
                    orca::GpsPrx,
                    orca::GpsMapGridConsumer,
                    orca::GpsMapGridConsumerPrx,
                    GpsSubscriptionMaker,
                    GpsUnSubscriptionMaker> gpsListener_;

    orcaice::Context context_;      
    
    // display options and settings
    bool displayGps_;
    orca::CartesianPoint gpsOrigin_;   
    
    // in GUI's coordinate system: [m] [m] [rad]
    float x_;
    float y_;
    float theta_;

    // utilities
    bool needToUpdate();
};

}

#endif
