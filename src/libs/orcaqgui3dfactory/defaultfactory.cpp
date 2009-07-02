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

#include <orcaqgui3dfactory/gridelement.h>
#include <orcaqgui3dfactory/laserscanner2delement.h>
#include <orcaqgui3dfactory/rangescanner2delement.h>
#include <orcaqgui3dfactory/localise2delement.h>
#include <orcaqgui3dfactory/localise3delement.h>
#include <orcaqgui3dfactory/featuremap2delement.h>
#include <orcaqgui3dfactory/ogmapelement.h>
#include <orcaqgui3dfactory/pointcloudelement.h>

#include "defaultfactory.h"

using namespace std;

namespace orcaqgui3d {

DefaultFactory::DefaultFactory()
{
    addSupportedType("Grid");
    addSupportedType("LaserScanner2d");
    addSupportedType("RangeScanner2d");
    addSupportedType("Localise2d");
    addSupportedType("Localise3d");
    addSupportedType("FeatureMap2d");
    addSupportedType("OgMap");
    addSupportedType("PointCloud");
}

bool 
DefaultFactory::lookupElementType( const QStringList &ids, QString &elementType ) const
{
    // One-to-one mappings (one interface per element):
    if (ids.size()==1)
    {
        elementType = ids[0].section(':',4,4);
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

        if ( guiElementInfo.type == "Grid" ) {
            elem = new orcaqgui3d::GridElement( guiElementInfo );
        }
        else if ( guiElementInfo.type == "LaserScanner2d" ) {
            elem = new orcaqgui3d::LaserScanner2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "RangeScanner2d" ) {
            elem = new orcaqgui3d::RangeScanner2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
        }
        else if ( guiElementInfo.type == "Localise2d" ) {
            elem = new orcaqgui3d::Localise2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "FeatureMap2d" ) {
            elem = new orcaqgui3d::FeatureMap2dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "Localise3d" ) {
            elem = new orcaqgui3d::Localise3dElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "OgMap" ) {
            elem = new orcaqgui3d::OgMapElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString(), &humanManager );
        }
        else if ( guiElementInfo.type == "PointCloud" ) {
            elem = new orcaqgui3d::PointCloudElement( guiElementInfo, context_, guiElementInfo.uniqueId.toStdString() );
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
    return new orcaqgui3d::DefaultFactory;
}
