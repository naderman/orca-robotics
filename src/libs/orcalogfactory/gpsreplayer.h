/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_GPS_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_GPS_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/gps.h>
#include <orcalog/replayer.h>

#include <orcaice/proxy.h>

namespace orcalogfactory
{
    
class GpsReplayer : public orca::Gps, public orcalog::Replayer
{
public:
    GpsReplayer( const std::string      &format,
                 const std::string      &filename,
                 const orcaice::Context &context );
    virtual ~GpsReplayer();
    
    // remote calls:

    // Get Gps Data and config
    virtual ::orca::GpsData getData(const ::Ice::Current& ) const;    
    virtual ::orca::GpsDescription getDescription(const ::Ice::Current& ) const;

    // Subscribes and unsubscribes
    virtual void subscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Proxy<orca::GpsDescription> gpsDescriptionPipe_;
    orcaice::Proxy<orca::GpsData> gpsDataPipe_;
    
    virtual void initInterfaces();
    virtual void initDescription();

    orca::GpsConsumerPrx gpsConsumerPrx_;
    
    IceStorm::TopicPrx topicPrx_;
    
    // file protocol specifics
    void loadDataIce( int index, orca::GpsData &gpsData );
    void loadDataAscii( int index, orca::GpsData &gpsData );
    void loadHeaderIce();

    // alexm: why this?
    orca::FQTopicName getProvidedTopicLocal( const orcaice::Context & context, const std::string &tag, const std::string & subtopic);

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
