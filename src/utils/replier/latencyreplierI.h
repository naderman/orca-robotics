/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef PING_LATENCY_REPLIER_I_H
#define PING_LATENCY_REPLIER_I_H

#include "pingreply.h"

class LatencyReplierI : public orca::util::LatencyReplier
{
public:

    virtual void setPinger(const ::orca::util::LatencyPingerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void takeIt( const orca::util::PingReplyData&, const ::Ice::Current& = ::Ice::Current() );
    virtual orca::util::PingReplyData takeItAndReturn( const orca::util::PingReplyData&, const ::Ice::Current& = ::Ice::Current() );
    virtual void shutdown(const ::Ice::Current& = ::Ice::Current());

private:

    orca::util::LatencyPingerPrx pinger_;
};

#endif
