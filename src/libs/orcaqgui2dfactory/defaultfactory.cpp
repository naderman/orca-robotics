/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <hydroqgui/exceptions.h>
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
#include <orcaqgui2dfactory/velocitycontrol2delement.h>

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
    addSupportedType("RangerArray");
    addSupportedType("Odometry2d");
    addSupportedType("DriveBicycle");
    addSupportedType("OgMap");
    addSupportedType("PathFollower2d");
    addSupportedType("PathPlanner2d");
    addSupportedType("Particle2d");
    addSupportedType("PixMap");
    addSupportedType("PolarFeature2d");
    addSupportedType("QGraphics2d");
    addSupportedType("VelocityControl2d");
    addSupportedType("Wifi");
}


bool 
DefaultFactory::lookupElementType( const QStringList &ids, QString &elementType ) const
{
    // One-to-one mappings (one interface per element):
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
        
hydroqguielementutil::GuiElement* 
DefaultFactory::create( const hydroqguielementutil::GuiElementInfo &guiElementInfo,
                        hydroqguielementutil::IHumanManager        &humanManager,
                        hydroqguielementutil::MouseEventManager    &mouseEventManager,
                        hydroqguielementutil::ShortcutKeyManager   &shortcutKeyManager,
                        const hydroqgui::GuiElementSet             &guiElementSet,
                        QSplitter                                  *spaceBottomRight ) const
{
    assert( isContextSet_ );
    hydroqguielementutil::GuiElement *elem = NULL;
    
    try
    {
        cout << "DefaultFactory::create(): creating "<< guiElementInfo.type.toStdString() << " element with id:" << guiElementInfo.uniqueId.toStdString() << endl;
        
        if ( guiElementInfo.type == "Button" ) {
            elem = new orcaqgui2d::ButtonElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), humanManager );
        }
        else if ( guiElementInfo.type == "Grid" ) {
            elem = new orcaqgui2d::GridElement( guiElementInfo);
        }
        else if ( guiElementInfo.type == "LaserScanner2d" ) {
            elem = new orcaqgui2d::LaserScanner2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }        
        else if ( guiElementInfo.type == "RangeScanner2d" ) {
            elem = new orcaqgui2d::RangeScanner2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "MultiOgMaps" ) {
            elem = new orcaqgui2d::OgMapsCombinedElement( guiElementInfo, context_, guiElementInfo.uniqueId );
        }
        else if ( guiElementInfo.type == "RangerArray" ) {
            elem = new orcaqgui2d::RangerArrayElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "Localise2d" ) {
            elem = new orcaqgui2d::Localise2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "Localise3d" ) {
            elem = new orcaqgui2d::Localise3dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "Odometry2d" ) {
            elem = new orcaqgui2d::Odometry2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "OgMap" ) {
            elem = new orcaqgui2d::OgMapElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "PolarFeature2d" ) {
            elem = new orcaqgui2d::PolarFeature2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "PathFollower2d" ) {
            elem = new orcaqgui2d::PathFollower2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), humanManager, mouseEventManager, shortcutKeyManager, guiElementSet );
        }
        else if ( guiElementInfo.type == "PathPlanner2d" ) {
            elem = new orcaqgui2d::PathPlanner2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), humanManager, mouseEventManager, shortcutKeyManager );
        }
        else if ( guiElementInfo.type == "PixMap" ) {
            elem = new orcaqgui2d::PixMapElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "FeatureMap2d" ) {
            elem = new orcaqgui2d::FeatureMap2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "Particle2d" ) {
            elem = new orcaqgui2d::Particle2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "QGraphics2d" ) {
            elem = new orcaqgui2d::QGraphics2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "Wifi" ) {
            elem = new orcaqgui2d::WifiElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), spaceBottomRight );
        }
        else if ( guiElementInfo.type == "VelocityControl2d" ) {
            elem = new orcaqgui2d::VelocityControl2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), humanManager, shortcutKeyManager, spaceBottomRight );
        }
        else
        {
            cout << "DefaultFactory::create(): Don't know how to handle guiElementInfo.type '" << guiElementInfo.type.toStdString() << "'" << endl;
        }
    
    } 
    catch (hydroqgui::Exception &e)
    {
        cout << "DefaultFactory::create(): Element creation failed: " << e.what() << endl;
        delete elem;
        elem=NULL;
    }
    return elem;
}

}


hydroqgui::IGuiElementFactory *createFactory()
{
    return new orcaqgui2d::DefaultFactory;
}
