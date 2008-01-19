/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>

#include <hydroqgui/hydroqgui.h>
#include <hydroqgui/hydroqgui.h>

#include <orcaqgui2dfactory/buttonelement.h>
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
    : isContextSet_(false)
{
    addSupportedType("FeatureMap2d");
    addSupportedType("Button");
    addSupportedType("Grid");
    addSupportedType("LaserScanner2d");
    addSupportedType("RangeScanner2d");
    addSupportedType("Localise2d");
    addSupportedType("Localise3d");
    addSupportedType("MultiOgMaps");
    addSupportedType("Odometry2d");
    addSupportedType("OgMap");
    addSupportedType("PathFollower2d");
    addSupportedType("PathPlanner2d");
    addSupportedType("Particle2d");
    addSupportedType("PixMap");
    addSupportedType("PolarFeature2d");
    addSupportedType("QGraphics2d");
    addSupportedType("Wifi");
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
            elementType="MultiOgMaps";
        else
            return false;
    }
    else
    {
        return false;    
    }
    
    return true;
}
        
hydroqgui::IGuiElement* 
DefaultFactory::create( const QString                &elementType,
                        const QStringList            &elementDetails,
                        QColor                        suggestedColor,
                        hydroqgui::IHumanManager     &humanManager,
                        hydroqgui::MouseEventManager &mouseEventManager,
                        hydroqgui::ShortcutKeyManager &shortcutKeyManager,
                        const hydroqgui::GuiElementSet &guiElementSet ) const
{
    assert( isContextSet_ );

    hydroqgui::IGuiElement *elem = NULL;
    
    try
    {
        stringstream ss;
        ss<<"DefaultFactory::create(): creating "<<elementType.toStdString()<<" element";
        if ( elementDetails.size() > 0 )
        {
            ss << " with details:";
            for ( int i=0; i < elementDetails.size(); i++ )
            {
                ss << " " << elementDetails[i].toStdString();
            }
        }
        cout << ss.str() << endl;

        if ( elementType == "Button" ) {
            elem = new orcaqgui2d::ButtonElement( context_, elementDetails[0].toStdString(), humanManager );
        }
        else if ( elementType == "Grid" ) {
            elem = new orcaqgui2d::GridElement();
        }
        else if ( elementType == "LaserScanner2d" ) {
            elem = new orcaqgui2d::LaserScanner2dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "RangeScanner2d" ) {
            elem = new orcaqgui2d::RangeScanner2dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "MultiOgMaps" ) {
            elem = new orcaqgui2d::OgMapsCombinedElement( context_, elementDetails );
        }
        else if ( elementType == "Localise2d" ) {
            elem = new orcaqgui2d::Localise2dElement( context_, elementDetails[0].toStdString(), &humanManager );
        }
        else if ( elementType == "Localise3d" ) {
            elem = new orcaqgui2d::Localise3dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "Odometry2d" ) {
            elem = new orcaqgui2d::Odometry2dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "OgMap" ) {
            elem = new orcaqgui2d::OgMapElement( context_, elementDetails[0].toStdString(), &humanManager );
        }
        else if ( elementType == "PolarFeature2d" ) {
            elem = new orcaqgui2d::PolarFeature2dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "PathFollower2d" ) {
            elem = new orcaqgui2d::PathFollower2dElement( context_, elementDetails[0].toStdString(), humanManager, mouseEventManager, shortcutKeyManager, guiElementSet );
        }
        else if ( elementType == "PathPlanner2d" ) {
            elem = new orcaqgui2d::PathPlanner2dElement( context_, elementDetails[0].toStdString(), humanManager, mouseEventManager );
        }
        else if ( elementType == "PixMap" ) {
            elem = new orcaqgui2d::PixMapElement( context_, elementDetails[0].toStdString(), &humanManager );
        }
        else if ( elementType == "FeatureMap2d" ) {
            elem = new orcaqgui2d::FeatureMap2dElement( context_, elementDetails[0].toStdString(), &humanManager );
        }
        else if ( elementType == "Particle2d" ) {
            elem = new orcaqgui2d::Particle2dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "QGraphics2d" ) {
            elem = new orcaqgui2d::QGraphics2dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "Wifi" ) {
            elem = new orcaqgui2d::WifiElement( context_, elementDetails[0].toStdString() );
        }
        else
        {
            cout << "DefaultFactory::create(): Don't know how to handle elementType '" << elementType.toStdString() << "'" << endl;
        }
    
    } 
    catch (hydroqgui::Exception &e)
    {
        cout << "DefaultFactory::create(): Element creation failed: " << e.what() << endl;
        delete elem;
        elem=NULL;
    }

    if ( elem != NULL )
        elem->setColor( suggestedColor );
    return elem;
}

}


hydroqgui::IGuiElementFactory *createFactory()
{
    return new orcaqgui2d::DefaultFactory;
}
