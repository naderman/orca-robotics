/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "odometry2dreplayer.h"

using namespace std;
using namespace orcalogfactory;

int Odometry2dReplayer::_counter = 0;

Odometry2dReplayer::Odometry2dReplayer( const std::string      &format,
                                        const std::string      &filename,
                                        const orcaice::Context &context )
    : orcalog::Replayer("Odometry2d", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" ) //&& format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "odometry2d" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

Odometry2dReplayer::~Odometry2dReplayer()
{
}

void 
Odometry2dReplayer::initInterfaces()
{
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

orca::Odometry2dData
Odometry2dReplayer::getData(const Ice::Current& current) const
{
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    orca::Odometry2dData data;
    dataPipe_.get( data );

    return data;
}


::orca::Odometry2dDescription
Odometry2dReplayer::getDescription(const ::Ice::Current& ) const
{
    return descr_;
}


void 
Odometry2dReplayer::subscribe(const ::orca::Odometry2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}


void 
Odometry2dReplayer::unsubscribe(const ::orca::Odometry2dConsumerPrx &subscriber, const ::Ice::Current&)
{
    topic_->unsubscribe( subscriber );
}


void 
Odometry2dReplayer::initConfigs()
{
    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readOdometry2dDescription( helper.stream_, descr_ );
    helper.read();
}

void 
Odometry2dReplayer::replayData( int index, bool isTest )
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
    }
}

void 
Odometry2dReplayer::loadDataIce( int index )
{
    while (index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readOdometry2dData( helper.stream_, data_ );
        helper.read();

        dataCounter_++;
    }
}
