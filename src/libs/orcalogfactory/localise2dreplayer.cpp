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

#include "localise2dreplayer.h"

using namespace std;
using namespace orcalogfactory;

int Localise2dReplayer::_counter = 0;

Localise2dReplayer::Localise2dReplayer( const std::string      &format,
                            const std::string      &filename,
                            const orcaice::Context &context )
    : orcalog::Replayer("Localise2d", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "localise2d" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

Localise2dReplayer::~Localise2dReplayer()
{
}

void 
Localise2dReplayer::initInterfaces()
{
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

orca::Localise2dData
Localise2dReplayer::getData(const Ice::Current& current) const
{
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    orca::Localise2dData data;
    dataPipe_.get( data );

    return data;
}


::orca::Localise2dData
Localise2dReplayer::getDataAtTime(const orca::Time&, const Ice::Current& current) const
{
    context_.tracer()->warning( "Localise2dReplayer::getDataAtTime(): Not implemented yet. Just returning latest data!" );
    return getData( current );
}

void 
Localise2dReplayer::subscribe(const ::orca::Localise2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    topic_->subscribe( qos, subscriber );
}

void 
Localise2dReplayer::unsubscribe(const ::orca::Localise2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    topic_->unsubscribe( subscriber );
}

void 
Localise2dReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    while ( !file_->eof() && index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readLocalise2dData( helper.stream_, data_ );
        helper.read();

        dataCounter_++;
    }

    // push to buffer for direct remote access
    dataPipe_.push( data_ );

    if ( !isTest ) 
    {
        // push to IceStorm
        publisher_->setData( data_ );
    }
}
