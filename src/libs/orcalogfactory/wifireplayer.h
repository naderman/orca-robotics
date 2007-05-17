/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_WIFI_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_WIFI_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/wifi.h>
#include <orcalog/replayer.h>

#include <orcaice/ptrbuffer.h>

namespace orcalogfactory
{

class WifiReplayer : public orca::Wifi, public orcalog::Replayer
{
public:
    WifiReplayer( const std::string      &format,
                    const std::string      &filename,
                    const orcaice::Context &context );
    virtual ~WifiReplayer();
    
    // from Orca object

    virtual ::orca::WifiData getData(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::WifiConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::WifiConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Buffer<orca::WifiData> dataPipe_;
    
    orca::WifiData data_;
    orca::WifiConsumerPrx publisher_;
    IceStorm::TopicPrx topicPrx_;

    virtual void initInterfaces();
    // not needed for this interface
    virtual void initDescription() {};

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
