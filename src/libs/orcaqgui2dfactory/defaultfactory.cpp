/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaqgui2dfactory/gridelement.h>
#include <orcaqgui2dfactory/simpleguielements.h>
#include <orcaqgui2dfactory/featuremap2delement.h>
#include <orcaqgui2dfactory/ogmapelement.h>
#include <orcaqgui2dfactory/pathfollower2delement.h>
#include <orcaqgui2dfactory/pathplanner2delement.h>
#include <orcaqgui2dfactory/gpselement.h>

#include "defaultfactory.h"
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui/mainwin.h>

using namespace std;

namespace orcaqgui {

DefaultFactory::DefaultFactory()
{
    addSupportedType("::local::Grid");
    addSupportedType("::orca::LaserScanner2d");
    addSupportedType("::orca::Localise2d");
    addSupportedType("::orca::OgMap");
    addSupportedType("::orca::PolarFeature2d");
    addSupportedType("::orca::PathFollower2d");
    addSupportedType("::orca::PathPlanner2d");
    addSupportedType("::orca::FeatureMap2d");
    addSupportedType("::orca::Particle2d");
    addSupportedType("::orca::Gps");
    addSupportedType("::orca::QGraphics2d");
}

orcaqgui::GuiElement*
DefaultFactory::create( const orcaice::Context           &context,
                        const QString                    &interfaceId,
                        const QString                    &proxyStr,
                        QColor                            suggestedColor,
                        IHumanManager                    *humanManager ) const
{
    orcaqgui::GuiElement *elem = NULL;
   
    if ( interfaceId == "::local::Grid" ) {
        cout<<"creating Grid element"<<endl;
        elem = new orcaqgui::GridElement();
    }
    else if ( interfaceId == "::orca::LaserScanner2d" ) {
        cout<<"creating LaserScanner2d element"<<endl;
            elem = new orcaqgui::LaserScanner2dElement( context, proxyStr.toStdString() );
    }
    else if ( interfaceId == "::orca::Localise2d" ) {
        cout<<"creating Localise2d element"<<endl;
        elem = new orcaqgui::Localise2dElement( context, proxyStr.toStdString() );
    }
    else if ( interfaceId == "::orca::OgMap" ) {
        cout<<"creating OgMap element"<<endl;
        elem = new orcaqgui::OgMapElement( context, proxyStr.toStdString(), humanManager );
    }
    else if ( interfaceId == "::orca::PolarFeature2d" ) {
        cout<<"creating PolarFeature2d element"<<endl;
        elem = new orcaqgui::PolarFeature2dElement( context, proxyStr.toStdString() );
    }
    else if ( interfaceId == "::orca::PathFollower2d" ) {
        cout<<"creating PathFollower2d element with proxyString "<<proxyStr.toStdString()<<endl;
        elem = new orcaqgui::PathFollower2dElement( context, proxyStr.toStdString(), humanManager );
    }
    else if ( interfaceId == "::orca::PathPlanner2d" ) {
        cout<<"creating PathPlanner2d element with proxyString "<<proxyStr.toStdString()<<endl;
        elem = new orcaqgui::PathPlanner2dElement( context, proxyStr.toStdString(), humanManager );
    }
    else if ( interfaceId == "::orca::FeatureMap2d" ) {
        cout<<"creating FeatureMap2d element with proxyString "<<proxyStr.toStdString()<<endl;
        elem = new orcaqgui::FeatureMap2dElement( context, proxyStr.toStdString(), humanManager );
    }
    else if ( interfaceId == "::orca::Particle2d" ) {
        cout<<"creating Particle2d element with proxyString "<<proxyStr.toStdString()<<endl;
        elem = new orcaqgui::Particle2dElement( context, proxyStr.toStdString() );
    }
    else if ( interfaceId == "::orca::Gps" ) {
        cout<<"creating Gps element with proxyString "<<proxyStr.toStdString()<<endl;
        elem = new orcaqgui::GpsElement( context, proxyStr.toStdString() );
    }
    else if ( interfaceId == "::orca::QGraphics2d" ) {
        cout<<"creating QGraphics2d element with proxyString "<<proxyStr.toStdString()<<endl;
        elem = new orcaqgui::QGraphics2dElement( context, proxyStr.toStdString() );
    }
    else
    {
        cout << "ERROR(guielementfactory.cpp): Don't know how to handle interfaceId '" << interfaceId.toStdString() << "'" << endl;
        elem = NULL;
    }

    if ( elem != NULL )
        elem->setColor( suggestedColor );
    return elem;
}

}


orcaqgui::GuiElementFactory *createFactory()
{
    return new orcaqgui::DefaultFactory;
}
