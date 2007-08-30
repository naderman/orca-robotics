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
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

void 
Localise3dReplayer::initDescription()
{
    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readVehicleGeometryDescription( helper.stream_, geometry_ );
    helper.read();
}
 

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

orca::VehicleGeometryDescriptionPtr
Localise3dReplayer::getVehicleGeometry( const ::Ice::Current& ) const
{
    return geometry_;
}

void 
Localise3dReplayer::subscribe(const ::orca::Localise3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise3dReplayer::unsubscribe(const ::orca::Localise3dConsumerPrx &subscriber, const ::Ice::Current&)
{
    topicPrx_->unsubscribe( subscriber );
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
    dataPipe_.set( data_ );

    if ( !isTest ) 
    {
        // push to IceStorm
        publisher_->setData( data_ );
    }
}
