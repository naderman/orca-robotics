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

#include "laserscanner2dreplayer.h"

using namespace std;
using namespace orcalogfactory;

int LaserScanner2dReplayer::_counter = 0;

LaserScanner2dReplayer::LaserScanner2dReplayer( 
                              const std::string      &format,
                              const std::string      &filename,
                              const orcaice::Context &context )
    : orcalog::Replayer("laserScanner2d", format, filename, context),
      data_(new orca::LaserScanner2dData)
{
    // check that we support this format
    if ( format_!="ice" ) //&& format_!="ascii" )
    {
        context_.tracer()->warning( interfaceType_+"Logger: unknown log format: "+format_ );
        throw orcalog::FormatNotSupportedException( ERROR_INFO, interfaceType_+"Logger: unknown log format: "+format_ );
    }

    stringstream ss;
    ss << "laserscanner2d" << _counter;
    interfaceName_ = ss.str();
    _counter++;
}

LaserScanner2dReplayer::~LaserScanner2dReplayer()
{
}

// served out the data to the client (it was stored here by the driver at the last read)
orca::RangeScanner2dDataPtr 
LaserScanner2dReplayer::getData(const Ice::Current& current) const
{
//    std::cout << "Sending data back" << std::endl;
    // we don't need to pop the data here because we don't block on it.
    if ( dataPipe_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }

    // create a null pointer. data will be cloned into it.
    orca::LaserScanner2dDataPtr data;
    dataPipe_.get( data );

    return data;
}

orca::RangeScanner2dDescriptionPtr 
LaserScanner2dReplayer::getDescription(const Ice::Current& current) const
{    
//     std::cout << "Sending config back" << std::endl;
    if ( laserDescriptionBuffer_.isEmpty() )
    {
        throw orca::DataNotExistException( "logplayer buffer is empty, probably because we are not replaying yet" );
    }
    orca::RangeScanner2dDescriptionPtr obj;
    laserDescriptionBuffer_.get( obj );
    return obj;
}

void 
LaserScanner2dReplayer::subscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
//    cout<<"INFO(laserreplayer.cpp): subscription request"<<endl;
    IceStorm::QoS qos;
    topic_->subscribe( qos, subscriber );
}

void 
LaserScanner2dReplayer::unsubscribe(const ::orca::RangeScanner2dConsumerPrx &subscriber, const ::Ice::Current&)
{
//    cout<<"INFO(laserreplayer.cpp): unsubscription request"<<endl;
    topic_->unsubscribe( subscriber );
}

void 
LaserScanner2dReplayer::initInterfaces()
{
    // cout << "INFO(laserreplayer.cpp): createInterface: " << interfaceName_ << endl;    
    topic_ = orcaice::connectToTopicWithString( context_, publisher_, interfaceName_ );
    
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithString( context_, obj, interfaceName_ );
}

void 
LaserScanner2dReplayer::initConfigs()
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
LaserScanner2dReplayer::replayData( int index, bool isTest )
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
            ostringstream ss;
            ss << filename_ << ": sent object " << dataCounter_;
            context_.tracer()->info( ss.str() );
        } 
    }
}

void 
LaserScanner2dReplayer::loadHeaderIce()
{
    orca::RangeScanner2dDescriptionPtr obj = new orca::RangeScanner2dDescription;

    orcalog::IceReadHelper helper( context_.communicator(), file_ );
    ice_readRangeScanner2dDescription( helper.stream_, obj );
    helper.read();

    laserDescriptionBuffer_.push( obj );  
}

void 
LaserScanner2dReplayer::loadDataIce( int index )
{
    while (index != dataCounter_ )
    {
        orcalog::IceReadHelper helper( context_.communicator(), file_ );
        ice_readLaserScanner2dData( helper.stream_, data_ );
        helper.read();

        dataCounter_++;
    }
}
