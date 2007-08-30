/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "drivebicyclereplayer.h"

using namespace std;
using namespace orcalogfactory;

int DriveBicycleReplayer::_counter = 0;

DriveBicycleReplayer::DriveBicycleReplayer( const std::string      &format,
                            const std::string      &filename,
                            const orcaice::Context &context )
    : orcalog::Replayer("DriveBicycle", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "drivebicycle" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

DriveBicycleReplayer::~DriveBicycleReplayer()
{
}

void 
DriveBicycleReplayer::initInterfaces()
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

void 
DriveBicycleReplayer::initDescription()
{
    orca::VehicleDescription localDescription;
    
    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readVehicleDescription( helper.stream_, localDescription );
    helper.read();

    descriptionPipe.set( localDescription );
}

orca::VehicleDescription
DriveBicycleReplayer::getDescription(const Ice::Current& c) const
{    
    if ( descriptionPipe.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }
 
    orca::VehicleDescription descr;
    descriptionPipe.get( descr );
    return descr;
}
 
void
DriveBicycleReplayer::setCommand(const ::orca::DriveBicycleCommand& command,  const Ice::Current& current)
{
    if ( this->hasNotifyHandler() ) {
        this->set( command );
    }
}


orca::DriveBicycleData
DriveBicycleReplayer::getData(const Ice::Current& current) const
{
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    orca::DriveBicycleData data;
    dataPipe_.get( data );

    return data;
}

void 
DriveBicycleReplayer::subscribe(const ::orca::DriveBicycleConsumerPrx &subscriber, const ::Ice::Current&)
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
DriveBicycleReplayer::unsubscribe(const ::orca::DriveBicycleConsumerPrx &subscriber, const ::Ice::Current&)
{
    topicPrx_->unsubscribe( subscriber );
}

void 
DriveBicycleReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    while ( !file_->eof() && index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readDriveBicycleData( helper.stream_, data_ );
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
