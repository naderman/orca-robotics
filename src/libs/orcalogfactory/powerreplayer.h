/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_POWER_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_POWER_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/power.h>
#include <orcalog/replayer.h>

#include <orcaice/ptrbuffer.h>

namespace orcalogfactory
{

class PowerReplayer : public orca::Power, public orcalog::Replayer
{
public:
    PowerReplayer( const std::string      &format,
                    const std::string      &filename,
                    const orcaice::Context &context );
    virtual ~PowerReplayer();
    
    // from Orca object

    virtual ::orca::PowerData getData(const ::Ice::Current& ) const;
    
    virtual void subscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Buffer<orca::PowerData> dataPipe_;
    
    orca::PowerData data_;
    orca::PowerConsumerPrx publisher_;
    IceStorm::TopicPrx topic_;

    virtual void initInterfaces();
    // not needed for this interface
//     virtual void initConfigs();

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
