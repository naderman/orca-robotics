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
#include <orcaqgui/guielement.h>
#include <orcaqgui/ihumanmanager.h>

#include <orcaqgui3dfactory/gridelement.h>
#include <orcaqgui3dfactory/laserscanner2delement.h>
#include <orcaqgui3dfactory/position3delement.h>

#include "defaultfactory.h"

using namespace std;

namespace orcaqgui3d {

DefaultFactory::DefaultFactory()
{
    addSupportedType("::local::Grid");
    addSupportedType("::orca::LaserScanner2d");
    addSupportedType("::orca::Position3d");
}


orcaqgui::GuiElement*
DefaultFactory::create( const orcaice::Context           &context,
                        const QString                    &interfaceId,
                        const QStringList                &proxyStrList,
                        QColor                            suggestedColor,
                        orcaqgui::IHumanManager           *humanManager) const
{
    orcaqgui::GuiElement *elem = NULL;
   
    if ( interfaceId == "::local::Grid" ) {
        cout<<"creating Grid element"<<endl;
        elem = new orcaqgui3d::GridElement();
    }
    else if ( interfaceId == "::orca::LaserScanner2d" ) {
        cout<<"creating LaserScanner2d element"<<endl;
        elem = new orcaqgui3d::LaserScanner2dElement( context, proxyStrList[0].toStdString() );
    }
    else if ( interfaceId == "::orca::Position3d" ) {
        cout<<"creating Position3d element"<<endl;
        elem = new orcaqgui3d::Position3dElement( context, proxyStrList[0].toStdString() );
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
    return new orcaqgui3d::DefaultFactory;
}
