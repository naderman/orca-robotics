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

#include <orcaqgui/exceptions.h>
#include <orcaqgui/ihumanmanager.h>

#include <orcaqgui2dfactory/gridelement.h>
#include <orcaqgui2dfactory/simpleguielements.h>
#include <orcaqgui2dfactory/featuremap2delement.h>
#include <orcaqgui2dfactory/ogmapelement.h>
#include <orcaqgui2dfactory/localise2delement.h>
#include <orcaqgui2dfactory/pathfollower2delement.h>
#include <orcaqgui2dfactory/pathplanner2delement.h>
#include <orcaqgui2dfactory/ogmapscombinedelement.h>
#include <orcaqgui2dfactory/pixmapelement.h>
#include <orcaqgui2dfactory/wifielement.h>

#include "defaultfactory.h"

using namespace std;

namespace orcaqgui2d {

DefaultFactory::DefaultFactory()
{
    addSupportedType("Grid");
    addSupportedType("LaserScanner2d");
    addSupportedType("Localise2d");
    addSupportedType("Localise3d");
    addSupportedType("OgMap");
    addSupportedType("PolarFeature2d");
    addSupportedType("PathFollower2d");
    addSupportedType("PathPlanner2d");
    addSupportedType("FeatureMap2d");
    addSupportedType("Particle2d");
    addSupportedType("PixMap");
    addSupportedType("QGraphics2d");
    addSupportedType("Wifi");
    addSupportedType("MuliOgMaps");
}


bool 
DefaultFactory::lookupElementType( const QStringList &ids, QString &elementType ) const
{
    // One-to-one mappings (one interface per element):
    // ::orca::LaserScanner2d maps to LaserScanner2d
    if (ids.size()==1)
    {
        elementType = ids[0].section(':',4,4);
    }
    // One element with two interfaces
    // Please extend as required
    else if (ids.size()==2)
    {
        if (ids[0]=="::orca::OgMap" && ids[1]=="::orca::OgMap")
            elementType="MuliOgMaps";
        else
            return false;
    }
    else
    {
        return false;    
    }
    
    return true;
}
        
orcaqgui::GuiElement* 
DefaultFactory::create( const orcaice::Context         &context,
                        const QString                  &elementType,
                        const QStringList              &elementDetails,
                        QColor                          suggestedColor,
                        orcaqgui::IHumanManager        *humanManager ) const
{
    orcaqgui::GuiElement *elem = NULL;
    
    try
    {
        if ( elementType == "Grid" ) {
                cout<<"creating Grid element"<<endl;
                elem = new orcaqgui2d::GridElement();
        }
        else if ( elementType == "LaserScanner2d" ) {
            cout<<"creating LaserScanner2d element"<<endl;
                elem = new orcaqgui2d::LaserScanner2dElement( context, elementDetails[0].toStdString() );
        }
        else if ( elementType == "MuliOgMaps" ) {
            cout<<"creating OgMapsCombined element"<<endl;
            elem = new orcaqgui2d::OgMapsCombinedElement( context, elementDetails );
        }
        else if ( elementType == "Localise2d" ) {
            cout<<"creating Localise2d element"<<endl;
            elem = new orcaqgui2d::Localise2dElement( context, elementDetails[0].toStdString(), humanManager );
        }
        else if ( elementType == "Localise3d" ) {
            cout<<"creating Localise3d element"<<endl;
            elem = new orcaqgui2d::Localise3dElement( context, elementDetails[0].toStdString() );
        }
        else if ( elementType == "OgMap" ) {
            cout<<"creating OgMap element"<<endl;
            elem = new orcaqgui2d::OgMapElement( context, elementDetails[0].toStdString(), humanManager );
        }
        else if ( elementType == "PolarFeature2d" ) {
            cout<<"creating PolarFeature2d element"<<endl;
            elem = new orcaqgui2d::PolarFeature2dElement( context, elementDetails[0].toStdString() );
        }
        else if ( elementType == "PathFollower2d" ) {
            cout<<"creating PathFollower2d element with details "<<elementDetails[0].toStdString()<<endl;
            elem = new orcaqgui2d::PathFollower2dElement( context, elementDetails[0].toStdString(), humanManager );
        }
        else if ( elementType == "PathPlanner2d" ) {
            cout<<"creating PathPlanner2d element with details "<<elementDetails[0].toStdString()<<endl;
            elem = new orcaqgui2d::PathPlanner2dElement( context, elementDetails[0].toStdString(), humanManager );
        }
        else if ( elementType == "PixMap" ) {
            cout<<"creating PixMap element with details "<<elementDetails[0].toStdString()<<endl;
            elem = new orcaqgui2d::PixMapElement( context, elementDetails[0].toStdString(), humanManager );
        }
        else if ( elementType == "FeatureMap2d" ) {
            cout<<"creating FeatureMap2d element with details "<<elementDetails[0].toStdString()<<endl;
            elem = new orcaqgui2d::FeatureMap2dElement( context, elementDetails[0].toStdString(), humanManager );
        }
        else if ( elementType == "Particle2d" ) {
            cout<<"creating Particle2d element with details "<<elementDetails[0].toStdString()<<endl;
            elem = new orcaqgui2d::Particle2dElement( context, elementDetails[0].toStdString() );
        }
        else if ( elementType == "QGraphics2d" ) {
            cout<<"creating QGraphics2d element with details "<<elementDetails[0].toStdString()<<endl;
            elem = new orcaqgui2d::QGraphics2dElement( context, elementDetails[0].toStdString() );
        }
        else if ( elementType == "Wifi" ) {
            cout<<"creating Wifi element with details "<<elementDetails[0].toStdString()<<endl;
            elem = new orcaqgui2d::WifiElement( context, elementDetails[0].toStdString() );
        }
        else
        {
            cout << "ERROR(guielementfactory.cpp): Don't know how to handle elementType '" << elementType.toStdString() << "'" << endl;
        }
    
    } 
    catch (orcaqgui::Exception &e)
    {
        cout << "Problem when trying to create element: " << e.what() << endl;
        delete elem;
        elem=NULL;
    }

    if ( elem != NULL )
        elem->setColor( suggestedColor );
    return elem;
}

}


orcaqgui::GuiElementFactory *createFactory()
{
    return new orcaqgui2d::DefaultFactory;
}
