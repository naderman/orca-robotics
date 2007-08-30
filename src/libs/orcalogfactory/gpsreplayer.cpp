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

#include "gpsreplayer.h"

using namespace std;
using namespace orcalogfactory;

int GpsReplayer::_counter = 0;

GpsReplayer::GpsReplayer( const std::string      &format,
                          const std::string      &filename,
                          const orcaice::Context &context )
    : orcalog::Replayer("Gps", format, filename, context)
{
    // check that we support this format
    if ( format_!="ice" && format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "gps" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

GpsReplayer::~GpsReplayer()
{
}

orca::FQTopicName
GpsReplayer::getProvidedTopicLocal( const orcaice::Context & context, const std::string &tag, const std::string & subtopic)
{
    orca::FQTopicName fqTName;
    fqTName.platform = context.name().platform;
    fqTName.component = context.name().component;
    fqTName.iface = tag;
    fqTName.topic = subtopic;

    initTracerInfo( context, "will publish to topic '"+orcaice::toString(fqTName)+"'",2 );
    return fqTName;
}

void 
GpsReplayer::initInterfaces()
{
    topicPrx_ = orcaice::connectToTopicWithString( context_, gpsConsumerPrx_, interfaceName_ );
    
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

orca::GpsDescription
GpsReplayer::getDescription(const ::Ice::Current& ) const
{    
    cout << "INFO(gpsreplayer.cpp): getDescription " << endl;
    if ( gpsDescriptionPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }
 
    orca::GpsDescription descr;
    gpsDescriptionPipe_.get( descr );
    return descr;
}

orca::GpsData
GpsReplayer::getData(const Ice::Current& current) const
{
    
    cout << "INFO(gpsreplayer.cpp): getData " << endl;
    // we don't need to pop the data here because we don't block on it.
    if ( gpsDataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    // create a null pointer. data will be cloned into it.
    orca::GpsData data;
    gpsDataPipe_.get( data );

    return data;
}

void 
GpsReplayer::subscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
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
GpsReplayer::unsubscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout<<"INFO(gpsreplayer.cpp): unsubscribe"<<endl;
    topicPrx_->unsubscribe( subscriber );
}

void 
GpsReplayer::initDescription()
{
    if (format_=="ice")
    {
        loadHeaderIce();
    }
    else if ( format_ == "ascii" )
    {
        orca::GpsDescription descr;
        std::string line;
        std::getline( *file_, line );
        std::stringstream ss( line );
        orcalog::fromLogString( ss, descr );
        cout<<"TRACE(gpsreplayer.cpp): read description: " << orcaice::toString(descr) << endl;
        gpsDescriptionPipe_.set( descr );
    }
    else
    {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "GpsReplayer::initDescription(): Unknown format: "+format_ );
    }

//     cout << "GpsDescription: " << orcaice::toString( descr_ ) << endl;
}

void 
GpsReplayer::replayData( int index, bool isTest )
{
    checkIndex( index );
    
    orca::GpsData gpsData;
    if (format_=="ice")
    {
        loadDataIce( index, gpsData );
    }
    else if (format_=="ascii")
    {
        loadDataAscii( index, gpsData );
    }
    else
    {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "GpsReplayer::replayData(): Unknown format: "+format_ );
    }
    
    // push to buffer for direct remote access
    gpsDataPipe_.set( gpsData );

    if ( !isTest ) 
    {
        // push to IceStorm
        gpsConsumerPrx_->setData( gpsData );
    }
}

void 
GpsReplayer::loadHeaderIce()
{
    orca::GpsDescription description;
    
    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readGpsDescription( helper.stream_, description );
    helper.read();
    
    // if there are configuration parameters in the logplayer config file,
    // they'll override the logged ones
    string prefix = context_.tag() + ".Config.Gps.";

    orca::Frame2d offset;
    orca::Frame3d antennaOffset;
    int ret;

    ret = orcaice::getPropertyAsFrame2d( context_.properties(), prefix+"Offset", offset );
    if (ret==0) description.offset = offset;
    
    ret = orcaice::getPropertyAsFrame3d( context_.properties(), prefix+"AntennaOffset", antennaOffset );
    if (ret==0) description.antennaOffset = antennaOffset;
    
    cout << "INFO(gpsreplayer.cpp): GpsDescription: " << orcaice::toString( description ) << endl;
    
    gpsDescriptionPipe_.set( description );
}

void
GpsReplayer::loadDataIce( int index, orca::GpsData &gpsData )
{
    while (index != (dataCounter_) )
    {        
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readGpsData( helper.stream_, gpsData );
        helper.read();

        dataCounter_++;
    }
}

void 
GpsReplayer::loadDataAscii( int index, orca::GpsData &gpsData )
{
    while (index != (dataCounter_) )
    {
        std::string line;
        std::getline( *file_, line );
        std::stringstream ss( line );

        orcalog::fromLogString( ss, gpsData );

        dataCounter_++;
    }
}
