/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "position3dreplayer.h"

using namespace std;
using namespace orcalogfactory;

int Position3dReplayer::_counter = 0;

Position3dReplayer::Position3dReplayer( const std::string      &format,
                              const std::string      &filename,
                              const orcaice::Context &context )
    : orcalog::Replayer("Position3d", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" ) //&& format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "position3d" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

Position3dReplayer::~Position3dReplayer()
{
}

void 
Position3dReplayer::initInterfaces()
{
//  cout << "INFO(position3dreplayer.cpp): createInterface" << endl;    
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );
    
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

// served out the data to the client (it was stored here by the driver at the last read)
orca::Position3dData
Position3dReplayer::getData(const Ice::Current& current) const
{
//    std::cout << "Sending data back" << std::endl;

    // we don't need to pop the data here because we don't block on it.
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    // create a null pointer. data will be cloned into it.
    orca::Position3dData data;
    dataPipe_.get( data );

    return data;
}

::orca::Position3dDescription
Position3dReplayer::getDescription(const ::Ice::Current& ) const
{
//    std::cout << "Sending description back" << std::endl;

    if ( position3dDescriptionBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }
    orca::Position3dDescription descr;
    position3dDescriptionBuffer_.get( descr );
    return descr;
}

void 
Position3dReplayer::subscribe(const ::orca::Position3dConsumerPrx &subscriber, const ::Ice::Current&)
{
//    cout<<"INFO(position3dreplayer.cpp): subscription request"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}

void 
Position3dReplayer::unsubscribe(const ::orca::Position3dConsumerPrx &subscriber, const ::Ice::Current&)
{
//    cout<<"INFO(position3dreplayer.cpp): unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}

void 
Position3dReplayer::replayData( int index, bool isTest )
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
Position3dReplayer::initDescription()
{
    if (format_=="ice")
    {
        loadHeaderIce();
    }
    else
    {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Unknown format: "+format_ );
    }
}

void 
Position3dReplayer::loadHeaderIce()
{
    orca::Position3dDescription descr;

    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readPosition3dDescription( helper.stream_, descr );
    helper.read();

    position3dDescriptionBuffer_.push( descr );
}

void 
Position3dReplayer::loadDataIce( int index )
{
    while (index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readPosition3dData( helper.stream_, data_ );
        helper.read();

        dataCounter_++;
    }
}
