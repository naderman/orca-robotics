/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_POWER_I_H
#define ORCA2_SEGWAY_RMP_POWER_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/power.h>

// utilities
#include <orcaice/ptrproxy.h>

namespace segwayrmp
{

class PowerI : public orca::Power
{
public:
    PowerI( orcaice::Proxy<orca::PowerData> &power, const IceStorm::TopicPrx &topic );

    virtual ::orca::PowerData getData(const ::Ice::Current& ) const;

    virtual void subscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void unsubscribe(const ::orca::PowerConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

private:
    // the driver will put the latest data into this proxy
    orcaice::Proxy<orca::PowerData> &powerPipe_;

    // IceStorm topic to which we send our updates and cand subscribe other to
    IceStorm::TopicPrx topic_;
};

} // namespace

#endif
