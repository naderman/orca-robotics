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

#include <orcaqgui3dfactory/gridelement.h>
#include <orcaqgui3dfactory/laserscanner2delement.h>
#include <orcaqgui3dfactory/localise2delement.h>

#include "defaultfactory.h"

using namespace std;

namespace orcaqgui3d {

DefaultFactory::DefaultFactory()
{
    addSupportedType("Grid");
    addSupportedType("LaserScanner2d");
    addSupportedType("Localise2d");
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

hydroqguielementutil::IGuiElement*
DefaultFactory::create( const QString                            &elementType,
                        const QStringList                        &elementDetails,
                        QColor                                    suggestedColor,
                        hydroqguielementutil::IHumanManager      &humanManager,
                        hydroqguielementutil::MouseEventManager  &mouseEventManager,
                        hydroqguielementutil::ShortcutKeyManager &shortcutKeyManager,
                        const hydroqgui::GuiElementSet           &guiElementSet ) const
{
    assert( isContextSet_ );
    hydroqguielementutil::IGuiElement *elem = NULL;

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

        if ( elementType == "Grid" ) {
            elem = new orcaqgui3d::GridElement();
        }
        else if ( elementType == "LaserScanner2d" ) {
            elem = new orcaqgui3d::LaserScanner2dElement( context_, elementDetails[0].toStdString() );
        }
        else if ( elementType == "Localise2d" ) {
            elem = new orcaqgui3d::Localise2dElement( context_, elementDetails[0].toStdString(), &humanManager );
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
    return new orcaqgui3d::DefaultFactory;
}
