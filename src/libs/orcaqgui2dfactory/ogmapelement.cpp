#include "ogmapelement.h"
#include <iostream>
#include <QFileDialog>
#include <orcaqgui/exceptions.h>

using namespace std;

namespace orcaqgui {

OgMapElement::OgMapElement( const orcaice::Context  &context,
                            const std::string       &proxyString,
                            IHumanManager* humanManager )
    : IceStormElement<OgMapPainter,
                      orca::OgMapData,
                      orca::OgMapDataPtr,
                      orca::OgMapPrx,
                      orca::OgMapConsumer,
                      orca::OgMapConsumerPrx>(context, proxyString, painter_, -1 ),
      humanManager_(humanManager),
      mapFileName_("/home"),
      mapFileNameSet_(false)
{
}

QStringList
OgMapElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Map"<<"Save OG Map"<<"Save OG Map As";
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
        throw orcaqgui::Exception( "OgMapElement::execute(): What the hell? bad action." );
        break;
    }
    }
}


void OgMapElement::saveMapAs()
{
    mapFileName_ = QFileDialog::getSaveFileName(
        0,
        //this,
        "Choose a filename to save under",
        mapFileName_,
        "*.txt");
    
    if ( mapFileName_ != "" )
    {
        painter_.saveMap( context_, mapFileName_, humanManager_ );
        mapFileNameSet_ = true;
    }
}

void OgMapElement::saveMap()
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
