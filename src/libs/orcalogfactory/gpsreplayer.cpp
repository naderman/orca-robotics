/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    : orcalog::Replayer("Gps", format, filename, context),
      descrPtr_(new orca::GpsDescription),
      gpsDataPtr_(new orca::GpsData),
      gpsTimeDataPtr_(new orca::GpsTimeData),
      gpsMapGridDataPtr_(new orca::GpsMapGridData)
{
    checkDescriptionFile();

    // check that we support this format
    if ( format_!="ice" ) //&& format_!="ascii" )
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

    initTracerPrint( context, "will publish to topic '"+orcaice::toString(fqTName)+"'",2 );
    return fqTName;
}

void
GpsReplayer::checkDescriptionFile()
{
    string prefix = context_.tag() + ".Config.Gps.";
   
    // initialize 
    orcaice::setInit( descrPtr_->size );
    orcaice::setInit( descrPtr_->offset );

    retSize_ = orcaice::getPropertyAsSize3d( context_.properties(), prefix+"Size", descrPtr_->size );
    retOffset_ = orcaice::getPropertyAsFrame3d( context_.properties(), prefix+"Offset", descrPtr_->offset );
}

void 
GpsReplayer::initInterfaces()
{
//     cout << "INFO(gpsreplayer.cpp): createInterface" << endl;

    topic_ = orcaice::connectToTopicWithString( context_, gpsConsumerPrx_, interfaceName_ );
    
    string topicNameTime = orcaice::toString( 
            getProvidedTopicLocal( context_, interfaceName_, "time" ) );
    cout << "INFO(gpsreplayer.cpp): topicNameTime: " << topicNameTime << endl;
    topicTimePrx_ = orcaice::connectToTopicWithString( context_, gpsTimeConsumerPrx_, topicNameTime );
    
    string topicNameGrid = orcaice::toString( 
            getProvidedTopicLocal( context_, interfaceName_, "mapgrid" ) );
    cout << "INFO(gpsreplayer.cpp): topicNameGrid: " << topicNameGrid << endl;
    topicMapGridPrx_ = orcaice::connectToTopicWithString( context_, gpsMapGridConsumerPrx_, topicNameGrid );   
      

    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

orca::GpsDescriptionPtr
GpsReplayer::getDescription(const ::Ice::Current& ) const
{    
//     cout << "INFO(gpsreplayer.cpp): getConfig " << endl;
    if ( gpsDescriptionBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }
 
    orca::GpsDescriptionPtr descr;
    gpsDescriptionBuffer_.get( descr );
    return descr;
}

orca::GpsDataPtr 
GpsReplayer::getData(const Ice::Current& current) const
{
    // we don't need to pop the data here because we don't block on it.
    if ( gpsDataBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    // create a null pointer. data will be cloned into it.
    orca::GpsDataPtr data;
    gpsDataBuffer_.get( data );

    return data;
}

orca::GpsTimeDataPtr
GpsReplayer::getTimeData(const ::Ice::Current& current ) const
{
    // we don't need to pop the data here because we don't block on it.
    if ( gpsTimeDataBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    // create a null pointer. data will be cloned into it.
    orca::GpsTimeDataPtr data;
    gpsTimeDataBuffer_.get( data );

    return data;
}

orca::GpsMapGridDataPtr
GpsReplayer::getMapGridData(const ::Ice::Current& current ) const
{
    // we don't need to pop the data here because we don't block on it.
    if ( gpsMapGridDataBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    // create a null pointer. data will be cloned into it.
    orca::GpsMapGridDataPtr data;
    gpsMapGridDataBuffer_.get( data );

    return data;    
}

void 
GpsReplayer::subscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
{
   cout<<"INFO(gpsreplayer.cpp): subscribe"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}

void 
GpsReplayer::unsubscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout<<"INFO(gpsreplayer.cpp): unsubscribe"<<endl;
    topic_->unsubscribe( subscriber );
}

void 
GpsReplayer::subscribeForTime(const ::orca::GpsTimeConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "INFO(gpsreplayer.cpp): subscribeForTime()" << endl;
    IceStorm::QoS qos;
    topicTimePrx_->subscribe( qos, subscriber );
}

void 
GpsReplayer::unsubscribeForTime(const ::orca::GpsTimeConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "INFO(gpsreplayer.cpp): unsubscribeForTime()" << endl;
    topicTimePrx_->unsubscribe( subscriber );
}

void 
GpsReplayer::subscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "INFO(gpsreplayer.cpp): subscribeForMapGrid()" << endl;
    IceStorm::QoS qos;
    topicMapGridPrx_->subscribe( qos, subscriber );
}

void 
GpsReplayer::unsubscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "INFO(gpsreplayer.cpp): unsubscribeForMapGrid()" << endl;
    topicMapGridPrx_->unsubscribe( subscriber );
}

void 
GpsReplayer::initDescriptions()
{
    if (format_=="ice")
    {
        loadHeaderIce();
    }
    else
    {
        throw orcalog::FormatNotSupportedException( ERROR_INFO, "Unknown format: "+format_ );
    }

    cout << "GpsDescription: " << orcaice::toString( descrPtr_ ) << endl;
}

void 
GpsReplayer::replayData( int index, bool isTest )
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
    if ( id_ == 0 )
    {
        gpsDataBuffer_.push( gpsDataPtr_ );
    }
    else if ( id_ == 1 )
    {
        gpsTimeDataBuffer_.push( gpsTimeDataPtr_ );
    }
    else if ( id_ == 2 )
    {
        gpsMapGridDataBuffer_.push( gpsMapGridDataPtr_ );
    }

    if ( !isTest ) 
    {
        // push to IceStorm
        if ( id_ == 0 )
        {
            gpsConsumerPrx_->setData( gpsDataPtr_ );
        }
        else if ( id_ == 1 )
        {
            gpsTimeConsumerPrx_->setData( gpsTimeDataPtr_ );
        }
        else if ( id_ == 2 )
        {
            gpsMapGridConsumerPrx_->setData( gpsMapGridDataPtr_ );
        }
    }
}

