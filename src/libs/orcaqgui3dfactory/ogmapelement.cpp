/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "ogmapelement.h"
#include <iostream>
#include <QFileDialog>
#include <hydroqgui/hydroqgui.h>

using namespace std;

namespace orcaqgui3d {

OgMapElement::OgMapElement( const orcaice::Context  &context,
                            const std::string       &proxyString,
                            hydroqguielementutil::IHumanManager* humanManager )
    : orcaqgui3d::IceStormElement3d<OgMapPainter,
                      orca::OgMapData,
                      orca::OgMapPrx,
                      orca::OgMapConsumer,
                      orca::OgMapConsumerPrx>(context, proxyString, painter_, -1 ),
      humanManager_(*humanManager),
      mapFileName_("/home"),
      mapFileNameSet_(false)
{
}

QStringList
OgMapElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Map";
    return s;
}

void 
OgMapElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
    {
        painter_.toggleDisplayMap();
        break;
    }
    default:
    {
        throw hydroqgui::Exception( ERROR_INFO, "OgMapElement::execute(): What the hell? bad action." );
        break;
    }
    }
}

}
