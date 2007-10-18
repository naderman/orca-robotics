/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef PING_REPLIER_I_H
#define PING_REPLIER_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>

#include <orca/pingreply.h>

class ReplierI : public orca::Replier
{
public:

    virtual void setPinger(const ::orca::PingerPrx&, const ::Ice::Current& = ::Ice::Current());

    virtual void ping1(const ::orca::OrcaObjectPtr&, const ::Ice::Current& = ::Ice::Current());

    virtual orca::OrcaObjectPtr ping2(const ::orca::OrcaObjectPtr&, const ::Ice::Current& = ::Ice::Current());

    virtual void takeIt( const orca::PingReplyData&, const ::Ice::Current& = ::Ice::Current() );

    virtual orca::PingReplyData takeItAndReturn( const orca::PingReplyData&, const ::Ice::Current& = ::Ice::Current() );

    virtual void shutdown(const ::Ice::Current& = ::Ice::Current());

private:

    orca::PingerPrx pinger_;
};

#endif
