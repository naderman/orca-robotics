/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceUtil/Time.h>
#include "utils.h"

using namespace std;

void 
orcaprobe::reportResult( orcacm::OperationData& data, const std::string& type, const std::string& text )
{
    orcacm::ResultHeader res;
    res.name = type;
    res.text = text;
    data.results.push_back( res );
}

void 
orcaprobe::reportException( orcacm::OperationData& data, const std::string& error )
{ 
    reportResult( data, "exception", IceUtil::Time::now().toDateTime()+" "+error ); 
}

void 
orcaprobe::reportSubscribed( orcacm::OperationData& data, const std::string& proxy )
{ 
    reportResult( data, "outcome", IceUtil::Time::now().toDateTime()+" Subscribed successfully, proxy="+proxy ); 
}

void 
orcaprobe::reportUnsubscribed( orcacm::OperationData& data )
{ 
    reportResult( data, "outcome", IceUtil::Time::now().toDateTime()+" Unsubscribed successfully" ); 
}

void 
orcaprobe::reportNotImplemented( orcacm::OperationData& data )
{ 
    reportResult( data, "outcome", IceUtil::Time::now().toDateTime()+" Operation not implemented" ); 
}