void 
GpsReplayer::loadHeaderIce()
{
    orca::GpsDescriptionPtr localDescriptionPtr = new orca::GpsDescription;

    std::vector<Ice::Byte> byteData;
    size_t length;
    file_->read( (char*)&length, sizeof(length) );
    checkFile();
    byteData.resize( length );
    file_->read( (char*)&byteData[0], length );
    checkFile();
       
    Ice::InputStreamPtr iceInputStreamPtr = Ice::createInputStream( context_.communicator(), byteData );
    
    if ( !byteData.empty() )
    {
        ice_readGpsDescription( iceInputStreamPtr, localDescriptionPtr );
        iceInputStreamPtr -> readPendingObjects();
        ostringstream stream;
        stream << "Result of readGpsDescription: " << orcaice::toString(localDescriptionPtr);
        context_.tracer()->print( stream.str() );
    }

    // if there was no geometry specified in the cfg file, take logged geometry
    if (retSize_!=0)
    {
        descrPtr_->size= localDescriptionPtr->size;
    }

    // if there was no origin specified in the cfg file, take logged origin
    if (retOffset_!=0)
    {
        descrPtr_->offset= localDescriptionPtr->offset;
    }

    gpsDescriptionBuffer_.push( descrPtr_ );
}

void 
GpsReplayer::loadDataIce( int index )
{
    while (index != (dataCounter_) )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        id_ = helper.id();
        if ( id_ == 0 ) {
            ice_readGpsData( helper.stream_, gpsDataPtr_ );
        }
        else if ( id_ == 1 ) {
            ice_readGpsTimeData( helper.stream_, gpsTimeDataPtr_ );
        }
        else if ( id_ == 2 ) {
            ice_readGpsMapGridData( helper.stream_, gpsMapGridDataPtr_ );
        }
        else {
            std::stringstream ss;
            ss << "GpsReplayer: unrecognized object type '" << helper.id() << "'";
            throw orcalog::Exception( ERROR_INFO, ss.str() );
        }
        helper.read();

        dataCounter_++;
    }
}
