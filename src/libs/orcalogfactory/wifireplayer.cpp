/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "wifireplayer.h"

using namespace std;
using namespace orcalogfactory;

int WifiReplayer::_counter = 0;

WifiReplayer::WifiReplayer( const std::string      &format,
                            const std::string      &filename,
                            const orcaice::Context &context )
    : orcalog::Replayer("Wifi", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "wifi" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

WifiReplayer::~WifiReplayer()
{
}

void 
WifiReplayer::initInterfaces()
{
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

orca::WifiData
WifiReplayer::getData(const Ice::Current& current) const
{
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    orca::WifiData data;
    dataPipe_.get( data );

    return data;
}

void 
WifiReplayer::subscribe(const ::orca::WifiConsumerPrx &subscriber, const ::Ice::Current&)
{
    topic_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
}

void 
WifiReplayer::unsubscribe(const ::orca::WifiConsumerPrx &subscriber, const ::Ice::Current&)
{
    topic_->unsubscribe( subscriber );
}

void 
WifiReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    if (format_=="ice")
    {
        while ( !file_->eof() && index != (dataCounter_) )
        {
            orcalog::IceReadHelper helper( context_.communicator(), file_ );
            ice_readWifiData( helper.stream_, data_ );
            helper.read();
    
            dataCounter_++;
        }
    }
    else if (format_=="ascii")
    {   
        context_.tracer()->warning( "WifiReplayer: Ascii replaying not implemented" );
    }
    else
    {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Unknown format: "+format_ );
    }

    // push to buffer for direct remote access
    dataPipe_.push( data_ );

    if ( !isTest ) 
    {
        // push to IceStorm
        publisher_->setData( data_ );
    }
}
