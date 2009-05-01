/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <QFileDialog>
#include <orcaogmap/orcaogmap.h>
#include "ogmapscombinedelement.h"
#include <hydroqgui/exceptions.h>

using namespace std;

namespace orcaqgui2d {

OgMapsCombinedElement::OgMapsCombinedElement( const orcaice::Context  &context,
                                              const QStringList       &proxyStrList)
    : context_(context),
      isConnected_(false)
{
    
    assert(proxyStrList.size()==2 && "Only 2 OgMaps supported at this point");
    
    cout << "TRACE(ogmapscombinedelement.cpp): Constructor" << endl;
    for (int i=0; i<proxyStrList.size(); i++)
    {
        cout << "proxyString: " << proxyStrList[i].toStdString() << endl;
        orcaqguielementutil::IceStormListener<orca::OgMapData,orca::OgMapPrx,orca::OgMapConsumer,orca::OgMapConsumerPrx>
            *listener = new orcaqguielementutil::IceStormListener<orca::OgMapData,
            orca::OgMapPrx,
            orca::OgMapConsumer,
            orca::OgMapConsumerPrx>(context, proxyStrList[i].toStdString());
        if ( listener->connect() != 0 ) 
        {
            throw hydroqgui::Exception(ERROR_INFO,"Problem connecting to interface with proxyString " + proxyStrList[i].toStdString());
        }
        
        listeners_.push_back(listener);
    }
    
    painter_ = new OgMapsCombinedPainter( context );
}

bool OgMapsCombinedElement::needToUpdate()
{
    for (unsigned int i=0; i<listeners_.size(); i++)
    {
        if ( !listeners_[i]->buffer().isEmpty() )
        {
            // An object has arrived in one of the buffers.  We need to update.
            return true;
        }
    }

    if ( !isConnected_ )
    {
        for (unsigned int i=0; i<listeners_.size(); i++)
        { 
            if ( listeners_[i]->connect() == 0 )
            {
                //actionOnConnection();
                isConnected_ = true;
            }
            else
            {
                return false;
            }
        }
    }
    
    return false;
}

void OgMapsCombinedElement::update()
{
    if ( !needToUpdate() ) {
        return;
    }
    
    orca::OgMapData data0;
    listeners_[0]->buffer().getAndPop( data0 );
    orca::OgMapData data1;
    listeners_[1]->buffer().getAndPop( data1 );
    
//     // for more than two ogMaps:
//     OgMapData data;
//     for (unsigned int i=0; i<listeners_.size(); i++)
//     {
//         // get data from the buffer
//         listeners_[i]->buffer().getAndPop( data );
//     }
    
    // transfer data into painter
    painter_->setData( data0, data1 );
}

QStringList
OgMapsCombinedElement::contextMenu()
{
    QStringList s;
    s<<"Toggle Map";
    return s;
}

void 
OgMapsCombinedElement::execute( int action )
{
    switch ( action )
    {
        case 0 :
        {
            painter_->toggleDisplayMap();
            break;
        }
        default:
        {
            throw hydroqgui::Exception( ERROR_INFO, "OgMapsCombinedElement::execute(): What the hell? bad action." );
            break;
        }
    }
}

}
