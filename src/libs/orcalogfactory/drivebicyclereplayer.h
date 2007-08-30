/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCALOGFACTORY_DRIVEBICYCLE_REPLAYER_H
#define ORCA2_ORCALOGFACTORY_DRIVEBICYCLE_REPLAYER_H

#include <IceStorm/IceStorm.h>
#include <orca/drivebicycle.h>
#include <orcalog/replayer.h>
#include <orcaice/notify.h>

#include <orcaice/proxy.h>

namespace orcalogfactory
{

class DriveBicycleReplayer : public orca::DriveBicycle, 
                             public IceUtil::Shared,
                             public orcaice::Notify<orca::DriveBicycleCommand>,
                             public orcalog::Replayer
{
public:
    DriveBicycleReplayer( const std::string      &format,
                    const std::string      &filename,
                    const orcaice::Context &context );
    virtual ~DriveBicycleReplayer();
    
    // from Orca object

    virtual ::orca::DriveBicycleData getData(const ::Ice::Current& ) const;
    
    virtual orca::VehicleDescription getDescription(const Ice::Current& c) const;
    
    virtual void setCommand(const ::orca::DriveBicycleCommand&, const Ice::Current& c);
    
    virtual void subscribe(const ::orca::DriveBicycleConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::DriveBicycleConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // from Replayer

    virtual void replayData( int index, bool isTest=false );

private:
    orcaice::Proxy<orca::DriveBicycleData> dataPipe_;
    orcaice::Proxy<orca::VehicleDescription> descriptionPipe;
    
    orca::DriveBicycleData data_;
    orca::DriveBicycleConsumerPrx publisher_;
    IceStorm::TopicPrx topicPrx_;

    virtual void initInterfaces();
    virtual void initDescription();

    // this is a counter of instaces of this type, used for registered interface name
    static int _counter;
};

} // namespace

#endif
