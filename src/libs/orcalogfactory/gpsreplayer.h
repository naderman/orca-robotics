/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

#include <orcaice/ptrbuffer.h>
// #include <orcalogplayer/josepackets.h>
#include <orcanavutil/mapgrid.h>

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
    virtual ::orca::GpsDataPtr getData(const ::Ice::Current& ) const;    
    virtual ::orca::GpsTimeDataPtr getTimeData(const ::Ice::Current& ) const;    
    virtual ::orca::GpsMapGridDataPtr getMapGridData(const ::Ice::Current& ) const;
    virtual ::orca::GpsDescriptionPtr getDescription(const ::Ice::Current& ) const;

    // Subscribes and unsubscribes
    virtual void subscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void subscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void subscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::PtrBuffer<orca::GpsDescriptionPtr> gpsDescriptionBuffer_;
    orcaice::PtrBuffer<orca::GpsDataPtr> gpsDataBuffer_;
    orcaice::PtrBuffer<orca::GpsTimeDataPtr> gpsTimeDataBuffer_;
    orcaice::PtrBuffer<orca::GpsMapGridDataPtr> gpsMapGridDataBuffer_;
    
    orca::GpsDescriptionPtr descrPtr_;
    orca::GpsDataPtr gpsDataPtr_;
    orca::GpsTimeDataPtr gpsTimeDataPtr_;
    orca::GpsMapGridDataPtr gpsMapGridDataPtr_;
    
    virtual void initInterfaces();
    virtual void initDescriptions();

    orca::GpsConsumerPrx gpsConsumerPrx_;
    orca::GpsTimeConsumerPrx gpsTimeConsumerPrx_;
    orca::GpsMapGridConsumerPrx gpsMapGridConsumerPrx_;
    
    IceStorm::TopicPrx topic_;
    IceStorm::TopicPrx topicTimePrx_;
    IceStorm::TopicPrx topicMapGridPrx_;
    
    // binary file protocol specifics
    void loadDataIce( int index );
    void loadHeaderIce();

    void checkDescriptionFile();
    int retSize_;
    int retOffset_;

    short id_;
    mgaMapgrid mgaMapgrid_;
    
    // alexm: why this?
    orca::FQTopicName getProvidedTopicLocal( const orcaice::Context & context, const std::string &tag, const std::string & subtopic);

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
