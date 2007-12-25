/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "pixmapelement.h"
#include <iostream>
#include <QFileDialog>
#include <hydroqgui/hydroqgui.h>

using namespace std;

namespace orcaqgui2d {

PixMapElement::PixMapElement( const orcaice::Context  &context,
                            const std::string       &proxyString,
                            hydroqgui::IHumanManager* humanManager )
    : IceStormElement<PixMapBackgroundPainter,
                      orca::PixMapData,
                      orca::PixMapPrx,
                      orca::PixMapConsumer,
                      orca::PixMapConsumerPrx>(context, proxyString, painter_, -1 ),
      humanManager_(humanManager),
      mapFileName_("/home"),
      mapFileNameSet_(false)
{
}

QStringList
PixMapElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Map"<<"Save PixMap"<<"Save PixMap As";
    return s;
}

void 
PixMapElement::execute( int action )
{
    switch ( action )
    {
    case 0 :
    {
        painter_.toggleDisplayMap();
        break;
    }
    case 1 :
    {
        saveMap();
        break;
    }
    case 2 :
    {
        saveMapAs();
        break;
    }
    default:
    {
        throw hydroqgui::Exception( ERROR_INFO, "PixMapElement::execute(): What the hell? bad action." );
        break;
    }
    }
}


void PixMapElement::saveMapAs()
{
    mapFileName_ = QFileDialog::getSaveFileName(
        0,
        //this,
        "Choose a filename to save under",
        mapFileName_,
        "*.png *.bmp *.jpg *.jpeg *.ppm *.xbm *.xpm *.bin");
    
    if ( mapFileName_ != "" )
    {
        painter_.saveMap( context_, mapFileName_, humanManager_ );
        mapFileNameSet_ = true;
    }
}

void PixMapElement::saveMap()
{
    if (!mapFileNameSet_)
    {   
        saveMapAs();
    }
    else
    {
        painter_.saveMap( context_, mapFileName_, humanManager_ );
    }
}

}
