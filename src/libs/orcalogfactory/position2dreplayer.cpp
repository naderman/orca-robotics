/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "position2dreplayer.h"

using namespace std;
using namespace orcalogfactory;

int Position2dReplayer::_counter = 0;

Position2dReplayer::Position2dReplayer( const std::string      &format,
                                        const std::string      &filename,
                                        const orcaice::Context &context )
    : orcalog::Replayer("Position2d", format, filename, context),
      data_(new orca::Position2dData)
{
    // check that we support this format
    if ( format_!="ice" ) //&& format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "position2d" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

Position2dReplayer::~Position2dReplayer()
{
}

void 
Position2dReplayer::initInterfaces()
{
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

orca::Position2dDataPtr 
Position2dReplayer::getData(const Ice::Current& current) const
{
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    orca::Position2dDataPtr data;
    dataPipe_.get( data );

    return data;
}


::orca::Position2dGeometryPtr 
Position2dReplayer::getGeometry(const ::Ice::Current& ) const
{
    if ( position2dGeometryBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }
    orca::Position2dGeometryPtr geometry;
    position2dGeometryBuffer_.get( geometry );
    return geometry;
}


void 
Position2dReplayer::subscribe(const ::orca::Position2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}


void 
Position2dReplayer::unsubscribe(const ::orca::Position2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    topic_->unsubscribe( subscriber );
}


void 
Position2dReplayer::initConfigs()
{
    orca::Position2dGeometryPtr geometry = new orca::Position2dGeometry;

    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readPosition2dGeometry( helper.stream_, geometry );
    helper.read();

    position2dGeometryBuffer_.push( geometry );  
}

void 
Position2dReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    if (format_=="ice")
    {
        loadDataIce( index );
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
        
        // Let the user know that something's happening
        if ( ! (dataCounter_ % 50 ) )
        {
            ostringstream stream;
            stream << filename_ << ": sent object " << dataCounter_;
            context_.tracer()->print( stream.str() );
        }
    }
}

void 
Position2dReplayer::loadDataIce( int index )
{
    while (index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readPosition2dData( helper.stream_, data_ );
        helper.read();

        dataCounter_++;
    }
}
