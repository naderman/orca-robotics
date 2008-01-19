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
#include <orcaqgui/guielement.h>
#include <orcaqgui/ihumanmanager.h>

#include <orcaqgui3dfactory/gridelement.h>
#include <orcaqgui3dfactory/laserscanner2delement.h>
//#include <orcaqgui3dfactory/position3delement.h>

#include "defaultfactory.h"

using namespace std;

namespace orcaqgui3d {

DefaultFactory::DefaultFactory()
{
    addSupportedType(QStringList("::local::Grid"));
    addSupportedType(QStringList("::orca::LaserScanner2d"));
//    addSupportedType(QStringList("::orca::Position3d"));
}


orcaqgui::GuiElement*
DefaultFactory::create( const orcaice::Context  &context,
                        const QStringList       &interfaceIds,
                        const QStringList       &proxyStrList,
                        QColor                   suggestedColor,
                        orcaqgui::IHumanManager *humanManager) const
{
    orcaqgui::GuiElement *elem = NULL;
   
    if (interfaceIds.size()>1)
    {
        cout << "ERROR(guielementfactory.cpp): Factory does not support elements with more than one interface" << endl;
        return elem;
    }
    
    QString interfaceId = interfaceIds[0];
    QString proxyString = proxyStrList[0];
    
    if ( interfaceId == "::local::Grid" ) {
        cout<<"creating Grid element"<<endl;
        elem = new orcaqgui3d::GridElement();
    }
    else if ( interfaceId == "::orca::LaserScanner2d" ) {
        cout<<"creating LaserScanner2d element"<<endl;
        elem = new orcaqgui3d::LaserScanner2dElement( context, proxyString.toStdString() );
    }
//     else if ( interfaceId == "::orca::Position3d" ) {
//         cout<<"creating Position3d element"<<endl;
//         elem = new orcaqgui3d::Position3dElement( context, proxyString.toStdString() );
//     }
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
