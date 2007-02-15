/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
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
#include <orcaqgui2dfactory/ogmapscombinedelement.h>
#include <orcaqgui2dfactory/pixmapelement.h>
#include <orcaqgui2dfactory/wifielement.h>

#include "defaultfactory.h"
#include <orcaqgui/ihumanmanager.h>
#include <orcaqgui/mainwin.h>

using namespace std;

namespace orcaqgui {

DefaultFactory::DefaultFactory()
{
    addSupportedType(QStringList("::local::Grid"));
    addSupportedType(QStringList("::orca::LaserScanner2d"));
    addSupportedType(QStringList("::orca::Localise2d"));
    addSupportedType(QStringList("::orca::Localise3d"));
    addSupportedType(QStringList("::orca::OgMap"));
    addSupportedType(QStringList("::orca::PolarFeature2d"));
    addSupportedType(QStringList("::orca::PathFollower2d"));
    addSupportedType(QStringList("::orca::PathPlanner2d"));
    addSupportedType(QStringList("::orca::FeatureMap2d"));
    addSupportedType(QStringList("::orca::Particle2d"));
    addSupportedType(QStringList("::orca::PixMap"));
    addSupportedType(QStringList("::orca::Gps"));
    addSupportedType(QStringList("::orca::QGraphics2d"));
    addSupportedType(QStringList("::orca::Wifi"));
    
    // multiple interfaces in one element
    QStringList ogMapList = (QStringList() << "::orca::OgMap" << "::orca::OgMap");
    addSupportedType(ogMapList);
}

orcaqgui::GuiElement*
DefaultFactory::create( const orcaice::Context           &context,
                        const QStringList                &interfaceIds,
                        const QStringList                &proxyStrList,
                        QColor                            suggestedColor,
                        IHumanManager                    *humanManager ) const
{
    orcaqgui::GuiElement *elem = NULL;
   
    if (interfaceIds.size()==1)
    {
        QString interfaceId = interfaceIds[0];
        QString proxyString = proxyStrList[0];
    
        if ( interfaceId == "::local::Grid" ) {
            cout<<"creating Grid element"<<endl;
            elem = new orcaqgui::GridElement();
        }
        else if ( interfaceId == "::orca::LaserScanner2d" ) {
            cout<<"creating LaserScanner2d element"<<endl;
                elem = new orcaqgui::LaserScanner2dElement( context, proxyString.toStdString() );
        }
        else if ( interfaceId == "::orca::Localise2d" ) {
            cout<<"creating Localise2d element"<<endl;
            elem = new orcaqgui::Localise2dElement( context, proxyString.toStdString(), humanManager );
        }
        else if ( interfaceId == "::orca::Localise3d" ) {
            cout<<"creating Localise3d element"<<endl;
            elem = new orcaqgui::Localise3dElement( context, proxyString.toStdString() );
        }
        else if ( interfaceId == "::orca::OgMap" ) {
            cout<<"creating OgMap element"<<endl;
            elem = new orcaqgui::OgMapElement( context, proxyString.toStdString(), humanManager );
        }
        else if ( interfaceId == "::orca::PolarFeature2d" ) {
            cout<<"creating PolarFeature2d element"<<endl;
            elem = new orcaqgui::PolarFeature2dElement( context, proxyString.toStdString() );
        }
        else if ( interfaceId == "::orca::PathFollower2d" ) {
            cout<<"creating PathFollower2d element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::PathFollower2dElement( context, proxyString.toStdString(), humanManager );
        }
        else if ( interfaceId == "::orca::PathPlanner2d" ) {
            cout<<"creating PathPlanner2d element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::PathPlanner2dElement( context, proxyString.toStdString(), humanManager );
        }
        else if ( interfaceId == "::orca::PixMap" ) {
            cout<<"creating PixMap element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::PixMapElement( context, proxyString.toStdString(), humanManager );
        }
        else if ( interfaceId == "::orca::FeatureMap2d" ) {
            cout<<"creating FeatureMap2d element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::FeatureMap2dElement( context, proxyString.toStdString(), humanManager );
        }
        else if ( interfaceId == "::orca::Particle2d" ) {
            cout<<"creating Particle2d element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::Particle2dElement( context, proxyString.toStdString() );
        }
        else if ( interfaceId == "::orca::Gps" ) {
            cout<<"creating Gps element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::GpsElement( context, proxyString.toStdString(), humanManager );
        }
        else if ( interfaceId == "::orca::QGraphics2d" ) {
            cout<<"creating QGraphics2d element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::QGraphics2dElement( context, proxyString.toStdString() );
        }
        else if ( interfaceId == "::orca::Wifi" ) {
            cout<<"creating Wifi element with proxyString "<<proxyString.toStdString()<<endl;
            elem = new orcaqgui::WifiElement( context, proxyString.toStdString() );
        }
        else
        {
            cout << "ERROR(guielementfactory.cpp): Don't know how to handle interfaceId '" << interfaceId.toStdString() << "'" << endl;
            elem = NULL;
        }
    }
    else if (interfaceIds.size()==2)
    {
        QString interfaceId = interfaceIds[0] + interfaceIds[1];
        
        if ( interfaceId == "::orca::OgMap::orca::OgMap" ) {
            cout<<"creating OgMapsCombined element"<<endl;
            elem = new orcaqgui::OgMapsCombinedElement( context, proxyStrList );
        }
        else
        {
            cout << "ERROR(guielementfactory.cpp): Don't know how to handle two-interface element with interfaceIds '" << interfaceId.toStdString() << "'" << endl;
            elem = NULL;
        }
    }
    else
    {
        cout << "ERROR(guielementfactory.cpp): Factory does not support elements with more than two interfaces" << endl;
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
