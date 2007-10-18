/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "replierI.h"

using namespace std;

void 
ReplierI::setPinger(const ::orca::PingerPrx& pinger, const ::Ice::Current& )
{
    cout<<"setPinger"<<endl;
    pinger_ = pinger;
}

void 
ReplierI::ping1(const ::orca::OrcaObjectPtr& obj, const ::Ice::Current& )
{
//     cout<<"ping1"<<endl;
    pinger_->callback( obj );
}

orca::OrcaObjectPtr 
ReplierI::ping2(const ::orca::OrcaObjectPtr& obj, const ::Ice::Current& )
{
//     cout<<"ping2"<<endl;
    return obj;
}

void 
ReplierI::takeIt( const orca::PingReplyData& data, const ::Ice::Current& )
{
    pinger_->takeItBack( data );
}

orca::PingReplyData 
ReplierI::takeItAndReturn( const orca::PingReplyData& data, const ::Ice::Current& )
{
    return data; 
}

void 
ReplierI::shutdown(const ::Ice::Current& )
{
    cout<<"shutdown"<<endl;
}
