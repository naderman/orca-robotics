#if 0
/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_IFACE_H
#define ORCA2_GPS_IFACE_H

#include <orca/gps.h>
#include <IceStorm/IceStorm.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace gps {

//
// Implements the Gps interface: Handles remote calls.
//
class GpsIface : public IceUtil::Shared
{
public:
    GpsIface( const std::string &ifaceTag,
              const orca::GpsDescription &descr,
              const orcaice::Context &context );
    ~GpsIface();

    // remote calls:

    // Get GPS Data
    ::orca::GpsData getData() const;

    // Get GPS Map Grid Data
    virtual ::orca::GpsMapGridData getMapGridData(const ::Ice::Current& ) const;

    // Get GPS Time Data
    virtual ::orca::GpsTimeData getTimeData(const ::Ice::Current& ) const;

    // Get Gps Description
    virtual ::orca::GpsDescription getDescription(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void subscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void subscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Local calls:

    // may throw orcaice::Exceptions
    void initInterface();

    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSendData( const orca::GpsData &data );
    void localSetAndSendTimeData( const orca::GpsData &data );
    void localSetAndSendMapGridData( const orca::GpsData &data );

private:

    orcaice::Proxy<orca::GpsData>        dataProxy_;
    orcaice::Proxy<orca::GpsMapGridData> mapGridDataProxy_;
    orcaice::Proxy<orca::GpsTimeData>    timeDataProxy_;

    orca::GpsConsumerPrx         consumerPrx_;
    orca::GpsMapGridConsumerPrx  mapGridConsumerPrx_;
    orca::GpsTimeConsumerPrx     timeConsumerPrx_;

    IceStorm::TopicPrx      topicPrx_;
    IceStorm::TopicPrx      mapGridTopicPrx_;
    IceStorm::TopicPrx      timeTopicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string                 ifaceTag_;
    const orca::GpsDescription        description_;
    orcaice::Context                  context_;
};
typedef IceUtil::Handle<GpsIface> GpsIfacePtr;

} // namespace

#endif
#endif
