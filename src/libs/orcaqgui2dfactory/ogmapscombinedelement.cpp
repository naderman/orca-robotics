/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "ogmapscombinedelement.h"
#include <iostream>
#include <QFileDialog>
#include <orcaqgui/exceptions.h>

using namespace std;
using namespace orca;

namespace orcaqgui {

OgMapsCombinedElement::OgMapsCombinedElement( const orcaice::Context  &context,
                                              const QStringList       &proxyStrList,
                                              IHumanManager* humanManager )
    : humanManager_(humanManager),
      mapFileName_("/home"),
      mapFileNameSet_(false),
      timeoutMs_(5000.0),
      isConnected_(false)
{
    cout << "TRACE(ogmapscombinedelement.cpp): Constructor" << endl;
    for (int i=0; i<proxyStrList.size(); i++)
    {
        cout << "proxyString: " << proxyStrList[i].toStdString() << endl;
        IceStormListener<OgMapData,OgMapDataPtr,OgMapPrx,OgMapConsumer,OgMapConsumerPrx>
            *listener = new IceStormListener<OgMapData,
                                             OgMapDataPtr,
                                             OgMapPrx,
                                             OgMapConsumer,
                                             OgMapConsumerPrx>(context, proxyStrList[i].toStdString());
        listeners_.push_back(listener);
    }
}

bool OgMapsCombinedElement::needToUpdate()
{
    for (uint i=0; i<listeners_.size(); i++)
    {
        if ( !listeners_[i]->buffer().isEmpty() )
        {
            // An object has arrived in one of the buffers.  We need to update.
            return true;
        }
    }

    if ( !isConnected_ )
    {
        for (uint i=0; i<listeners_.size(); i++)
        { 
            if ( listeners_[i]->connect() == 0 )
            {
                actionOnConnection();
                isConnected_ = true;
            }
            return false;
        }
    }

    for (uint i=0; i<listeners_.size(); i++)
    {
        // The buffer is empty.  How long since we last received something?
        if ( timeoutMs_ != -1 && listeners_[i]->msSinceReceipt() >= timeoutMs_ )
        {
            std::cout<<"TRACE(ogmapscombinedelement.h): Haven't received anything from " 
                    << listeners_[i]->interfaceName() << " for " << listeners_[i]->msSinceReceipt() << "ms" << std::endl;
            std::cout<<"TRACE(ogmapscombinedelement.h): Timing out..." << std::endl;
    
            painter_.clear();
            listeners_[i]->resetTimer();
            if ( listeners_[i]->connect() == 0 )
                actionOnConnection();
        }
    }
    return false;
}

void OgMapsCombinedElement::update()
{
    if ( !needToUpdate() ) {
        return;
    }

    OgMapDataPtr data;
    
    for (uint i=0; i<listeners_.size(); i++)
    {
        // get data from the buffer
        listeners_[i]->buffer().getAndPop( data );
        fuse(data);
    }

    // transfer data into painter
    cout << orcaice::toVerboseString(totalMap_);
    painter_.setData( totalMap_ );
}

void OgMapsCombinedElement::fuse( OgMapDataPtr data)
{
    totalMap_ = data;
}

void OgMapsCombinedElement::actionOnConnection()
{
    for (uint i=0; i<listeners_.size(); i++)
    {
        OgMapPrx prx;
        orcaice::connectToInterfaceWithString( context_, prx, listeners_[i]->interfaceName() );
        fuse( prx->getData() );
    }
    painter_.setData( totalMap_ );
}

QStringList
OgMapsCombinedElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Map"<<"Save OG Map"<<"Save OG Map As";
    return s;
}

void 
OgMapsCombinedElement::execute( int action )
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
        throw orcaqgui::Exception( "OgMapsCombinedElement::execute(): What the hell? bad action." );
        break;
    }
    }
}


void OgMapsCombinedElement::saveMapAs()
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

void OgMapsCombinedElement::saveMap()
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
