/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "latencyreplierI.h"

using namespace std;

void 
LatencyReplierI::setPinger(const orca::util::LatencyPingerPrx& pinger, const ::Ice::Current& )
{
    cout<<"setPinger"<<endl;
    pinger_ = pinger;
}

void 
LatencyReplierI::takeIt( const orca::util::PingReplyData& data, const ::Ice::Current& )
{
    pinger_->takeItBack( data );
}

orca::util::PingReplyData 
LatencyReplierI::takeItAndReturn( const orca::util::PingReplyData& data, const ::Ice::Current& )
{
    return data; 
}

void 
LatencyReplierI::shutdown(const ::Ice::Current& )
{
    cout<<"shutdown"<<endl;
}
