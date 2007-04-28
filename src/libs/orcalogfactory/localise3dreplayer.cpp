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

#include "localise3dreplayer.h"

using namespace std;
using namespace orcalogfactory;

int Localise3dReplayer::_counter = 0;

Localise3dReplayer::Localise3dReplayer( const std::string      &format,
                            const std::string      &filename,
                            const orcaice::Context &context )
    : orcalog::Replayer("Localise3d", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "localise3d" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

Localise3dReplayer::~Localise3dReplayer()
{
}

void 
Localise3dReplayer::initInterfaces()
{
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

void 
Localise3dReplayer::initDescription()
{
// Localise3d does not have a description
//     orca::Localise3dDescription localDescription;
//     
//     orcalog::IceReadHelper helper( context_.communicator(), file_ );
//     ice_readLocalise3dDescription( helper.stream_, localDescription );
//     helper.read();
// 
//     descriptionBuffer_.push( localDescription );
}

// orca::Localise3dDescription
// Localise3dReplayer::getDescription(const Ice::Current& c) const
// {    
// Localise3d does not have a description
//     if ( descriptionBuffer_.isEmpty() )
//     {
//         throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
//     }
//  
//     orca::Localise3dDescription descr;
//     descriptionBuffer_.get( descr );
//     return descr;
// }
 

orca::Localise3dData
Localise3dReplayer::getData(const Ice::Current& current) const
{
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    orca::Localise3dData data;
    dataPipe_.get( data );

    return data;
}


::orca::Localise3dData
Localise3dReplayer::getDataAtTime(const orca::Time&, const Ice::Current& current) const
{
    context_.tracer()->warning( "Localise3dReplayer::getDataAtTime(): Not implemented yet. Just returning latest data!" );
    return getData( current );
}

void 
Localise3dReplayer::subscribe(const ::orca::Localise3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    topic_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
}

void 
Localise3dReplayer::unsubscribe(const ::orca::Localise3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    topic_->unsubscribe( subscriber );
}

void 
Localise3dReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    while ( !file_->eof() && index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readLocalise3dData( helper.stream_, data_ );
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
